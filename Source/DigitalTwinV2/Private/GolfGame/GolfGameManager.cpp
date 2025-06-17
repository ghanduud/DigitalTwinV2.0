// Refactored GolfGameManager.cpp
// Cleaned up duplicate logic, improved organization, grouped by feature

#include "GolfGame/GolfGameManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "GolfGame/HoleData.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Blueprint/UserWidget.h"

AGolfGameManager* AGolfGameManager::Instance = nullptr;

AGolfGameManager* AGolfGameManager::Get()
{
	return Instance;
}

AGolfGameManager::AGolfGameManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGolfGameManager::BeginPlay()
{
	Super::BeginPlay();
	Instance = this;
	InitializeHoles();
	SpawnBallAtCurrentPosition();
	StartGameSequence();
}

void AGolfGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bIsWaitingForBallToStop || !SpawnedBall) return;

	TimeSinceShot += DeltaTime;
	if (TimeSinceShot < WaitBeforeCheckingStop) return;

	UPrimitiveComponent* BallRoot = Cast<UPrimitiveComponent>(SpawnedBall->GetRootComponent());
	if (BallRoot && BallRoot->GetComponentVelocity().Size() < StopVelocityThreshold)
	{
		AfterShotSequence();
	}
}

void AGolfGameManager::InitializeHoles()
{
	Holes.Empty();
	UWorld* World = GetWorld();
	if (!World) return;

	TArray<AActor*> StartActors, TargetActors, BoundsActors;
	UGameplayStatics::GetAllActorsOfClass(World, StartActorClass, StartActors);
	UGameplayStatics::GetAllActorsOfClass(World, TargetActorClass, TargetActors);
	UGameplayStatics::GetAllActorsOfClass(World, BoundsActorClass, BoundsActors);

	for (int32 i = 1; i <= TotalHoles; ++i)
	{
		FName StartTag = FName(*FString::Printf(TEXT("Hole%d_Start"), i));
		FName TargetTag = FName(*FString::Printf(TEXT("Hole%d_Target"), i));
		FName BoundsTag = FName(*FString::Printf(TEXT("Hole%d_Bounds"), i));

		AActor* Start = FindTaggedActor(StartActors, StartTag);
		AActor* Target = FindTaggedActor(TargetActors, TargetTag);
		AActor* Bounds = FindTaggedActor(BoundsActors, BoundsTag);

		if (Start && Target && Bounds)
		{
			Holes.Add(FHoleData(i, Start, Target, Bounds));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Missing actors for Hole %d"), i);
		}
	}

	if (Holes.Num() > 0)
	{
		CurrentHoleIndex = 0;
		CurrentStartActor = Holes[0].StartPoint;
		CurrentTargetActor = Holes[0].HoleTarget;
	}
}

AActor* AGolfGameManager::FindTaggedActor(const TArray<AActor*>& Actors, const FName& Tag)
{
	for (AActor* Actor : Actors)
	{
		if (Actor && Actor->Tags.Contains(Tag)) return Actor;
	}
	return nullptr;
}

void AGolfGameManager::BeginAdjustShot()
{
	if (!bHasStarted || bIsWaitingForBallToStop) return;
	bIsAdjustingShot = true;
	bHasAdjusted = false;
	AccumulatedInput = FVector2D::ZeroVector;
	CurrentInputDirection = FVector2D::ZeroVector;
	OriginalStartRotation = CurrentStartActor->GetActorRotation();
	RotationAngle = 0.0f;
}

