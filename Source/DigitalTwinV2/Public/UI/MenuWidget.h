// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.generated.h"

// Forward Declarations
class UButton;
class USlider;
class UTextBlock;
class ATwinUiManager;

// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTapChanged, EMenuTap, NewTap);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSliderValueChanged, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeatherChanged, EWeather, NewWeather);

/**
 * UMenuWidget - Main menu widget for controlling UI filters and environmental state.
 */
UCLASS()
class DIGITALTWINV2_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// Delegates
	UPROPERTY(BlueprintAssignable, Category = "Menu")
	FOnTapChanged OnTapChanged;

	UPROPERTY(BlueprintAssignable, Category = "Menu")
	FOnWeatherChanged OnWeatherChanged;

	UPROPERTY(BlueprintAssignable, Category = "Menu")
	FOnSliderValueChanged OnSliderValueChanged;

	// Reference to UI manager
	UPROPERTY(BlueprintReadWrite, Category = "UI")
	ATwinUiManager* UiManager;

	// Public Bindings
	UPROPERTY(meta = (BindWidget))
	UButton* GolfGameButton;

protected:
	// Main Menu Buttons
	UPROPERTY(meta = (BindWidget))
	UButton* OverviewButton;

	UPROPERTY(meta = (BindWidget))
	UButton* FiltersButton;

	UPROPERTY(meta = (BindWidget))
	UButton* AtmosphereButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ResetButton;

	// Time Controls
	UPROPERTY(meta = (BindWidget))
	USlider* DayNightSlider;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TimeLabelText;

	// Weather Controls
	UPROPERTY(meta = (BindWidget))
	UButton* SunnyBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* RainBtn;

private:
	// Button Click Handlers
	UFUNCTION()
	void OnOverviewClicked();

	UFUNCTION()
	void OnFiltersClicked();

	UFUNCTION()
	void OnAtmosphereClicked();

	UFUNCTION()
	void OnResetClicked();

	UFUNCTION()
	void OnSunnyBtnClicked();

	UFUNCTION()
	void OnRainBtnClicked();

	// Slider Handler
	UFUNCTION()
	void HandleSliderValueChanged(float NewValue);

	// Helper Methods
	void SetSliderValue(float InValue);
	void SetTime(float Time);
};
