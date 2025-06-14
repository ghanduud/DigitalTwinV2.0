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
    // Set bIsAvailable for filter logic
    bIsAvailable = (Status == EBuildingStatus::Available);
    // Set bHasDiscount for discount filter logic
    bHasDiscount = (Discount > 0.0f);
    UE_LOG(LogTemp, Warning, TEXT("[ApplyBuildingData] %s: Status=%s, bIsAvailable=%s, Discount=%.2f, bHasDiscount=%s"), *DisplayName, *Data.Status, bIsAvailable ? TEXT("true") : TEXT("false"), Discount, bHasDiscount ? TEXT("true") : TEXT("false"));
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

    HighlightBox = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HighlightBox"));
    HighlightBox->SetupAttachment(RootComponent);
    HighlightBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    HighlightBox->SetVisibility(false);
    HighlightBox->SetHiddenInGame(true);
    // Set default mesh (Cube) from engine
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded())
    {
        HighlightBox->SetStaticMesh(CubeMesh.Object);
    }
    // Assign highlight material (replace with your actual path if needed)
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> HighlightMat(TEXT("/Game/TwinApp/Materials/M_HighlightBox.M_HighlightBox"));
    if (HighlightMat.Succeeded())
    {
        HighlightBox->SetMaterial(0, HighlightMat.Object);
    }
}


// Called when the game starts or when spawned
void ABulding::BeginPlay()
{
	Super::BeginPlay();
	LoadBuildingDataFromJson();

    FString ThisType;
     switch (BuildingType)
    {
        case EBuildingType::OneStoryVilla: ThisType = "OneStoryVilla"; break;
        case EBuildingType::Palace: ThisType = "Palace"; break;
        case EBuildingType::StandAloneVilla: ThisType = "StandaloneVilla"; break;
        case EBuildingType::GolfClub: ThisType = "GolfClub"; break; // Ensure GolfClub is included
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

    FBox Bounds = Mesh->Bounds.GetBox();
    FVector BoxExtent = Bounds.GetExtent();
    FVector Origin = Bounds.GetCenter();
    HighlightBox->SetWorldLocation(Origin);
    HighlightBox->SetWorldScale3D(BoxExtent / 50.0f * 1.1f); // 10% bigger than building
}

void ABulding::ShowHighlightBox(FLinearColor Color)
{
    if (!HighlightBox || !Mesh) return;

    // Always update scale and position to match the building mesh
    FBox Bounds = Mesh->Bounds.GetBox();
    FVector BoxExtent = Bounds.GetExtent();
    FVector Origin = Bounds.GetCenter();
    HighlightBox->SetWorldLocation(Origin);
    HighlightBox->SetWorldScale3D(BoxExtent / 50.0f * 1.1f); // 10% bigger than building

    HighlightBox->SetVisibility(true);
    HighlightBox->SetHiddenInGame(false);

    UMaterialInstanceDynamic* DynMat = HighlightBox->CreateAndSetMaterialInstanceDynamic(0);
    if (DynMat)
    {
        DynMat->SetVectorParameterValue("Color", Color); // Material must support "Color" parameter
        UE_LOG(LogTemp, Warning, TEXT("[HighlightBox] Material color set to R:%.2f G:%.2f B:%.2f A:%.2f on %s"), Color.R, Color.G, Color.B, Color.A, *GetName());
        if (FMath::IsNearlyEqual(Color.R, 0.2f, 0.05f) && FMath::IsNearlyEqual(Color.G, 1.0f, 0.05f) && FMath::IsNearlyEqual(Color.B, 0.2f, 0.05f))
        {
            UE_LOG(LogTemp, Warning, TEXT("[HighlightBox] LIGHT GREEN highlight applied for AVAILABILITY on %s"), *GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[HighlightBox] Failed to create dynamic material on %s"), *GetName());
    }
}

// Called every frame
void ABulding::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);
}

