// Fill out your copyright notice in the Description page of Project Settings.


#include "GolfBox.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AGolfBox::AGolfBox()
{
  PrimaryActorTick.bCanEverTick = false;

    // Create BoxCollider
    BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
    RootComponent = BoxCollider;

    // Adjust BoxCollider settings to ensure it can detect clicks
    BoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BoxCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
    BoxCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    BoxCollider->InitBoxExtent(FVector(200.f)); // Increase size for testing
    BoxCollider->bHiddenInGame = false;

    // Add a static mesh component with default material
    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMesh->SetupAttachment(RootComponent);

    // Load a default cube mesh (Engine Content)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded())
    {
        StaticMesh->SetStaticMesh(CubeMesh.Object);
    }

    // Load a default material (Engine Content)
    static ConstructorHelpers::FObjectFinder<UMaterial> DefaultMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    if (DefaultMaterial.Succeeded())
    {
        StaticMesh->SetMaterial(0, DefaultMaterial.Object);
    }

}

// Called when the game starts or when spawned
void AGolfBox::BeginPlay()
{
	   Super::BeginPlay();

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        PC->bEnableClickEvents = true;
        PC->bShowMouseCursor = true;
    }

    // Bind OnClicked event
    if (BoxCollider)
    {
        BoxCollider->OnClicked.AddDynamic(this, &AGolfBox::OnBoxClicked);
        UE_LOG(LogTemp, Log, TEXT("OnClicked bound to BoxCollider"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("BoxCollider is null in BeginPlay"));
    }
}

// Called every frame
void AGolfBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AGolfBox::OnBoxClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
    UE_LOG(LogTemp, Warning, TEXT("Box Clicked! Touched: %s | Button: %s"),
        *TouchedComponent->GetName(), *ButtonPressed.ToString());

    // Attempt to load the level
    FName LevelName = FName("GolfScene");
    UGameplayStatics::OpenLevel(this, LevelName);
    UE_LOG(LogTemp, Warning, TEXT("Attempted to load level: %s"), *LevelName.ToString());
}