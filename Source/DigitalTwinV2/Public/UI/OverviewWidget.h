// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverviewWidget.generated.h"

class UImage;
class UTexture2D;

/**
 *
 */
UCLASS()
class DIGITALTWINV2_API UOverviewWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UOverviewWidget(const FObjectInitializer& ObjectInitializer);

	/** Loads and sets a texture from a given path */
	UFUNCTION(BlueprintCallable, Category = "Overview")
	void SetUnitImageFromPath(const FString& AssetPath);


	UFUNCTION(BlueprintCallable, Category = "Building")
	void SetBuilding(ABulding* NewBuilding);

protected:
	virtual void NativeConstruct() override;

	/** Default image asset path */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overview")
	FString DefaultImagePath;

	/** Image widget to display the texture */
	UPROPERTY(meta = (BindWidget))
	UImage* UnitImage;
};
