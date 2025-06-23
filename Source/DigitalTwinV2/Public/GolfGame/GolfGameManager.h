// Copyright notice (optional)
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GolfGameManager.generated.h"

// Enumeration to define different shot types
UENUM(BlueprintType)
enum class EShotType : uint8
{
	LongShot UMETA(DisplayName = "Long Shot"),
	ShortShot UMETA(DisplayName = "Chip Shot"), // Deprecated / unused
	ChipShot UMETA(DisplayName = "Chip Shot")
};

UCLASS()
class DIGITALTWINV2_API AGolfGameManager : public AActor
{
	GENERATED_BODY()

public:
	AGolfGameManager();

	// Singleton instance getter
	static AGolfGameManager* Get();

	// Lifecycle overrides
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// === Gameplay ===
	UFUNCTION(BlueprintCallable, Category = "Golf|Shot") void BeginAdjustShot();
	UFUNCTION(BlueprintCallable, Category = "Golf|Shot") void AdjustShot(const FVector2D& Delta);
	UFUNCTION(BlueprintCallable, Category = "Golf|Shot") void CancelShotAdjust();
	UFUNCTION(BlueprintCallable, Category = "Golf|Shot") void Shoot();
	UFUNCTION(BlueprintCallable, Category = "Golf|Shot") void SetShotTypeToLong();
	UFUNCTION(BlueprintCallable, Category = "Golf|Shot") void SetShotTypeToChip();
	UFUNCTION(BlueprintCallable, Category = "Golf|Sequence") void StartGameSequence();
	UFUNCTION(BlueprintCallable, Category = "Golf") void SpawnBallAtCurrentPosition();

	// === Trajectory ===
	UFUNCTION(BlueprintCallable, Category = "Trajectory") TArray<FVector> CalculateArcPoints(const FVector& Start, const FVector& Velocity, int Steps, float TimeStep);
	UFUNCTION(BlueprintCallable, Category = "Trajectory") void UpdateTrajectorySpline(const TArray<FVector>& Points);
	UFUNCTION(BlueprintCallable, Category = "Trajectory") void AfterShotSequence();
	UFUNCTION(BlueprintCallable, Category = "Trajectory") FVector ComputeLaunchVelocity() const;
	UFUNCTION(BlueprintCallable, Category = "Trajectory") void HandleBallOutOfBounds();
	UFUNCTION(BlueprintCallable, Category = "Trajectory") void EndGame();
	UFUNCTION() void BeginCameraTransitionToBall();
	UFUNCTION() void UpdateCameraLerp();
	UFUNCTION() void BeginCameraTransitionToBallFollow();
	UFUNCTION() void UpdateCameraLerpToFollow();


	// === Public Game Configuration ===
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Golf") int32 CurrentHoleIndex = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Golf") int32 TotalHoles = 9;

	// Actor classes used to spawn objects
	UPROPERTY(EditDefaultsOnly, Category = "Golf") TSubclassOf<AActor> BallActorClass;

	// Shot configuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Golf|Shot") EShotType CurrentShotType = EShotType::LongShot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Golf|Shot") float LongShotPower = 2000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Golf|Shot") float ChipShotPower = 600.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Golf|Shot") float SwipeMultiplier = 10.0f;

	// Scene actors
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Golf") AActor* CurrentStartActor = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Golf") AActor* BoundsActor = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Golf") TArray<AActor*> AllTargets;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Golf") AActor* SpawnedBall = nullptr;

	// Game state
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Golf|Sequence") bool bHasStarted = false;

	// Trajectory visual components
	UPROPERTY(VisibleAnywhere, Category = "Trajectory") class USplineComponent* TrajectorySpline;
	UPROPERTY() TArray<class USplineMeshComponent*> SplineMeshes;
	UPROPERTY() TArray<UStaticMeshComponent*> TrajectorySpheres;

	// Debug/visual meshes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual") UStaticMesh* SphereMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual") UMaterialInterface* SphereMaterial;
	UPROPERTY() UStaticMesh* LineMesh;
	UPROPERTY() UMaterialInterface* LineMaterial;

	// UI elements
	UPROPERTY(EditDefaultsOnly, Category = "UI") TSubclassOf<UUserWidget> GolfGameUIClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI") TSubclassOf<class UGolfStartMenu> GolfStartMenuClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI") TSubclassOf<class UGolfEndGameMenu> GolfEndMenuClass;
	UPROPERTY() UUserWidget* GolfGameUIInstance;



	// Camera configuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Golf|Camera") AActor* FollowCameraActor = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Golf|Camera") FVector CameraOffset = FVector(-400, 0, 200);







private:
	// Singleton instance
	static AGolfGameManager* Instance;

	// Input tracking for drag direction
	FVector2D AccumulatedInput = FVector2D::ZeroVector;
	FVector2D CurrentInputDirection = FVector2D::ZeroVector;

	// Rotation and shot control
	float RotationAngle = 0.0f;
	float MinRotation = -60.0f;
	float MaxRotation = 60.0f;
	float CurrentPower = 0.0f;

	// Input state flags
	bool bIsAdjustingShot = false;
	bool bHasAdjusted = false;
	bool bIsWaitingForBallToStop = false;
	float MinInputDistance = 1.0f;

	// Ball movement state
	float StopVelocityThreshold = 50.0f;
	float TimeSinceShot = 0.0f;
	float WaitBeforeCheckingStop = 0.5f;
	FTimerHandle AfterShotDelayHandle;

	// Original rotation used to reset after canceling
	FRotator OriginalStartRotation;


	// Camera transition state
	FVector CameraStartLocation;
	FRotator CameraStartRotation;
	float CameraLerpAlpha = 0.0f;
	float CameraLerpDuration = 2.5f;
	float CameraLerpFollowDuration = 0.5f;
	FTimerHandle CameraLerpTimer;
	FVector CameraDynamicOffset;




public:

	// Camera setup for menu intro
	UPROPERTY(EditAnywhere, Category = "Golf|Camera")
	FTransform MenuCameraTransform;

	bool bHasSetMenuCamera = false;
	// === Add to private section ===
	bool bShouldFollowBall = false;
	FVector BallFollowOffset;



	UPROPERTY(EditAnywhere, Category = "VFX")
	class UNiagaraSystem* TrailSystemTemplate;

	// Reference to the MetaHuman Blueprint in the level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	ACharacter* ThirdCharacter;

	// Handles the AnimNotify from the montage to spawn the ball
	UFUNCTION()
	void HandleAnimNotify_SpawnBall(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);


	// Call this on mouse release to play ResumeMontage (if not ChipShot) and then shoot
	UFUNCTION(BlueprintCallable, Category = "Golf|Shot")
	void OnMouseReleaseAndResumeMontage();
};
