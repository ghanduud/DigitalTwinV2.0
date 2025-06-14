// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buldings/Bulding.h"
#include "StandAloneVilla.generated.h"

/**
 * 
 */
UCLASS()
class DIGITALTWINV2_API AStandAloneVilla : public ABulding
{
	GENERATED_BODY()
public:
	AStandAloneVilla();
protected:
	virtual void BeginPlay() override;
};
