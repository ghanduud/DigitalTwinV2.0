#include "Camera/BirdEye.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SceneComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Buldings/Bulding.h"
#include "Framework/Application/SlateApplication.h"
#include "UI/TwinUiManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABirdEye::ABirdEye()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void ABirdEye::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = GetWorld()->GetFirstPlayerController();

	if (PlayerController)
	{
		PlayerController->bShowMouseCursor = true;
		PlayerController->bEnableClickEvents = true;
		PlayerController->bEnableMouseOverEvents = true;

		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (BirdEyeInputMapping)
				{
					Subsystem->AddMappingContext(BirdEyeInputMapping, 0);
				}
			}
		}
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATwinUiManager::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		ATwinUiManager* UiManager = Cast<ATwinUiManager>(FoundActors[0]);
		if (UiManager)
		{
			UiManager->OnResetRequested.AddDynamic(this, &ABirdEye::ResetCamera);
		}
	}



	// Set default camera arm length and enable input by default
	SetCameraArmLength(40000.f);
	SetInputEnabled(false);;
}

void ABirdEye::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsDragging)
	{
		ApplyPitchSpringBack(DeltaTime);
		HandleRotationMomentum(DeltaTime);
	}

	if (bIsPanningToTarget)
	{
		UpdateSmoothPan(DeltaTime);
	}

	HandleSmoothZoom(DeltaTime);

	// ✅ Use local temp vars so you don’t overwrite bTouch1Down/bTouch2Down mid-frame
	bool bCurrentTouch1Down = false;
	bool bCurrentTouch2Down = false;
	FVector2D CurrentTouch1 = FVector2D::ZeroVector;
	FVector2D CurrentTouch2 = FVector2D::ZeroVector;

	PlayerController->GetInputTouchState(ETouchIndex::Touch1, CurrentTouch1.X, CurrentTouch1.Y, bCurrentTouch1Down);
	PlayerController->GetInputTouchState(ETouchIndex::Touch2, CurrentTouch2.X, CurrentTouch2.Y, bCurrentTouch2Down);

	if (bCurrentTouch1Down && bCurrentTouch2Down)
	{
		float CurrentDistance = FVector2D::Distance(CurrentTouch1, CurrentTouch2);

		if (!FMath::IsNearlyZero(PreviousTouchDistance))
		{
			float Delta = CurrentDistance - PreviousTouchDistance;

			// 🔧 You can tweak this multiplier as needed
			float ZoomDelta = Delta * 0.1f;

			ApplyZoom(ZoomDelta);
		}

		PreviousTouchDistance = CurrentDistance;
	}
	else
	{
		// If one or both touches are released, reset the previous distance
		PreviousTouchDistance = 0.0f;
	}

	// ✅ Finally, update the touch state flags at the end of Tick
	bTouch1Down = bCurrentTouch1Down;
	bTouch2Down = bCurrentTouch2Down;
}


void ABirdEye::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(Input_ScreenPress, ETriggerEvent::Started, this, &ABirdEye::OnDragStart);
		Input->BindAction(Input_ScreenPress, ETriggerEvent::Completed, this, &ABirdEye::OnDragEnd);
		Input->BindAction(Input_ScreenMove, ETriggerEvent::Triggered, this, &ABirdEye::OnDragUpdate);
		Input->BindAction(Input_Zoom, ETriggerEvent::Triggered, this, &ABirdEye::HandleZoomInput);


		Input->BindAction(Input_ScreenPress, ETriggerEvent::Started, this, &ABirdEye::OnTouchPressed);
		Input->BindAction(Input_ScreenPress, ETriggerEvent::Completed, this, &ABirdEye::OnTouchReleased);

	}
}

// ======================
// 🎮 Input Handling
// ======================

