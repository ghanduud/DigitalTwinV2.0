// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buldings/Bulding.h"
#include "OneStoryVilla.generated.h"

/**
 * 
 */
UCLASS()
class DIGITALTWINV2_API AOneStoryVilla : public ABulding
{
	GENERATED_BODY()
public:
	AOneStoryVilla();
protected:
	virtual void BeginPlay() override;
};
