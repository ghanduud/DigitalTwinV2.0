// Fill out your copyright notice in the Description page of Project Settings.


#include "Buldings/Bulding.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"

// Sets default values
ABulding::ABulding()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneRoot;

	// Create Static Mesh Component and attach to root
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	// Create Box Collider and attach to root or mesh (your choice)
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	BoxCollider->SetupAttachment(RootComponent); // or Mesh, if you prefer
	BoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxCollider->SetCollisionObjectType(ECC_WorldStatic);
	BoxCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

}


// Called when the game starts or when spawned
void ABulding::BeginPlay()
{
	Super::BeginPlay();
	switch (Id)
	{
	case 1: // One Story Villa
		BuildingType = EBuildingType::OneStoryVilla;
		FootPrintArea = 228.f;
		BuldingUnitArea = 228.f;
		GroundFloorArea = 228.f;
		FirstFloorArea = 0.f;
		RoofFloorArea = 0.f;
		Status = EBuildingStatus::Available;
		Discount = 0.f;
		Price = 1000000.f;
		break;
	case 2: // Palace
		BuildingType = EBuildingType::Palace;
		FootPrintArea = 600.f;
		BuldingUnitArea = 1547.f;
		GroundFloorArea = 498.f;
		FirstFloorArea = 529.f;
		RoofFloorArea = 204.f;
		Status = EBuildingStatus::Sold;
		Discount = 0.f;
		Price = 5000000.f;
		break;
	case 3: // Stand Alone Villa
		BuildingType = EBuildingType::StandAloneVilla;
		FootPrintArea = 128.f;
		BuldingUnitArea = 255.f;
		GroundFloorArea = 93.f;
		FirstFloorArea = 121.f;
		RoofFloorArea = 41.f;
		Status = EBuildingStatus::Available;
		Discount = 10.f;
		Price = 1500000.f;
		break;
	default:
		break;
	}
}

// Called every frame
void ABulding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

