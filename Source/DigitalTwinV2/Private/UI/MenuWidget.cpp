// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MenuWidget.h"
#include "Components/Button.h"
#include "UI/TwinUiManager.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"

void UMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (OverviewButton)
    {
        OverviewButton->OnClicked.AddDynamic(this, &UMenuWidget::OnOverviewClicked);
    }

    if (FiltersButton)
    {
        FiltersButton->OnClicked.AddDynamic(this, &UMenuWidget::OnFiltersClicked);
    }

    if (AtmosphereButton)
    {
        AtmosphereButton->OnClicked.AddDynamic(this, &UMenuWidget::OnAtmosphereClicked);
    }

    if (ResetButton)
    {
        ResetButton->OnClicked.AddDynamic(this, &UMenuWidget::OnResetClicked);
    }
    if (DayNightSlider)
    {
        DayNightSlider->OnValueChanged.AddDynamic(this, &UMenuWidget::HandleSliderValueChanged);
    }
    if (SunnyBtn)
    {
        SunnyBtn->OnClicked.AddDynamic(this, &UMenuWidget::OnSunnyBtnClicked);
    }
    if (RainBtn)
    {
        RainBtn->OnClicked.AddDynamic(this, &UMenuWidget::OnRainBtnClicked);
    }
}

void UMenuWidget::OnOverviewClicked()
{
    OnTapChanged.Broadcast(EMenuTap::Overview);
}

void UMenuWidget::OnFiltersClicked()
{
    OnTapChanged.Broadcast(EMenuTap::Filters);
}

void UMenuWidget::OnAtmosphereClicked()
{
    OnTapChanged.Broadcast(EMenuTap::Atmosphere);
}

void UMenuWidget::OnResetClicked()
{
    if (UiManager)
    {
        UiManager->ResetCameraAndBuilding();
    }
}

void UMenuWidget::HandleSliderValueChanged(float NewValue)
{
    // Clamp to 6:30 AM (6.5) to 5:00 PM (17.0)
    float ClampedValue = FMath::Clamp(NewValue, 6.5f, 17.0f);
    OnSliderValueChanged.Broadcast(ClampedValue);
    SetTime(ClampedValue);
}

void UMenuWidget::SetSliderValue(float InValue)
{
    float ClampedValue = FMath::Clamp(InValue, 6.5f, 17.0f);
    if (DayNightSlider)
    {
        DayNightSlider->SetValue(ClampedValue);
    }
    SetTime(ClampedValue);
}

void UMenuWidget::SetTime(float Time)
{
    float ClampedTime = FMath::Clamp(Time, 6.5f, 17.0f);
    if (TimeLabelText)
    {
        int32 Hour = FMath::FloorToInt(ClampedTime);
        int32 Minute = FMath::RoundToInt((ClampedTime - Hour) * 60.0f);
        if (Minute >= 60)
        {
            Minute -= 60;
            Hour += 1;
        }
        FString Period = (Hour >= 12) ? TEXT("PM") : TEXT("AM");
        int32 Hour12 = Hour % 12;
        if (Hour12 == 0) Hour12 = 12;
        FString TimeString = FString::Printf(TEXT("%02d:%02d %s"), Hour12, Minute, *Period);
        TimeLabelText->SetText(FText::FromString(TimeString));
    }
}

void UMenuWidget::OnSunnyBtnClicked()
{
    OnWeatherChanged.Broadcast(EWeather::Sunny);
}
void UMenuWidget::OnRainBtnClicked()
{
    OnWeatherChanged.Broadcast(EWeather::RainAndFog);
}
