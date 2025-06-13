// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buldings/Bulding.h"
#include "GolfClubBuilding.generated.h"

/**
 * 
 */
UCLASS()
class AGolfClubBuilding : public ABulding
{
	GENERATED_BODY()
	
public:
    AGolfClubBuilding();

protected:
    virtual void BeginPlay() override;
};