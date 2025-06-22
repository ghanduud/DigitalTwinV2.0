// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gallery.generated.h"

/**
 * 
 */
UCLASS()
class DIGITALTWINV2_API UGallery : public UUserWidget
{
	GENERATED_BODY()

public:
	UGallery(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
};
