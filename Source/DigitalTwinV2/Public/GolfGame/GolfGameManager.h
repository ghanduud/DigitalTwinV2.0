// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GolfGameManager.generated.h"



UENUM(BlueprintType)
enum class EShotType : uint8
{
	LongShot UMETA(DisplayName = "Long Shot"),
	ShortShot UMETA(DisplayName = "Chip Shot"),
	ChipShot UMETA(DisplayName = "Chip Shot")
};

UCLASS()
class DIGITALTWINV2_API AGolfGameManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGolfGameManager();


	// Static access to the manager
	static AGolfGameManager* Get();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	// Singleton-like instance
	static AGolfGameManager* Instance;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;



	// For tracking current hole
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Golf")
	int32 CurrentHoleIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Golf")
	int32 TotalHoles = 9;


	UPROPERTY(EditDefaultsOnly, Category = "Golf")
	TSubclassOf<AActor> StartActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Golf")
	TSubclassOf<AActor> TargetActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Golf")
	TSubclassOf<AActor> BoundsActorClass;


	UFUNCTION(BlueprintCallable, Category = "Golf|Shot")
	void BeginAdjustShot();

	UFUNCTION(BlueprintCallable, Category = "Golf|Shot")
	void AdjustShot(const FVector2D& Delta);

	UFUNCTION(BlueprintCallable, Category = "Golf|Shot")
	void Shoot();



private:
	bool bIsAdjustingShot = false;
	FVector2D CurrentInputDirection = FVector2D::ZeroVector;




public:
	// Currently active ball location (will be set to Start at beginning)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Golf")
	AActor* CurrentStartActor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Golf")
	AActor* BoundsActor = nullptr;



	// Current type of shot being used
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Golf|Shot")
	EShotType CurrentShotType = EShotType::LongShot;


	// Flag to indicate if the starting sequence has ended
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Golf|Sequence")
	bool bHasStarted = false;

	UFUNCTION(BlueprintCallable, Category = "Golf|Shot")
	void SetShotTypeToLong();


	UFUNCTION(BlueprintCallable, Category = "Golf|Shot")
	void SetShotTypeToChip();


	// Called at the beginning of the game to handle intro sequence
	UFUNCTION(BlueprintCallable, Category = "Golf|Sequence")
	void StartGameSequence();







	// Blueprint class of the ball to spawn
	UPROPERTY(EditDefaultsOnly, Category = "Golf")
	TSubclassOf<AActor> BallActorClass;

	// The currently spawned ball
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Golf")
	AActor* SpawnedBall = nullptr;

	// Spawns the ball at the current start position
	UFUNCTION(BlueprintCallable, Category = "Golf")
	void SpawnBallAtCurrentPosition();

	UFUNCTION(BlueprintCallable, Category = "Golf")
	void CancelShotAdjust();





public:

	UPROPERTY(VisibleAnywhere, Category = "Trajectory")
	class USplineComponent* TrajectorySpline;

	// Controls how powerful the swipe is
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Golf|Shot")
	float SwipeMultiplier = 10.0f;  // Tune this value as needed

	UFUNCTION(BlueprintCallable, Category = "Trajectory")
	TArray<FVector> CalculateArcPoints(const FVector& Start, const FVector& Velocity, int Steps, float TimeStep);

	UFUNCTION(BlueprintCallable, Category = "Trajectory")
	void UpdateTrajectorySpline(const TArray<FVector>& Points);

	UFUNCTION(BlueprintCallable, Category = "Trajectory")
	void AfterShotSequence();

	UFUNCTION(BlueprintCallable, Category = "Trajectory")
	FVector ComputeLaunchVelocity() const;


	UFUNCTION(BlueprintCallable, Category = "Trajectory")
	void HandleBallOutOfBounds();

	UFUNCTION(BlueprintCallable, Category = "Trajectory")
	void EndGame();


	UPROPERTY()
	UStaticMesh* LineMesh;

	UPROPERTY()
	UMaterialInterface* LineMaterial;

	TArray<class USplineMeshComponent*> SplineMeshes; // Store to destroy/clear later



	// For spawning debug spheres
	UPROPERTY()
	TArray<UStaticMeshComponent*> TrajectorySpheres;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	UStaticMesh* SphereMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	UMaterialInterface* SphereMaterial;

	// UI Widget class to spawn
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> GolfGameUIClass;

	// Reference to the spawned UI
	UPROPERTY()
	UUserWidget* GolfGameUIInstance;




	float CurrentPower = 0.0f;
	float MinPower = 100.0f;
	float MaxPower = 3000.0f;

	float RotationAngle = 0.0f;
	float MinRotation = -60.0f;
	float MaxRotation = 60.0f;

	FRotator OriginalStartRotation;

	FVector2D AccumulatedInput = FVector2D::ZeroVector;
	bool bHasAdjusted = false;
	float MinInputDistance = 1.0f;


	bool bIsWaitingForBallToStop = false;
	float StopVelocityThreshold = 50.0f;

	float TimeSinceShot = 0.0f;
	float WaitBeforeCheckingStop = 0.5f; // Half a second


	FTimerHandle AfterShotDelayHandle;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Golf|Shot")
	float LongShotPower = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Golf|Shot")
	float ShortShotPower = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Golf|Shot")
	float ChipShotPower = 600.0f;


	// Reference to the camera actor that should follow the ball
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Golf|Camera")
	AActor* FollowCameraActor = nullptr;

	// Offset from the ball (relative to ball's forward vector)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Golf|Camera")
	FVector CameraOffset = FVector(-400, 0, 200);


	UPROPERTY()
	TArray<AActor*> AllTargets;

};



