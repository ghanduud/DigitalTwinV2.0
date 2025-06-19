// Cleaned and streamlined GolfGameManager.cpp
// Removed unused features (short shot, multiple holes, scoring)

#include "GolfGame/GolfGameManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraActor.h"
#include "EngineUtils.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

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

	// Bind AnimNotify
	if (ThirdCharacter && ThirdCharacter->GetMesh())
	{
		UAnimInstance* AnimInstance = ThirdCharacter->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &AGolfGameManager::HandleAnimNotify_SpawnBall);
		}
	}

	TArray<AActor*> FoundStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), StartActorClass, FoundStarts);
	if (FoundStarts.Num() > 0)
	{
		CurrentStartActor = FoundStarts[0];
	}

	TArray<AActor*> FoundBounds;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), BoundsActorClass, FoundBounds);
	if (FoundBounds.Num() > 0)
	{
		BoundsActor = FoundBounds[0];
	}

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), TargetActorClass, AllTargets);

	SpawnBallAtCurrentPosition();
	StartGameSequence();

	if (!FollowCameraActor)
	{
		FollowCameraActor = GetWorld()->SpawnActor<AActor>(AActor::StaticClass());
		if (FollowCameraActor)
		{
			FollowCameraActor->SetActorLocation(FVector::ZeroVector);
		}
	}
}

void AGolfGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FollowCameraActor && SpawnedBall)
	{
		FVector BallLocation = SpawnedBall->GetActorLocation();
		FVector CameraLocation = BallLocation + CameraOffset;
		FollowCameraActor->SetActorLocation(CameraLocation);
	}

	if (!bIsWaitingForBallToStop || !SpawnedBall) return;

	TimeSinceShot += DeltaTime;
	if (TimeSinceShot < WaitBeforeCheckingStop) return;

	UPrimitiveComponent* BallRoot = Cast<UPrimitiveComponent>(SpawnedBall->GetRootComponent());
	if (BallRoot && BallRoot->GetComponentVelocity().Size() < StopVelocityThreshold)
	{
		AfterShotSequence();
	}

	// Out of bounds
	if (BoundsActor)
	{
		UBoxComponent* Box = BoundsActor->FindComponentByClass<UBoxComponent>();
		if (Box)
		{
			FVector LocalPos = Box->GetComponentTransform().InverseTransformPosition(SpawnedBall->GetActorLocation());
			FVector Extent = Box->GetUnscaledBoxExtent();
			if (FMath::Abs(LocalPos.X) > Extent.X || FMath::Abs(LocalPos.Y) > Extent.Y || FMath::Abs(LocalPos.Z) > Extent.Z)
			{
				HandleBallOutOfBounds();
				return;
			}
		}
	}

	// Entered hole
	for (AActor* TargetActor : AllTargets)
	{
		if (!TargetActor) continue;
		UBoxComponent* TargetBox = TargetActor->FindComponentByClass<UBoxComponent>();
		if (!TargetBox) continue;

		FVector LocalPos = TargetBox->GetComponentTransform().InverseTransformPosition(SpawnedBall->GetActorLocation());
		FVector Extent = TargetBox->GetUnscaledBoxExtent();

		if (FMath::Abs(LocalPos.X) <= Extent.X && FMath::Abs(LocalPos.Y) <= Extent.Y && FMath::Abs(LocalPos.Z) <= Extent.Z)
		{
			EndGame();
			return;
		}
	}
}

void AGolfGameManager::BeginAdjustShot()
{
	if (!bHasStarted || bIsWaitingForBallToStop) return;
	bIsAdjustingShot = true;
	bHasAdjusted = false;
	AccumulatedInput = FVector2D::ZeroVector;
	OriginalStartRotation = CurrentStartActor->GetActorRotation();
	RotationAngle = 0.0f;

	if (ThirdCharacter)
	{
		ThirdCharacter->CallFunctionByNameWithArguments(TEXT("PlayMontageToHold"), *GLog, nullptr, true);
	}
}

void AGolfGameManager::AdjustShot(const FVector2D& Delta)
{
	if (!bHasStarted || bIsWaitingForBallToStop) return;

	if (!bIsAdjustingShot)
	{
		if (Delta.Y < 0) AccumulatedInput.Y += Delta.Y;
		if (AccumulatedInput.Y < -MinInputDistance)
		{
			AccumulatedInput = FVector2D::ZeroVector;
			BeginAdjustShot();
		}
		return;
	}

	AccumulatedInput += Delta;
	if (!bHasAdjusted && FMath::Abs(AccumulatedInput.Y) >= MinInputDistance) bHasAdjusted = true;
	if (!bHasAdjusted) return;
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

	FVector StartLocation = SpawnedBall ? SpawnedBall->GetActorLocation() : CurrentStartActor->GetActorLocation();
	FVector LaunchVelocity = ComputeLaunchVelocity();
	TArray<FVector> Arc = CalculateArcPoints(StartLocation, LaunchVelocity, 30, 0.1f);
	UpdateTrajectorySpline(Arc);
}

