// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DaynNightSlider.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "UI/TwinUiManager.h"

void UDaynNightSlider::NativeConstruct()
{
    Super::NativeConstruct();

    if (DayNightSlider)
    {
        DayNightSlider->OnValueChanged.AddDynamic(this, &UDaynNightSlider::HandleSliderValueChanged);
    }


    if (SunnyBtn)
    {
        SunnyBtn->OnClicked.AddDynamic(this, &UDaynNightSlider::OnSunnyBtnClicked);
    }

    if (RainBtn)
    {
        RainBtn->OnClicked.AddDynamic(this, &UDaynNightSlider::OnRainBtnClicked);
    }

    if (FogBtn)
    {
        FogBtn->OnClicked.AddDynamic(this, &UDaynNightSlider::OnFogBtnClicked);
    }
}

void UDaynNightSlider::HandleSliderValueChanged(float NewValue)
{
    OnSliderValueChanged.Broadcast(NewValue);

    SetTime(NewValue);
}

void UDaynNightSlider::SetSliderValue(float InValue)
{
    if (DayNightSlider)
    {
        DayNightSlider->SetValue(InValue);
    }

    SetTime(InValue);
}

void UDaynNightSlider::SetTime(float Time)
{
    if (TimeLabelText)
    {
        int32 Hour = FMath::FloorToInt(Time);
        int32 Minute = FMath::RoundToInt((Time - Hour) * 60.0f);

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






void UDaynNightSlider::OnSunnyBtnClicked()
{
    OnWeatherChanged.Broadcast(EWeather::Sunny);
}

void UDaynNightSlider::OnRainBtnClicked()
{
    OnWeatherChanged.Broadcast(EWeather::Rain);
}

void UDaynNightSlider::OnFogBtnClicked()
{
    OnWeatherChanged.Broadcast(EWeather::Fog);
}