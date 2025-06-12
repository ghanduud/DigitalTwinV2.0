// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bulding.generated.h"


UENUM(BlueprintType)
enum class EBuildingStatus : uint8
{
	Available     UMETA(DisplayName = "Available"),
	Reserved      UMETA(DisplayName = "Reserved"),
	Sold          UMETA(DisplayName = "Sold"),
	UnderConstruction UMETA(DisplayName = "Under Construction")
};

UENUM(BlueprintType)
enum class EBuildingType : uint8
{
    OneStoryVilla     UMETA(DisplayName = "One Story Villa"),
    Palace            UMETA(DisplayName = "Palace"),
    StandAloneVilla   UMETA(DisplayName = "Stand Alone Villa")
};

USTRUCT(BlueprintType)
struct FBuildingData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString DisplayName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString BuildingType;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ImagePath;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FootPrintArea = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BuldingUnitArea = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float GroundFloorArea = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FirstFloorArea = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RoofFloorArea = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Price = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Discount = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Status;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString NumberOfFloors;
};

UCLASS()
class DIGITALTWINV2_API ABulding : public AActor
{
    GENERATED_BODY()
public:
	ABulding();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	int32 Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	int32 NumberOfRooms;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	float FootPrintArea;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	float BuldingUnitArea;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	float GroundFloorArea;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	float FirstFloorArea;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	float RoofFloorArea;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	float Price;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	float Discount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	EBuildingStatus Status;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	EBuildingType BuildingType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	FString ImagePath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Info")
	FString DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building Info")
    FString NumberOfFloors;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Data")
    int32 DataIndex = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
    bool bIsAvailable = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
    bool bHasDiscount = false;

    // Function to apply data from a struct
    void ApplyBuildingData(const struct FBuildingData& Data);
	UFUNCTION(BlueprintCallable, Category = "Building")
	float GetFinalPrice() const
	{
		return Price - (Price * Discount / 100.0f);
	}
	UFUNCTION(BlueprintCallable, Category = "Building")
	FString GetStatusAsString() const
	{
		switch (Status)
		{
			case EBuildingStatus::Available: return TEXT("Available");
			case EBuildingStatus::Reserved: return TEXT("Reserved");
			case EBuildingStatus::Sold: return TEXT("Sold");
			case EBuildingStatus::UnderConstruction: return TEXT("Under Construction");
			default: return TEXT("Unknown");
		}
	}
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* BoxCollider;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* HighlightBox;

	UFUNCTION()
    void ShowHighlightBox(FLinearColor Color);

	static TArray<FBuildingData> AllBuildingData;
    static void LoadBuildingDataFromJson();
};
