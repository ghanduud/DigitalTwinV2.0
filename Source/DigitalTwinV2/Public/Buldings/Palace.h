// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buldings/Bulding.h"
#include "Palace.generated.h"

/**
 * 
 */
UCLASS()
class DIGITALTWINV2_API APalace : public ABulding
{
	GENERATED_BODY()
public:
	APalace();
protected:
	virtual void BeginPlay() override;
};
