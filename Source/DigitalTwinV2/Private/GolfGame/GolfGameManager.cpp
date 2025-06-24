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
#include "GolfGame/UI/GolfStartMenu.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "GolfGame/UI/GolfEndGameMenu.h"

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

	TArray<AActor*> FoundStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), StartActorClass, FoundStarts);
	if (FoundStarts.Num() > 0)
	if (GolfStartMenuClass)
	{
		UGolfStartMenu* StartMenu = CreateWidget<UGolfStartMenu>(GetWorld(), GolfStartMenuClass);
		if (StartMenu)
		{
			StartMenu->AddToViewport();
		}
	}

	if (FollowCameraActor)
	{
		FollowCameraActor->SetActorLocation(MenuCameraTransform.GetLocation());
		FollowCameraActor->SetActorRotation(MenuCameraTransform.GetRotation().Rotator());
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


	if (bShouldFollowBall && SpawnedBall && FollowCameraActor)
	{
		FVector DesiredLocation = SpawnedBall->GetActorLocation() + BallFollowOffset;

		FollowCameraActor->SetActorLocation(FMath::VInterpTo(
			FollowCameraActor->GetActorLocation(),
			DesiredLocation,
			DeltaTime,
			5.0f
		));

		// ✅ Do NOT update rotation here
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
	TArray<FVector> Arc = CalculateArcPoints(StartLocation, LaunchVelocity, 35, 0.1f);
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

void AGolfGameManager::OnMouseReleaseAndResumeMontage()
{
	// Delay Shoot() slightly to ensure animation resumes before shot logic
	GetWorldTimerManager().SetTimer(AfterShotDelayHandle, this, &AGolfGameManager::Shoot, 0.05f, false);
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

	if (SpawnedBall && TrailSystemTemplate)
	{
		UNiagaraComponent* Trail = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TrailSystemTemplate,
			SpawnedBall->GetRootComponent(),
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			true  // AutoDestroy
		);
	}

	UpdateTrajectorySpline({});
	TimeSinceShot = 0.0f;
	bIsWaitingForBallToStop = true;
	BeginCameraTransitionToBallFollow();
	CurrentPower = 0;
	bHasAdjusted = false;
	RotationAngle = 0.0f;
	// bShouldFollowBall = true;
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

	for (int32 i = 1; i < Points.Num(); ++i)
	{
		TrajectorySpline->AddPoint(FSplinePoint(i, Points[i]), false);

		if (SphereMesh && SpawnedBall)
		{
			FVector Start = Points[i - 1];
			FVector End = Points[i];
			FVector Segment = End - Start;
			float Length = Segment.Size();
			FVector MidPoint = (Start + End) * 0.5f;
			FVector Direction = Segment.GetSafeNormal();

			UStaticMeshComponent* TubeSegment = NewObject<UStaticMeshComponent>(SpawnedBall);
			TubeSegment->SetStaticMesh(SphereMesh);

			// Stretch along Z, assuming the mesh's up axis is Z
			TubeSegment->SetWorldScale3D(FVector(0.05f, 0.05f, Length * 0.01f)); // tweak 0.005f based on your mesh height

			// Set position at midpoint
			TubeSegment->SetWorldLocation(MidPoint);

			// Align cylinder Z-axis to direction vector
			FRotator Rotation = FRotationMatrix::MakeFromZ(Direction).Rotator();
			TubeSegment->SetWorldRotation(Rotation);

			TubeSegment->RegisterComponent();
			TubeSegment->AttachToComponent(SpawnedBall->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);

			if (SphereMaterial)
				TubeSegment->SetMaterial(0, SphereMaterial);

			TrajectorySpheres.Add(TubeSegment);
		}
	}

	TrajectorySpline->UpdateSpline();
}


void AGolfGameManager::AfterShotSequence()
{
    if (!SpawnedBall || !GolfPlayer) return;

    FVector FinalLocation = SpawnedBall->GetActorLocation();
    FRotator FinalRotation = SpawnedBall->GetActorRotation();
    SpawnedBall->Destroy();
    SpawnedBall = nullptr;

    if (CurrentStartActor)
    {
        CurrentStartActor->SetActorLocation(FinalLocation);
        CurrentStartActor->SetActorRotation(FinalRotation);
    }

    // Move GolfPlayer to 200 units behind and 100 units left of the ball
    FVector BallForward = CurrentStartActor->GetActorForwardVector();
    FVector BallLeft = FRotationMatrix(CurrentStartActor->GetActorRotation()).GetUnitAxis(EAxis::Y) * -1.0f;
    FVector MoveToLoc = FinalLocation - BallForward * 200.0f + BallLeft * 100.0f;
    GolfPlayer->MoveTo(MoveToLoc);

    SpawnBallAtCurrentPosition();
    bIsWaitingForBallToStop = false;
	bShouldFollowBall = false;
	BeginCameraTransitionToBall();
}


void AGolfGameManager::SpawnBallAtCurrentPosition()
{
    if (!BallActorClass || !CurrentStartActor) return;
    if (SpawnedBall) SpawnedBall->Destroy();

	AActor* NearestTarget = nullptr;
	float MinDistanceSqr = TNumericLimits<float>::Max();
	FVector StartLocation = CurrentStartActor->GetActorLocation() + FVector(0, 0, 3.0f);

	// Find nearest target
	for (AActor* Target : AllTargets)
	{
		if (!Target) continue;

		float DistanceSqr = FVector::DistSquared(StartLocation, Target->GetActorLocation());
		if (DistanceSqr < MinDistanceSqr)
		{
			MinDistanceSqr = DistanceSqr;
			NearestTarget = Target;
		}
	}

	// Face toward nearest target if one is found
	FRotator NewRotation = FRotator::ZeroRotator;
	if (NearestTarget)
	{
		FVector ToTarget = (NearestTarget->GetActorLocation() - StartLocation).GetSafeNormal();
		NewRotation = ToTarget.Rotation();
		CurrentStartActor->SetActorRotation(NewRotation);
	}

	// Spawn the ball
	FActorSpawnParameters SpawnParams;
	SpawnedBall = GetWorld()->SpawnActor<AActor>(BallActorClass, StartLocation, NewRotation, SpawnParams);

	// Add trajectory spline
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
	UE_LOG(LogTemp, Warning, TEXT("[GolfGameManager] StartGameSequence called"));
    SpawnBallAtCurrentPosition();

	// Start smooth camera move
	BeginCameraTransitionToBall();
	bHasStarted = true;
	if (GolfGameUIClass)
	{
		GolfGameUIInstance = CreateWidget<UUserWidget>(GetWorld(), GolfGameUIClass);
		if (GolfGameUIInstance) GolfGameUIInstance->AddToViewport();
	}

	if (GolfPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GolfGameManager] GolfPlayer is %s"), GolfPlayer ? TEXT("VALID") : TEXT("NULL"));
        APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
        if (PC)
        {
            PC->Possess(GolfPlayer);
            UE_LOG(LogTemp, Warning, TEXT("[GolfGameManager] PlayerController now possesses GolfPlayer: %s"), *GolfPlayer->GetName());
            // Bind to the delegate so we know when the player reaches the start
            GolfPlayer->OnReachedStartPosition.AddDynamic(this, &AGolfGameManager::OnGolfPlayerReachedStart);
            // Move player to start position
            if (CurrentStartActor)
            {
                GolfPlayer->MoveTo(CurrentStartActor->GetActorLocation());
            }
        }
	}
}


