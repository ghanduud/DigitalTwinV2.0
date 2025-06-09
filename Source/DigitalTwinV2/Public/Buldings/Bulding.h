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

UCLASS()
class DIGITALTWINV2_API ABulding : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABulding();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;




	//data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Budling")
	int32 Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Budling")
	int32 NumberOfRooms;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Budling")
	float FootPrintArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Budling")
	float BuldingUnitArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Budling")
	float Price;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Budling")
	float Discount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Budling")
	EBuildingStatus Status;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	FString ImagePath;






	//components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* BoxCollider;

};
