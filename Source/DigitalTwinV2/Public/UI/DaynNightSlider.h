// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DaynNightSlider.generated.h"

/**
 *
 */
UCLASS()
class DIGITALTWINV2_API UDaynNightSlider : public UUserWidget
{
	GENERATED_BODY()

public:

protected:
	virtual void NativeConstruct() override;
};