void AGolfGameManager::OnGolfPlayerReachedStart()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (PC)
    {
        PC->UnPossess(); // Only unpossess, do not possess the manager
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

	BeginCameraTransitionToBall();
}

void AGolfGameManager::EndGame()
{
	if (!bHasStarted) return;

	bHasStarted = false; // ✅ Prevent further gameplay
	bIsWaitingForBallToStop = false;
	bIsAdjustingShot = false;
	bHasAdjusted = false;

	FTimerHandle EndGameTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		EndGameTimerHandle,
		[this]()
		{
			// Remove in-game UI
			if (GolfGameUIInstance)
			{
				GolfGameUIInstance->RemoveFromParent();
				GolfGameUIInstance = nullptr;
			}

			// Show end menu
			if (GolfEndMenuClass)
			{
				UGolfEndGameMenu* EndMenu = CreateWidget<UGolfEndGameMenu>(GetWorld(), GolfEndMenuClass);
				if (EndMenu)
				{
					EndMenu->AddToViewport();
				}
			}
		},
		3.0f,
		false
	);
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
void AGolfGameManager::BeginCameraTransitionToBall()
{
	if (!FollowCameraActor || !CurrentStartActor) return;

	CameraStartLocation = FollowCameraActor->GetActorLocation();
	CameraStartRotation = FollowCameraActor->GetActorRotation();

	// ✅ Offset relative to direction the ball is facing
	FVector Forward = CurrentStartActor->GetActorForwardVector();
	FVector Up = FVector::UpVector;
	FVector Right = FVector::CrossProduct(Up, Forward);

	// You can tweak these values
	float BackDistance = 600.0f;
	float Height = 200.0f;
	float SideOffset = 0.0f;

	// Construct the dynamic camera offset based on direction
	CameraDynamicOffset = -Forward * BackDistance + Up * Height + Right * SideOffset;

	// Reset lerp alpha
	CameraLerpAlpha = 0.0f;

	// Begin interpolating
	GetWorld()->GetTimerManager().SetTimer(CameraLerpTimer, this, &AGolfGameManager::UpdateCameraLerp, 0.01f, true);
}



