// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FilterMenu.generated.h"


class UCheckBox;
/**
 *
 */
UCLASS()
class DIGITALTWINV2_API UFilterMenu : public UUserWidget
{
	GENERATED_BODY()


public:
	virtual void NativeConstruct() override;

	TArray<UCheckBox*> AllCheckBoxes;



	// These will point to your checkboxes in the widget
	UPROPERTY(meta = (BindWidget))
	UCheckBox* StandAloneVillaCheck;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* OneStoryVillaCheck;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* PalaceCheck;



	UPROPERTY(meta = (BindWidget))
	UCheckBox* AvailableCheck;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* SoldCheck;



	UPROPERTY(meta = (BindWidget))
	class UButton* StandalonevillaButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* OneStoryvillaButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* PalaceButton;



	protected:

	UFUNCTION()
	void StandAloneVillaCheckChanged(bool bIsChecked);

	UFUNCTION()
	void OneStoryVillaCheckChanged(bool bIsChecked);

	UFUNCTION()
	void PalaceCheckChanged(bool bIsChecked);

	UFUNCTION()
	void AvailableCheckChanged(bool bIsChecked);

	UFUNCTION()
	void SoldCheckChanged(bool bIsChecked);


	void HandleExclusiveCheck(UCheckBox* CheckedOne);

	UFUNCTION()
	void OnStandalonevillaButtonClicked();
	UFUNCTION()
	void OnOneStoryvillaButtonClicked();
	UFUNCTION()
	void OnPalaceButtonClicked();
};
