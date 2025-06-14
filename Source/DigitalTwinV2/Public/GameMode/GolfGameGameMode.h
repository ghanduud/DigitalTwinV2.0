// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GolfGameGameMode.generated.h"



UENUM(BlueprintType)
enum class EShotType : uint8
{
	LongShot UMETA(DisplayName = "Long Shot"),
	ShortShot UMETA(DisplayName = "Chip Shot"),
	ChipShot UMETA(DisplayName = "Chip Shot")
};

/**
 *
 */
UCLASS()
class DIGITALTWINV2_API AGolfGameGameMode : public AGameModeBase
{
	GENERATED_BODY()


public:
	AGolfGameGameMode();


public:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Golf")
	TArray<struct FHoleData> Holes;

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

	UFUNCTION(BlueprintCallable, Category = "Golf|Setup")
	void InitializeHoles();

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
	AActor* CurrentTargetActor = nullptr;


	// Current type of shot being used
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Golf|Shot")
	EShotType CurrentShotType = EShotType::LongShot;


	// Flag to indicate if the starting sequence has ended
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Golf|Sequence")
	bool bHasStarted = false;

	UFUNCTION(BlueprintCallable, Category = "Golf|Shot")
	void SetShotTypeToLong();

	UFUNCTION(BlueprintCallable, Category = "Golf|Shot")
	void SetShotTypeToShort();

	UFUNCTION(BlueprintCallable, Category = "Golf|Shot")
	void SetShotTypeToChip();


	// Called at the beginning of the game to handle intro sequence
	UFUNCTION(BlueprintCallable, Category = "Golf|Sequence")
	void StartGameSequence();




	// Accessor for current hole data
	UFUNCTION(BlueprintCallable, Category = "Golf")
	FHoleData GetCurrentHoleData() const;




	// Blueprint class of the ball to spawn
	UPROPERTY(EditDefaultsOnly, Category = "Golf")
	TSubclassOf<AActor> BallActorClass;

	// The currently spawned ball
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Golf")
	AActor* SpawnedBall = nullptr;

	// Spawns the ball at the current start position
	UFUNCTION(BlueprintCallable, Category = "Golf")
	void SpawnBallAtCurrentPosition();





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


	UPROPERTY()
	UStaticMesh* LineMesh;

	UPROPERTY()
	UMaterialInterface* LineMaterial;

	TArray<class USplineMeshComponent*> SplineMeshes; // Store to destroy/clear later



	// For spawning debug spheres
	UPROPERTY()
	TArray<UStaticMeshComponent*> TrajectoryDebugSpheres;

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* DebugSphereMesh;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* DebugSphereMaterial;

	// UI Widget class to spawn
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> GolfGameUIClass;

	// Reference to the spawned UI
	UPROPERTY()
	UUserWidget* GolfGameUIInstance;


};
