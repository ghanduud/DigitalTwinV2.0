// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FilterMenu.generated.h"


class UButton;
/**
 *
 */
UCLASS()
class DIGITALTWINV2_API UFilterMenu : public UUserWidget
{
	GENERATED_BODY()


public:
	virtual void NativeConstruct() override;



	// These will point to your checkboxes in the widget
	UPROPERTY(meta = (BindWidget))
	class UButton* StandalonevillaButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* OneStoryvillaButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* PalaceButton;



	// Standalone Villa sub-filters
	UPROPERTY(meta = (BindWidget))
	UButton* AvailableStandAlone;
	UPROPERTY(meta = (BindWidget))
	UButton* ReservedStandAlone;
	UPROPERTY(meta = (BindWidget))
	UButton* SoldStandAlone;

	// One Story Villa sub-filters
	UPROPERTY(meta = (BindWidget))
	UButton* AvailableOneStory;
	UPROPERTY(meta = (BindWidget))
	UButton* ReservedOneStory;
	UPROPERTY(meta = (BindWidget))
	UButton* SoldOneStory;

	// Palace sub-filters
	UPROPERTY(meta = (BindWidget))
	UButton* AvailablePalace;
	UPROPERTY(meta = (BindWidget))
	UButton* ReservedPalace;
	UPROPERTY(meta = (BindWidget))
	UButton* SoldPalace;



	protected:


	UFUNCTION()
	void OnStandalonevillaButtonClicked();
	UFUNCTION()
	void OnOneStoryvillaButtonClicked();
	UFUNCTION()
	void OnPalaceButtonClicked();


	public:

	UFUNCTION(BlueprintCallable)
	void ApplyStatusFilters();

private:
	TArray<UButton*> StandAloneSubFilters;
	TArray<UButton*> OneStorySubFilters;
	TArray<UButton*> PalaceSubFilters;
	void SetSubFiltersEnabled(const TArray<UButton*>& SubFilters, bool bEnabled);

	// Add per-type active flags as member variables (move from file-scope to class-scope)
	bool bStandaloneVillaMainFilterActive = true;
	bool bOneStoryVillaMainFilterActive = true;
	bool bPalaceMainFilterActive = true;

	// Per-type, per-status sub-filter flags
	bool bShowAvailableStandAlone = true;
	bool bShowReservedStandAlone = true;
	bool bShowSoldStandAlone = true;
	bool bShowAvailableOneStory = true;
	bool bShowReservedOneStory = true;
	bool bShowSoldOneStory = true;
	bool bShowAvailablePalace = true;
	bool bShowReservedPalace = true;
	bool bShowSoldPalace = true;

	// Sub-filter button handlers
	UFUNCTION()
	void OnAvailableStandAloneClicked();
	UFUNCTION()
	void OnReservedStandAloneClicked();
	UFUNCTION()
	void OnSoldStandAloneClicked();
	UFUNCTION()
	void OnAvailableOneStoryClicked();
	UFUNCTION()
	void OnReservedOneStoryClicked();
	UFUNCTION()
	void OnSoldOneStoryClicked();
	UFUNCTION()
	void OnAvailablePalaceClicked();
	UFUNCTION()
	void OnReservedPalaceClicked();
	UFUNCTION()
	void OnSoldPalaceClicked();
};