void AGolfGameManager::AdjustShot(const FVector2D& Delta)
{
	if (!bHasStarted || bIsWaitingForBallToStop)
		return;

	// 🔁 Restart shot if swiping down again after canceling
	if (!bIsAdjustingShot)
	{
		// ⛔ Ignore upward movement — only accumulate downward input
		if (Delta.Y < 0)
			AccumulatedInput.Y += Delta.Y;

		if (AccumulatedInput.Y < -MinInputDistance)
		{
			AccumulatedInput = FVector2D::ZeroVector; // Reset cleanly
			BeginAdjustShot();
		}
		return;
	}

	// Normal shot logic
	AccumulatedInput += Delta;

	if (!bHasAdjusted && FMath::Abs(AccumulatedInput.Y) >= MinInputDistance)
		bHasAdjusted = true;
	if (!bHasAdjusted)
		return;

	// Cancel shot if pulled back up
	if (AccumulatedInput.Y > -MinInputDistance * 0.1f)
	{
		CancelShotAdjust();
		return;
	}

	CurrentPower = FMath::Clamp(CurrentPower - Delta.Y * 0.1f, 0.0f, 1.0f);
	RotationAngle = FMath::Clamp(RotationAngle - Delta.X, MinRotation, MaxRotation);

	if (CurrentStartActor)
	{
		FRotator NewRot = OriginalStartRotation;
		NewRot.Yaw += RotationAngle;
		CurrentStartActor->SetActorRotation(NewRot);
	}

	const FVector StartLocation = SpawnedBall ? SpawnedBall->GetActorLocation() : CurrentStartActor->GetActorLocation();
	const FVector LaunchVelocity = ComputeLaunchVelocity();
	const TArray<FVector> Arc = (CurrentShotType == EShotType::ChipShot)
		? TArray<FVector>{ StartLocation, StartLocation + LaunchVelocity }
	: CalculateArcPoints(StartLocation, LaunchVelocity, 30, 0.1f);

	UpdateTrajectorySpline(Arc);
}




void AGolfGameManager::CancelShotAdjust()
{
	bIsAdjustingShot = false;
	bHasAdjusted = false;
	RotationAngle = 0.0f;
	AccumulatedInput = FVector2D::ZeroVector;
	UpdateTrajectorySpline({});
}

void AGolfGameManager::Shoot()
{
	if (!bHasStarted || !bIsAdjustingShot || !bHasAdjusted || bIsWaitingForBallToStop) return;

	bIsAdjustingShot = false;
	FVector LaunchVelocity = ComputeLaunchVelocity();

	if (SpawnedBall)
	{
		if (UPrimitiveComponent* BallRoot = Cast<UPrimitiveComponent>(SpawnedBall->GetRootComponent()))
		{
			BallRoot->AddImpulse(LaunchVelocity, NAME_None, true);
		}
	}

	UpdateTrajectorySpline({});
	TimeSinceShot = 0.0f;
	bIsWaitingForBallToStop = true;
}

FVector AGolfGameManager::ComputeLaunchVelocity() const
{
	if (!CurrentStartActor) return FVector::ZeroVector;

	float PowerScalar = 0.0f;
	FVector UpwardBoost = FVector::ZeroVector;

	switch (CurrentShotType)
	{
	case EShotType::LongShot:
		PowerScalar = LongShotPower;
		UpwardBoost = FVector(0, 0, 1000);
		break;
	case EShotType::ShortShot:
		PowerScalar = ShortShotPower;
		UpwardBoost = FVector(0, 0, 500);
		break;
	case EShotType::ChipShot:
		PowerScalar = ChipShotPower;
		break;
	default:
		break;
	}

	return CurrentStartActor->GetActorForwardVector().GetSafeNormal() * (CurrentPower * PowerScalar) + UpwardBoost;
}

TArray<FVector> AGolfGameManager::CalculateArcPoints(const FVector& Start, const FVector& Velocity, int Steps, float TimeStep)
{
	TArray<FVector> Points;
	FVector Gravity(0, 0, GetWorld()->GetGravityZ());
	FVector Position = Start;
	FVector CurrentVelocity = Velocity;

	for (int i = 0; i < Steps; ++i)
	{
		Points.Add(Position);
		Position += CurrentVelocity * TimeStep;
		CurrentVelocity += Gravity * TimeStep;
	}
	return Points;
}

