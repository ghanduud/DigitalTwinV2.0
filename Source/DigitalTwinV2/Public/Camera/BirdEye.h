// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BirdEye.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildingSelected, ABulding*, SelectedBuilding);

UCLASS()
class DIGITALTWINV2_API ABirdEye : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABirdEye();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	// Input mapping
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* BirdEyeInputMapping;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* Input_ScreenMove;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* Input_ScreenPress;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* Input_Zoom;

public:
	// Camera components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* Camera;






private:
	// Drag state
	bool bIsDragging = false;
	FVector2D CurrentDragVelocity;
	FVector2D MomentumVelocity;
	FVector2D TotalDragDelta = FVector2D::ZeroVector;
	float DragStartTime;

	// Panning state
	FVector StartLocation;
	FVector TargetPanLocation;
	float PanProgress = 0.0f;
	float ZoomProgress = 0.0f;
	bool bIsPanningToTarget = false;
	float StartZoomLength;
	float TargetZoomLengthStored;

	// Zoom state
	float PendingZoomInput = 0.0f;
	float ZoomVelocity = 0.0f;

private:
    FVector2D Touch1Location;
    FVector2D Touch2Location;
    bool bTouch1Down = false;
    bool bTouch2Down = false;
    float PreviousTouchDistance = 0.0f;





public:

	//Event
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBuildingSelected OnBuildingSelected;

	// Camera movement parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Movement")
	float MomentumFriction = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Movement")
	float VelocitySmoothing = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Movement")
	float MomentumMinimumSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Movement")
	float RotationSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Movement")
	float MaxMomentumSpeed = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Movement")
	float MinPitchAngle = -80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Movement")
	float MaxPitchAngle = -30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Movement")
	float OverPitchTolerance = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Movement")
	float SpringBackSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Movement")
	float PanSpeed = 2.0f;

	// Zoom parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	float MinZoomDistance = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	float MaxZoomDistance = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	float SoftZoomPadding = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	float ZoomSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	float ZoomFriction = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	float ZoomMomentumThreshold = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	float FocusZoomDistance = 6000.0f;

	// Click detection
	UPROPERTY(EditAnywhere, Category = "Click Detection")
	float ClickMaxDragDistance = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Movement")
	FVector DefaultLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Movement")
	float DefaultZoom = 20000.0f;

	// Player controller
	UPROPERTY()
	class APlayerController* PlayerController;

public:
	UFUNCTION(BlueprintCallable, Category = "Camera Movement")
	void RotateCamera(const FVector2D& DragDelta);

	UFUNCTION(BlueprintCallable, Category = "Camera Movement")
	void ApplyPitchSpringBack(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Camera Movement")
	void OnDragUpdate(const FInputActionInstance& Instance);

	UFUNCTION(BlueprintCallable, Category = "Camera Movement")
	void OnDragEnd();

	UFUNCTION(BlueprintCallable, Category = "Camera Movement")
	void OnDragStart();

	UFUNCTION(BlueprintCallable, Category = "Camera Movement")
	void CheckForClickTarget();

	UFUNCTION(BlueprintCallable, Category = "Camera Movement")
	void FocusCameraOnBuilding(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Camera Movement")
	void MoveToLocationSmooth(FVector TargetLocation, float InTargetZoomLength);

	UFUNCTION(BlueprintCallable, Category = "Camera Movement")
	FVector2D CalculateMomentumVelocity(float DragDuration, const FVector2D& DragDelta) const;

	UFUNCTION(BlueprintCallable, Category = "Camera Movement")
	void HandleRotationMomentum(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Camera Movement")
	void UpdateSmoothPan(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Camera Movement")
	void HandleSmoothZoom(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Camera Movement")
	void HandleZoomInput(const FInputActionInstance& Instance);

	UFUNCTION(BlueprintCallable, Category = "Camera Movement")
	void ResetCamera();

	UFUNCTION(BlueprintCallable, Category = "Camera Movement")
	void ApplyZoom(float Delta);





public:
	UFUNCTION()
	void SetCameraArmLength(float Length);

	UFUNCTION()
	void SetInputEnabled(bool bEnabled);
};
