// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GolfBox.generated.h"

UCLASS()
class DIGITALTWINV2_API AGolfBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGolfBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
   UPROPERTY(VisibleAnywhere)
    UBoxComponent* BoxCollider;

    UPROPERTY(VisibleAnywhere) // Add StaticMesh property declaration
    UStaticMeshComponent* StaticMesh;

    UFUNCTION()
    void OnBoxClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
};
