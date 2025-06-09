// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DaynNightSlider.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSliderValueChanged, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeatherChanged, EWeather, NewTap);

/**
 *
 */
UCLASS()
class DIGITALTWINV2_API UDaynNightSlider : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class USlider* DayNightSlider;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TimeLabelText;



	UPROPERTY(meta = (BindWidget))
	class UButton* SunnyBtn;

	UPROPERTY(meta = (BindWidget))
	class UButton* RainBtn;

	UPROPERTY(meta = (BindWidget))
	class UButton* FogBtn;

	UPROPERTY(BlueprintAssignable, Category = "Menu")
	FOnWeatherChanged OnWeatherChanged;






	UPROPERTY(BlueprintAssignable)
	FOnSliderValueChanged OnSliderValueChanged;

	void SetSliderValue(float InValue);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleSliderValueChanged(float NewValue);

	void SetTime(float Time);





	UFUNCTION()
	void OnSunnyBtnClicked();

	UFUNCTION()
	void OnRainBtnClicked();

	UFUNCTION()
	void OnFogBtnClicked();

};
