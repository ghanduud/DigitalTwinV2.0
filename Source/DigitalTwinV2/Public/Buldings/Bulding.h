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

UCLASS()
class DIGITALTWINV2_API ABulding : public AActor
{
	GENERATED_BODY()
public:
	ABulding();
protected:
	virtual void BeginPlay() override;
public:
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
};
