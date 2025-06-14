#include "GameMode/GolfGameGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "GolfGame/HoleData.h"
#include "Components/SplineComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "GolfGame/UI/GolfGameUI.h"
#include "Blueprint/UserWidget.h"



AGolfGameGameMode::AGolfGameGameMode()
{
    // Load sphere mesh from Engine Content
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshFinder(TEXT("/Engine/BasicShapes/Sphere"));
    if (SphereMeshFinder.Succeeded())
    {
        DebugSphereMesh = SphereMeshFinder.Object;
    }


    static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatFinder(TEXT("/Engine/EngineDebugMaterials/VertexColorViewMode_RedOnly"));
    if (MatFinder.Succeeded())
    {
        DebugSphereMaterial = MatFinder.Object;
    }




}

void AGolfGameGameMode::BeginPlay()
{


    Super::BeginPlay();



    InitializeHoles();
    SpawnBallAtCurrentPosition();
    StartGameSequence(); // start the intro
}


void AGolfGameGameMode::InitializeHoles()
{
    Holes.Empty();

    UWorld* World = GetWorld();
    if (!World) return;

    TArray<AActor*> StartActors;
    TArray<AActor*> TargetActors;
    TArray<AActor*> BoundsActors;

    UGameplayStatics::GetAllActorsOfClass(World, StartActorClass, StartActors);
    UGameplayStatics::GetAllActorsOfClass(World, TargetActorClass, TargetActors);
    UGameplayStatics::GetAllActorsOfClass(World, BoundsActorClass, BoundsActors);

    for (int32 i = 1; i <= TotalHoles; ++i)
    {
        FName StartTag = FName(*FString::Printf(TEXT("Hole%d_Start"), i));
        FName TargetTag = FName(*FString::Printf(TEXT("Hole%d_Target"), i));
        FName BoundsTag = FName(*FString::Printf(TEXT("Hole%d_Bounds"), i));

        AActor* Start = nullptr;
        AActor* Target = nullptr;
        AActor* Bounds = nullptr;

        for (AActor* Actor : StartActors)
        {
            if (Actor && Actor->Tags.Contains(StartTag))
            {
                Start = Actor;
                break;
            }
        }

        for (AActor* Actor : TargetActors)
        {
            if (Actor && Actor->Tags.Contains(TargetTag))
            {
                Target = Actor;
                break;
            }
        }

        for (AActor* Actor : BoundsActors)
        {
            if (Actor && Actor->Tags.Contains(BoundsTag))
            {
                Bounds = Actor;
                break;
            }
        }

        if (Start && Target && Bounds)
        {
            FHoleData Hole(i, Start, Target, Bounds);
            Holes.Add(Hole);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Could not find one or more tagged actors for Hole %d"), i);
        }
    }


    if (Holes.Num() > 0)
    {
        CurrentStartActor = Holes[0].StartPoint;
        CurrentTargetActor = Holes[0].HoleTarget;
        CurrentHoleIndex = 0;
    }
}




void AGolfGameGameMode::BeginAdjustShot()
{
    if (!bHasStarted)
        return;

    bIsAdjustingShot = true;
    CurrentInputDirection = FVector2D::ZeroVector;
}

void AGolfGameGameMode::AdjustShot(const FVector2D& Delta)
{
    if (!bHasStarted || !bIsAdjustingShot) return;

    // Accumulate swipe input
    CurrentInputDirection += Delta;

    // Compute force based on swipe length
    float ForceStrength = CurrentInputDirection.Size() * SwipeMultiplier;

    // Use forward direction of the start actor (no camera/player)
    FVector Direction = CurrentStartActor->GetActorForwardVector().GetSafeNormal();

    // Add slight upward lift to the shot
    FVector LaunchVelocity = Direction * ForceStrength + FVector(0, 0, 1000);

    // Start from ball's real position
    FVector StartLocation = SpawnedBall ? SpawnedBall->GetActorLocation() : CurrentStartActor->GetActorLocation();

    // Visualize trajectory
    TArray<FVector> Arc = CalculateArcPoints(StartLocation, LaunchVelocity, 30, 0.1f);
    UpdateTrajectorySpline(Arc);
}



void AGolfGameGameMode::Shoot()
{
    if (!bHasStarted || !bIsAdjustingShot) return;
    bIsAdjustingShot = false;

    // Same force and direction as AdjustShot
    float ForceStrength = CurrentInputDirection.Size() * SwipeMultiplier;
    FVector Direction = CurrentStartActor->GetActorForwardVector().GetSafeNormal();
    FVector LaunchVelocity = Direction * ForceStrength + FVector(0, 0, 1000);

    if (SpawnedBall && SpawnedBall->IsRootComponentMovable())
    {
        UPrimitiveComponent* BallRoot = Cast<UPrimitiveComponent>(SpawnedBall->GetRootComponent());
        if (BallRoot)
        {
            BallRoot->AddImpulse(LaunchVelocity, NAME_None, true);
        }
    }

    CurrentInputDirection = FVector2D::ZeroVector;
    UpdateTrajectorySpline({}); // Hide trajectory
}