void ABirdEye::OnDragStart()
{
	if (bIsDragging) return;

	bIsDragging = true;
	TotalDragDelta = FVector2D::ZeroVector;
	DragStartTime = GetWorld()->GetTimeSeconds();

	// Store the initial touch/mouse position
	LastTouchPosition = FVector2D::ZeroVector;
	SmoothedInputDelta = FVector2D::ZeroVector;

	// Read screen position on drag start
	FVector2D ScreenPos;
	bool bTouchDown = false;
	PlayerController->GetInputTouchState(ETouchIndex::Touch1, ScreenPos.X, ScreenPos.Y, bTouchDown);

	if (bTouchDown)
	{
		LastTouchPosition = ScreenPos;
	}
	else
	{
		PlayerController->GetMousePosition(LastTouchPosition.X, LastTouchPosition.Y);
	}
}


// void ABirdEye::OnDragUpdate(const FInputActionInstance& Instance)
// {
// 	if (!bIsDragging) return;

// 	FVector2D InputPosition = Instance.GetValue().Get<FVector2D>();
// 	FVector2D Delta;

// 	// #if PLATFORM_IOS || PLATFORM_ANDROID
// 		// Calculate raw delta
// 	Delta = InputPosition - LastTouchPosition;
// 	LastTouchPosition = InputPosition;

// 	// 🔽 Scale down touch delta (you can tweak this factor)
// 	Delta *= TouchDragSensitivity;
// 	// #else
// 	// 	// Assume InputPosition is already a delta (mouse)
// 	// 	Delta = InputPosition;
// 	// #endif

// 	RotateCamera(Delta);

// 	float DeltaTime = GetWorld()->GetDeltaSeconds();
// 	CurrentDragVelocity = FMath::Vector2DInterpTo(CurrentDragVelocity, Delta / DeltaTime, DeltaTime, VelocitySmoothing);
// 	TotalDragDelta += Delta;
// }


void ABirdEye::OnDragUpdate(const FInputActionInstance& Instance)
{
	if (!bIsDragging) return;

	FVector2D InputPosition = Instance.GetValue().Get<FVector2D>();

	if (LastTouchPosition.IsNearlyZero())
	{
		LastTouchPosition = InputPosition;
		return;
	}

	FVector2D RawDelta = InputPosition - LastTouchPosition;
	LastTouchPosition = InputPosition;

	// Clamp absurdly large deltas (caused by input skips)
	if (RawDelta.SizeSquared() > 50000.0f) return;

	FVector2D TargetDelta = RawDelta * TouchDragSensitivity;

	// Apply the smoothed delta
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	SmoothedInputDelta = FMath::Vector2DInterpTo(SmoothedInputDelta, TargetDelta, DeltaTime, 20.0f);
	RotateCamera(SmoothedInputDelta);

	// 🚨 Save raw (not smoothed) velocity for momentum
	CurrentDragVelocity = TargetDelta / DeltaTime;

	TotalDragDelta += TargetDelta;
}




void ABirdEye::OnDragEnd()
{
	bIsDragging = false;

	float DragDuration = GetWorld()->GetTimeSeconds() - DragStartTime;
	MomentumVelocity = CalculateMomentumVelocity(DragDuration, TotalDragDelta);

	CheckForClickTarget();
}

void ABirdEye::HandleZoomInput(const FInputActionInstance& Instance)
{
	float Value = Instance.GetValue().Get<float>();
	ApplyZoom(Value);
}

void ABirdEye::ApplyZoom(float Delta)
{
	PendingZoomInput += Delta * ZoomSpeed;
}

// ======================
// 📷 Camera Control
// ======================