void AGolfGameManager::UpdateTrajectorySpline(const TArray<FVector>& Points)
{
	if (!TrajectorySpline) return;

	TrajectorySpline->ClearSplinePoints();
	for (UStaticMeshComponent* Sphere : TrajectorySpheres)
		if (Sphere) Sphere->DestroyComponent();
	TrajectorySpheres.Empty();

	for (int32 i = 0; i < Points.Num(); ++i)
	{
		TrajectorySpline->AddPoint(FSplinePoint(i, Points[i]), false);
		if (SphereMesh && SpawnedBall)
		{
			UStaticMeshComponent* Sphere = NewObject<UStaticMeshComponent>(SpawnedBall);
			Sphere->SetStaticMesh(SphereMesh);
			Sphere->SetWorldLocation(Points[i]);
			Sphere->SetWorldScale3D(FVector(0.0427f));
			Sphere->RegisterComponent();
			Sphere->AttachToComponent(SpawnedBall->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
			if (SphereMaterial) Sphere->SetMaterial(0, SphereMaterial);
			TrajectorySpheres.Add(Sphere);
		}
	}
	TrajectorySpline->UpdateSpline();
}

void AGolfGameManager::AfterShotSequence()
{
	if (!SpawnedBall) return;

	FVector FinalLocation = SpawnedBall->GetActorLocation();
	FRotator FinalRotation = SpawnedBall->GetActorRotation();
	SpawnedBall->Destroy();
	SpawnedBall = nullptr;

	if (CurrentStartActor)
	{
		CurrentStartActor->SetActorLocation(FinalLocation);
		CurrentStartActor->SetActorRotation(FinalRotation);
	}

	SpawnBallAtCurrentPosition();
	bIsAdjustingShot = false;
	bHasAdjusted = false;
	RotationAngle = 0.0f;
	bIsWaitingForBallToStop = false;
}

void AGolfGameManager::SpawnBallAtCurrentPosition()
{
	if (!BallActorClass || !CurrentStartActor || !CurrentTargetActor) return;

	if (SpawnedBall) SpawnedBall->Destroy();

	FVector ToTarget = (CurrentTargetActor->GetActorLocation() - CurrentStartActor->GetActorLocation()).GetSafeNormal();
	FRotator NewRotation = ToTarget.Rotation();
	CurrentStartActor->SetActorRotation(NewRotation);

	FActorSpawnParameters SpawnParams;
	SpawnedBall = GetWorld()->SpawnActor<AActor>(BallActorClass, CurrentStartActor->GetActorLocation(), NewRotation, SpawnParams);

	if (SpawnedBall)
	{
		USplineComponent* BallSpline = NewObject<USplineComponent>(SpawnedBall, TEXT("TrajectorySpline"));
		if (BallSpline)
		{
			BallSpline->RegisterComponent();
			BallSpline->AttachToComponent(SpawnedBall->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			BallSpline->SetMobility(EComponentMobility::Movable);
			TrajectorySpline = BallSpline;
		}
	}
}

void AGolfGameManager::StartGameSequence()
{
	bHasStarted = false;
	bHasStarted = true;

	if (GolfGameUIClass)
	{
		GolfGameUIInstance = CreateWidget<UUserWidget>(GetWorld(), GolfGameUIClass);
		if (GolfGameUIInstance) GolfGameUIInstance->AddToViewport();
	}
}

FHoleData AGolfGameManager::GetCurrentHoleData() const
{
	return Holes.IsValidIndex(CurrentHoleIndex) ? Holes[CurrentHoleIndex] : FHoleData();
}

void AGolfGameManager::SetShotTypeToLong() { CurrentShotType = EShotType::LongShot; }
void AGolfGameManager::SetShotTypeToShort() { CurrentShotType = EShotType::ShortShot; }
void AGolfGameManager::SetShotTypeToChip() { CurrentShotType = EShotType::ChipShot; }