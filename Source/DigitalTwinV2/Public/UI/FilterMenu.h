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
	UCheckBox* AvailableStandAloneVillaCheck;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* AvailableOneStoryVillaCheck;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* AvailablePalaceCheck;

	TArray<UCheckBox*> AvailabilityCheckBoxes;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* DiscountStandAloneVillaCheck;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* DiscountOneStoryVillaCheck;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* DiscountPalaceCheck;

	TArray<UCheckBox*> DiscountCheckBoxes;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* SoldStandAloneVillaCheck;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* SoldOneStoryVillaCheck;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* SoldPalaceCheck;

	TArray<UCheckBox*> SoldCheckBoxes;



	protected:

	UFUNCTION()
	void StandAloneVillaCheckChanged(bool bIsChecked);

	UFUNCTION()
	void OneStoryVillaCheckChanged(bool bIsChecked);

	UFUNCTION()
	void PalaceCheckChanged(bool bIsChecked);

	UFUNCTION()
	void AvailableStandAloneVillaCheckChanged(bool bIsChecked);

	UFUNCTION()
	void AvailableOneStoryVillaCheckChanged(bool bIsChecked);

	UFUNCTION()
	void AvailablePalaceCheckChanged(bool bIsChecked);

	UFUNCTION()
	void DiscountStandAloneVillaCheckChanged(bool bIsChecked);

	UFUNCTION()
	void DiscountOneStoryVillaCheckChanged(bool bIsChecked);

	UFUNCTION()
	void DiscountPalaceCheckChanged(bool bIsChecked);

	UFUNCTION()
	void SoldStandAloneVillaCheckChanged(bool bIsChecked);

	UFUNCTION()
	void SoldOneStoryVillaCheckChanged(bool bIsChecked);

	UFUNCTION()
	void SoldPalaceCheckChanged(bool bIsChecked);

	void HandleExclusiveCheck(UCheckBox* CheckedOne);
};