void AGolfGameManager::CancelShotAdjust()
{
	bIsAdjustingShot = false;
	bHasAdjusted = false;
	RotationAngle = 0.0f;
	AccumulatedInput = FVector2D::ZeroVector;
	CurrentPower = 0;
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
			BallRoot->SetPhysicsLinearVelocity(LaunchVelocity, false);
		}
	}

	if (ThirdCharacter)
	{
		if (CurrentShotType == EShotType::ChipShot)
		{
			ThirdCharacter->CallFunctionByNameWithArguments(TEXT("ChipShot"), *GLog, nullptr, true);
		}
		else
		{
			ThirdCharacter->CallFunctionByNameWithArguments(TEXT("ResumeMontage"), *GLog, nullptr, true);
		}
	}

	UpdateTrajectorySpline({});
	TimeSinceShot = 0.0f;
	bIsWaitingForBallToStop = true;
	CurrentPower = 0;
	bHasAdjusted = false;
	RotationAngle = 0.0f;
}

FVector AGolfGameManager::ComputeLaunchVelocity() const
{
	if (!CurrentStartActor) return FVector::ZeroVector;

	FVector Forward = CurrentStartActor->GetActorForwardVector().GetSafeNormal();

	if (CurrentShotType == EShotType::ChipShot)
	{
		return Forward * (CurrentPower * ChipShotPower);
	}
	else // LongShot
	{
		return Forward * (CurrentPower * LongShotPower) + FVector(0, 0, 1000);
	}
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
    if (!SpawnedBall || !ThirdCharacter) return;

    FVector FinalLocation = SpawnedBall->GetActorLocation();
    FRotator FinalRotation = SpawnedBall->GetActorRotation();
    SpawnedBall->Destroy();
    SpawnedBall = nullptr;

    if (CurrentStartActor)
    {
        CurrentStartActor->SetActorLocation(FinalLocation);
        CurrentStartActor->SetActorRotation(FinalRotation);
    }

    // Always move BP_Third to 60 units behind the ball, matching the ball's ground height
    FVector BallForward = CurrentStartActor->GetActorForwardVector();
    FVector BehindBallLocation = FinalLocation - BallForward * 60.0f;
    BehindBallLocation.Z = FinalLocation.Z; // Match ball's Z (height)
    ThirdCharacter->SetActorLocation(BehindBallLocation);

    SpawnBallAtCurrentPosition();
    bIsWaitingForBallToStop = false; // ✅ Reset here
}


void AGolfGameManager::SpawnBallAtCurrentPosition()
{
    if (!BallActorClass || !CurrentStartActor) return;
    if (SpawnedBall) SpawnedBall->Destroy();

    FRotator NewRotation = FRotator::ZeroRotator;
    if (AllTargets.Num() > 0)
    {
        FVector ToTarget = (AllTargets[0]->GetActorLocation() - CurrentStartActor->GetActorLocation()).GetSafeNormal();
        NewRotation = ToTarget.Rotation();
        CurrentStartActor->SetActorRotation(NewRotation);
    }

    FActorSpawnParameters SpawnParams;
    SpawnedBall = GetWorld()->SpawnActor<AActor>(BallActorClass, CurrentStartActor->GetActorLocation(), NewRotation, SpawnParams);

    if (SpawnedBall && ThirdCharacter)
    {
        // Prevent collision between BP_Third and the spawned ball
        TArray<UPrimitiveComponent*> BallComponents;
        SpawnedBall->GetComponents<UPrimitiveComponent>(BallComponents);
        for (UPrimitiveComponent* BallComp : BallComponents)
        {
            BallComp->IgnoreActorWhenMoving(ThirdCharacter, true);
        }

        TArray<UPrimitiveComponent*> ThirdComponents;
        ThirdCharacter->GetComponents<UPrimitiveComponent>(ThirdComponents);
        for (UPrimitiveComponent* ThirdComp : ThirdComponents)
        {
            ThirdComp->IgnoreActorWhenMoving(SpawnedBall, true);
        }
    }

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
	bHasStarted = true;
	if (GolfGameUIClass)
	{
		GolfGameUIInstance = CreateWidget<UUserWidget>(GetWorld(), GolfGameUIClass);
		if (GolfGameUIInstance) GolfGameUIInstance->AddToViewport();
	}
}

void AGolfGameManager::HandleBallOutOfBounds()
{
	if (SpawnedBall)
	{
		SpawnedBall->Destroy();
		SpawnedBall = nullptr;
	}
	SpawnBallAtCurrentPosition();
	bIsAdjustingShot = false;
	bHasAdjusted = false;
	RotationAngle = 0.0f;
	bIsWaitingForBallToStop = false;
	TimeSinceShot = 0.0f;
}

void AGolfGameManager::EndGame()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}


void AGolfGameManager::SetShotTypeToLong() { CurrentShotType = EShotType::LongShot; }
void AGolfGameManager::SetShotTypeToChip() { CurrentShotType = EShotType::ChipShot; }

void AGolfGameManager::HandleAnimNotify_SpawnBall(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
    this->SpawnBallAtCurrentPosition();
    FVector LaunchVelocity = this->ComputeLaunchVelocity();
    if (this->SpawnedBall)
    {
        this->SpawnedBall->GetRootComponent()->ComponentVelocity = LaunchVelocity;
    }
}