void ABirdEye::RotateCamera(const FVector2D& DragDelta)
{
	if (!SpringArm) return;

	float DeltaTime = GetWorld()->GetDeltaSeconds();
	float YawInput = DragDelta.X * RotationSpeed * DeltaTime;
	float PitchInput = -DragDelta.Y * RotationSpeed * DeltaTime;

	FRotator CurrentRotation = SpringArm->GetRelativeRotation();

	float NewPitch = CurrentRotation.Pitch + PitchInput;
	float NewYaw = CurrentRotation.Yaw + YawInput;

	float MaxPitchWithOvershoot = MaxPitchAngle + OverPitchTolerance;
	float MinPitchWithOvershoot = MinPitchAngle - OverPitchTolerance;

	if (NewPitch > MaxPitchAngle)
	{
		float OvershootRatio = (NewPitch - MaxPitchAngle) / OverPitchTolerance;
		PitchInput *= 1.0f - FMath::Clamp(OvershootRatio, 0.0f, 1.0f);
	}
	else if (NewPitch < MinPitchAngle)
	{
		float OvershootRatio = (MinPitchAngle - NewPitch) / OverPitchTolerance;
		PitchInput *= 1.0f - FMath::Clamp(OvershootRatio, 0.0f, 1.0f);
	}

	NewPitch = FMath::Clamp(CurrentRotation.Pitch + PitchInput, MinPitchWithOvershoot, MaxPitchWithOvershoot);
	SpringArm->SetRelativeRotation(FRotator(NewPitch, NewYaw, 0.0f));
}

void ABirdEye::ApplyPitchSpringBack(float DeltaTime)
{
	if (!SpringArm) return;

	FRotator CurrentRotation = SpringArm->GetRelativeRotation();
	float ClampedPitch = FMath::Clamp(CurrentRotation.Pitch, MinPitchAngle, MaxPitchAngle);

	if (!FMath::IsNearlyEqual(CurrentRotation.Pitch, ClampedPitch, 0.1f))
	{
		float NewPitch = FMath::FInterpTo(CurrentRotation.Pitch, ClampedPitch, DeltaTime, SpringBackSpeed);
		SpringArm->SetRelativeRotation(FRotator(NewPitch, CurrentRotation.Yaw, 0.0f));
	}
}

void ABirdEye::HandleRotationMomentum(float DeltaTime)
{
	if (!MomentumVelocity.IsNearlyZero())
	{
		RotateCamera(MomentumVelocity * DeltaTime);
		MomentumVelocity = FMath::Vector2DInterpTo(MomentumVelocity, FVector2D::ZeroVector, DeltaTime, MomentumFriction);
	}
}

void ABirdEye::HandleSmoothZoom(float DeltaTime)
{
	if (!SpringArm) return;

	if (!FMath::IsNearlyZero(PendingZoomInput))
	{
		ZoomVelocity = FMath::FInterpTo(ZoomVelocity, PendingZoomInput, DeltaTime, ZoomFriction);
		PendingZoomInput = 0.0f;
	}
	else if (!FMath::IsNearlyZero(ZoomVelocity, 1.0f))
	{
		ZoomVelocity = FMath::FInterpTo(ZoomVelocity, 0.0f, DeltaTime, ZoomFriction);
	}

	if (!FMath::IsNearlyZero(ZoomVelocity, ZoomMomentumThreshold))
	{
		float NewLength = SpringArm->TargetArmLength - ZoomVelocity * DeltaTime;

		float SoftMin = MinZoomDistance - SoftZoomPadding;
		float SoftMax = MaxZoomDistance + SoftZoomPadding;

		if (NewLength < MinZoomDistance)
		{
			NewLength += (MinZoomDistance - NewLength) * 0.9f;
		}
		else if (NewLength > MaxZoomDistance)
		{
			NewLength -= (NewLength - MaxZoomDistance) * 0.9f;
		}

		SpringArm->TargetArmLength = FMath::Clamp(NewLength, SoftMin, SoftMax);
	}
}



FVector2D ABirdEye::CalculateMomentumVelocity(float DragDuration, const FVector2D& DragDelta) const
{
	float SafeDuration = FMath::Max(DragDuration, KINDA_SMALL_NUMBER);
	FVector2D DragDirection = DragDelta.GetSafeNormal();
	float Speed = DragDelta.Size() / SafeDuration;

	if (Speed > MomentumMinimumSpeed)
	{
		return DragDirection * FMath::Clamp(Speed, 0.f, MaxMomentumSpeed);
	}

	return FVector2D::ZeroVector;
}