void AGolfGameManager::UpdateCameraLerp()
{
	if (!FollowCameraActor || !CurrentStartActor)
	{
		GetWorld()->GetTimerManager().ClearTimer(CameraLerpTimer);
		return;
	}

	CameraLerpAlpha += GetWorld()->GetDeltaSeconds() / CameraLerpDuration;
	CameraLerpAlpha = FMath::Clamp(CameraLerpAlpha, 0.0f, 1.0f);

	// Apply easing function
	float Alpha = FMath::InterpEaseInOut(0.0f, 1.0f, CameraLerpAlpha, 2.5f); // You can tweak the exponent

	FVector TargetLocation = CurrentStartActor->GetActorLocation() + CameraDynamicOffset;
	FRotator TargetRotation = CurrentStartActor->GetActorRotation(); // ✅ Face the same way the ball does

	FollowCameraActor->SetActorLocation(FMath::Lerp(CameraStartLocation, TargetLocation, Alpha));

	// Smooth rotation
	FQuat StartQuat = CameraStartRotation.Quaternion();
	FQuat EndQuat = TargetRotation.Quaternion();
	FollowCameraActor->SetActorRotation(FQuat::Slerp(StartQuat, EndQuat, Alpha));


	if (CameraLerpAlpha >= 1.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(CameraLerpTimer);
		bHasStarted = true;
	}
}




void AGolfGameManager::BeginCameraTransitionToBallFollow()
{
	if (!FollowCameraActor || !SpawnedBall) return;

	CameraStartLocation = FollowCameraActor->GetActorLocation();
	CameraStartRotation = FollowCameraActor->GetActorRotation();

	FVector BallForward = SpawnedBall->GetActorForwardVector();
	FVector BallUp = FVector::UpVector;
	FVector BallRight = FVector::CrossProduct(BallUp, BallForward);

	// Closer follow offset
	float BackDistance = 300.0f;
	float Height = 100.0f;

	// Save follow offset for later tracking
	BallFollowOffset = -BallForward * BackDistance + BallUp * Height;

	FVector TargetLocation = SpawnedBall->GetActorLocation() + BallFollowOffset;
	FRotator TargetRotation = BallForward.Rotation();

	CameraDynamicOffset = BallFollowOffset; // So `UpdateCameraLerp()` works as-is
	CameraLerpAlpha = 0.0f;

	GetWorld()->GetTimerManager().SetTimer(CameraLerpTimer, this, &AGolfGameManager::UpdateCameraLerpToFollow, 0.016f, true);
}



void AGolfGameManager::UpdateCameraLerpToFollow()
{
	if (!FollowCameraActor || !SpawnedBall)
	{
		GetWorld()->GetTimerManager().ClearTimer(CameraLerpTimer);
		return;
	}

	CameraLerpAlpha += GetWorld()->GetDeltaSeconds() / CameraLerpFollowDuration;
	CameraLerpAlpha = FMath::Clamp(CameraLerpAlpha, 0.0f, 1.0f);
	float Alpha = FMath::InterpEaseInOut(0.0f, 1.0f, CameraLerpAlpha, 2.5f);

	FVector TargetLocation = SpawnedBall->GetActorLocation() + BallFollowOffset;

	// Only interpolate position
	FollowCameraActor->SetActorLocation(FMath::Lerp(CameraStartLocation, TargetLocation, Alpha));

	// ✅ Keep rotation unchanged (just use the starting one)
	FollowCameraActor->SetActorRotation(CameraStartRotation);

	if (CameraLerpAlpha >= 1.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(CameraLerpTimer);
		bShouldFollowBall = true; // Start following position
	}
}
