// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GolfGameUI.generated.h"

/**
 *
 */
UCLASS()
class DIGITALTWINV2_API UGolfGameUI : public UUserWidget
{
	GENERATED_BODY()


public:
	virtual void NativeConstruct() override;

protected:

	UPROPERTY(meta = (BindWidget))
	class UButton* LongShotBtn;


	UPROPERTY(meta = (BindWidget))
	class UButton* ShortShotBtn;

	UPROPERTY(meta = (BindWidget))
	class UButton* ChipShotBtn;



	UFUNCTION()
	void HandleLongShotClicked();

	UFUNCTION()
	void HandleShortShotClicked();

	UFUNCTION()
	void HandleChipShotClicked();


};