FHoleData AGolfGameGameMode::GetCurrentHoleData() const
{
    if (Holes.IsValidIndex(CurrentHoleIndex))
    {
        return Holes[CurrentHoleIndex];
    }

    return FHoleData();
}



void AGolfGameGameMode::SetShotTypeToLong()
{
    CurrentShotType = EShotType::LongShot;
}

void AGolfGameGameMode::SetShotTypeToShort()
{
    CurrentShotType = EShotType::ShortShot;
}

void AGolfGameGameMode::SetShotTypeToChip()
{
    CurrentShotType = EShotType::ChipShot;
}



void AGolfGameGameMode::StartGameSequence()
{
    bHasStarted = false;

    UE_LOG(LogTemp, Log, TEXT("Starting Game Sequence..."));

    // Simulated end of sequence
    bHasStarted = true;

    UE_LOG(LogTemp, Log, TEXT("Game Sequence Completed. Ready to play."));

    // 👉 Show UI after game starts
    if (GolfGameUIClass)
    {
        GolfGameUIInstance = CreateWidget<UUserWidget>(GetWorld(), GolfGameUIClass);
        if (GolfGameUIInstance)
        {
            GolfGameUIInstance->AddToViewport();
            UE_LOG(LogTemp, Log, TEXT("Golf Game UI displayed."));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create GolfGameUIInstance."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("GolfGameUIClass not assigned in GameMode."));
    }
}




void AGolfGameGameMode::SpawnBallAtCurrentPosition()
{
    if (!BallActorClass || !CurrentStartActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot spawn ball: Missing BallActorClass or CurrentStartActor."));
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    if (SpawnedBall)
    {
        SpawnedBall->Destroy();
    }

    FVector SpawnLocation = CurrentStartActor->GetActorLocation();
    FRotator SpawnRotation = CurrentStartActor->GetActorRotation();

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;

    SpawnedBall = World->SpawnActor<AActor>(BallActorClass, SpawnLocation, SpawnRotation, SpawnParams);

    if (SpawnedBall)
    {
        UE_LOG(LogTemp, Log, TEXT("Spawned ball at Hole %d start position."), CurrentHoleIndex + 1);

        // 🔧 Create spline component
        USplineComponent* BallSpline = NewObject<USplineComponent>(SpawnedBall, TEXT("TrajectorySpline"));
        if (BallSpline)
        {
            BallSpline->RegisterComponent(); // Register with the world
            BallSpline->AttachToComponent(SpawnedBall->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
            BallSpline->SetMobility(EComponentMobility::Movable);

            TrajectorySpline = BallSpline;
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn ball."));
    }
}





TArray<FVector> AGolfGameGameMode::CalculateArcPoints(const FVector& Start, const FVector& Velocity, int Steps, float TimeStep)
{
    TArray<FVector> Points;

    FVector Gravity = FVector(0, 0, GetWorld()->GetGravityZ());
    FVector Position = Start;
    FVector CurrentVelocity = Velocity;

    for (int i = 0; i < Steps; i++)
    {
        Points.Add(Position);
        Position += CurrentVelocity * TimeStep;
        CurrentVelocity += Gravity * TimeStep;
    }

    return Points;
}





void AGolfGameGameMode::UpdateTrajectorySpline(const TArray<FVector>& Points)
{
    if (!TrajectorySpline) return;

    // Clear old spline points
    TrajectorySpline->ClearSplinePoints();

    // Destroy previous debug spheres
    for (UStaticMeshComponent* Sphere : TrajectoryDebugSpheres)
    {
        if (Sphere) Sphere->DestroyComponent();
    }
    TrajectoryDebugSpheres.Empty();

    // Add new spline points and debug spheres
    for (int32 i = 0; i < Points.Num(); ++i)
    {
        const FVector& Point = Points[i];

        FSplinePoint SplinePoint(i, Point);
        TrajectorySpline->AddPoint(SplinePoint, false);

        if (DebugSphereMesh && SpawnedBall)
        {
            UStaticMeshComponent* Sphere = NewObject<UStaticMeshComponent>(SpawnedBall);
            Sphere->SetStaticMesh(DebugSphereMesh);
            Sphere->SetWorldLocation(Point);
            Sphere->SetWorldScale3D(FVector(0.1f)); // Small sphere
            Sphere->RegisterComponent();
            Sphere->SetMobility(EComponentMobility::Movable);
            Sphere->AttachToComponent(SpawnedBall->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);

            if (DebugSphereMaterial)
            {
                Sphere->SetMaterial(0, DebugSphereMaterial);
            }

            TrajectoryDebugSpheres.Add(Sphere);
        }
    }

    TrajectorySpline->UpdateSpline();
}