// ======================
// 🧭 Targeting / Focus
// ======================

void ABirdEye::CheckForClickTarget()
{
	if (!PlayerController || bIsDragging || TotalDragDelta.Size() > ClickMaxDragDistance)
		return;

	FHitResult HitResult;
	bool bHitSuccess = false;

	bHitSuccess = PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

	if (!bHitSuccess)
	{
		FVector2D ScreenPosition;
		bool bTouchDown = false;

		PlayerController->GetInputTouchState(ETouchIndex::Touch1, ScreenPosition.X, ScreenPosition.Y, bTouchDown);

		if (bTouchDown || TotalDragDelta.IsNearlyZero())
		{
			bHitSuccess = PlayerController->GetHitResultAtScreenPosition(ScreenPosition, ECC_Visibility, false, HitResult);
		}
	}

	if (bHitSuccess)
	{
		if (ABulding* HitBuilding = Cast<ABulding>(HitResult.GetActor()))
		{
			FocusCameraOnBuilding(HitBuilding);
			OnBuildingSelected.Broadcast(HitBuilding);
		}
	}
}



void ABirdEye::FocusCameraOnBuilding(AActor* TargetActor)
{
	if (!TargetActor) return;

	FVector TargetLocation = TargetActor->GetActorLocation();
	MoveToLocationSmooth(TargetLocation, FocusZoomDistance);
}


void ABirdEye::MoveToLocationSmooth(FVector TargetLocation, float InTargetZoomLength)
{
	StartLocation = GetActorLocation();
	TargetPanLocation = TargetLocation;
	StartZoomLength = SpringArm->TargetArmLength;
	TargetZoomLengthStored = InTargetZoomLength;
	PanProgress = 0.0f;
	bIsPanningToTarget = true;
}

void ABirdEye::UpdateSmoothPan(float DeltaTime)
{
	PanProgress += DeltaTime * PanSpeed;
	float EasedProgress = FMath::InterpEaseInOut(0.0f, 1.0f, PanProgress, 2.0f);

	// Update location
	FVector NewLocation = FMath::Lerp(StartLocation, TargetPanLocation, EasedProgress);
	SetActorLocation(NewLocation);

	// Update zoom
	float NewZoom = FMath::Lerp(StartZoomLength, TargetZoomLengthStored, EasedProgress);
	SpringArm->TargetArmLength = FMath::Clamp(NewZoom, MinZoomDistance, MaxZoomDistance);

	if (PanProgress >= 1.0f)
	{
		bIsPanningToTarget = false;
	}
}


void ABirdEye::ResetCamera()
{
	MoveToLocationSmooth(DefaultLocation, DefaultZoom);
}





void ABirdEye::SetCameraArmLength(float Length)
{
	if (SpringArm)
	{
		SpringArm->TargetArmLength = Length;
	}
}

void ABirdEye::SetInputEnabled(bool bEnabled)
{
	if (bEnabled)
	{
		EnableInput(PlayerController);
	}
	else
	{
		DisableInput(PlayerController);
	}

	if (PlayerController)
	{
		PlayerController->bShowMouseCursor = bEnabled;
	}
}



void ABirdEye::OnTouchPressed()
{
	bool bDown1 = false, bDown2 = false;
	FVector2D P1, P2;

	PlayerController->GetInputTouchState(ETouchIndex::Touch1, P1.X, P1.Y, bDown1);
	PlayerController->GetInputTouchState(ETouchIndex::Touch2, P2.X, P2.Y, bDown2);

	bTouch1Down = bDown1;
	bTouch2Down = bDown2;
}

void ABirdEye::OnTouchReleased()
{
	bTouch1Down = false;
	bTouch2Down = false;
	PreviousTouchDistance = 0.0f;
}
