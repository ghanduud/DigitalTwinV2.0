// Fill out your copyright notice in the Description page of Project Settings.


#include "Buldings/Bulding.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Json.h"
#include "JsonUtilities.h"

TArray<FBuildingData> ABulding::AllBuildingData;

void ABulding::LoadBuildingDataFromJson()
{
    if (AllBuildingData.Num() > 0) return; // Only load once
    FString JsonFilePath = FPaths::ProjectContentDir() / TEXT("TwinApp/Data/BuildingsData.json");
    FString JsonString;
    if (!FFileHelper::LoadFileToString(JsonString, *JsonFilePath)) return;
    TArray<TSharedPtr<FJsonValue>> JsonArray;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    if (FJsonSerializer::Deserialize(Reader, JsonArray))
    {
        for (auto& Value : JsonArray)
        {
            FBuildingData Data;
            if (FJsonObjectConverter::JsonObjectToUStruct(Value->AsObject().ToSharedRef(), &Data, 0, 0))
            {
                AllBuildingData.Add(Data);
            }
        }
    }
}

void ABulding::ApplyBuildingData(const FBuildingData& Data)
{
    DisplayName = Data.DisplayName;
    ImagePath = Data.ImagePath;
    FootPrintArea = Data.FootPrintArea;
    BuldingUnitArea = Data.BuldingUnitArea;
    GroundFloorArea = Data.GroundFloorArea;
    FirstFloorArea = Data.FirstFloorArea;
    RoofFloorArea = Data.RoofFloorArea;
    Price = Data.Price;
    Discount = Data.Discount;
    NumberOfFloors = Data.NumberOfFloors;
    // Status conversion
    if (Data.Status == "Available") Status = EBuildingStatus::Available;
    else if (Data.Status == "Reserved") Status = EBuildingStatus::Reserved;
    else if (Data.Status == "Sold") Status = EBuildingStatus::Sold;
    else if (Data.Status == "UnderConstruction") Status = EBuildingStatus::UnderConstruction;
    // BuildingType conversion if needed
}

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
	AActor::BeginPlay();
	LoadBuildingDataFromJson();
	FString ThisType;
	switch (BuildingType)
	{
		case EBuildingType::OneStoryVilla: ThisType = "OneStoryVilla"; break;
		case EBuildingType::Palace: ThisType = "Palace"; break;
		case EBuildingType::StandAloneVilla: ThisType = "StandaloneVilla"; break;
		default: break;
	}
	TArray<FBuildingData> Filtered;
	for (const FBuildingData& Data : AllBuildingData)
	{
		if (Data.BuildingType == ThisType)
			Filtered.Add(Data);
	}
	if (Filtered.IsValidIndex(DataIndex))
	{
		ApplyBuildingData(Filtered[DataIndex]);
	}
}

// Called every frame
void ABulding::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);
}

