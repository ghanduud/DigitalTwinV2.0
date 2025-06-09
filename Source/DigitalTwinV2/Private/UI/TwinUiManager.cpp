// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TwinUiManager.h"
#include "Blueprint/UserWidget.h"
#include "UI/DaynNightSlider.h"
#include "UI/MenuWidget.h"
#include "UI/FilterMenu.h"
#include "UI/OverviewWidget.h"
#include "Atmosphere/Atmosphere.h"
#include "Kismet/GameplayStatics.h"
#include "Buldings/Bulding.h"
#include "Camera/BirdEye.h"

// Sets default values
ATwinUiManager::ATwinUiManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	// PrimaryActorTick.bCanEverTick = false;
}

void ATwinUiManager::BeginPlay()
{
	Super::BeginPlay();




	if (OverviewWidgetClass)
	{
		WOverview = CreateWidget<UOverviewWidget>(GetWorld(), OverviewWidgetClass);
		if (WOverview)
		{
			WOverview->AddToViewport();
			WOverview->SetVisibility(ESlateVisibility::Collapsed);

			// Bind the building change event to the OverviewWidget's SetBuilding method
			OnCurrentBuildingChanged.AddDynamic(WOverview, &UOverviewWidget::SetBuilding);
		}
	}

	if (FilterWidgetClass)
	{
		WFilter = CreateWidget<UFilterMenu>(GetWorld(), FilterWidgetClass);
		if (WFilter)
		{
			WFilter->AddToViewport();
			WFilter->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (DayNightSliderClass)
	{
		DayAndNightSlider = CreateWidget<UDaynNightSlider>(GetWorld(), DayNightSliderClass);
		if (DayAndNightSlider)
		{
			DayAndNightSlider->AddToViewport(); // Add it now, control visibility later

			DayAndNightSlider->SetSliderValue(DayTime);

			DayAndNightSlider->OnSliderValueChanged.AddDynamic(this, &ATwinUiManager::UpdateDayTimeFromSlider);

			DayAndNightSlider->SetVisibility(ESlateVisibility::Collapsed); // Initially hidden, shown if Atmosphere is active

			DayAndNightSlider->OnWeatherChanged.AddDynamic(this, &ATwinUiManager::SetCurrentWeather);
		}
	}



	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (ABirdEye* BirdEyePawn = Cast<ABirdEye>(PC->GetPawn()))
		{
			// Bind the delegate
			BirdEyePawn->OnBuildingSelected.AddDynamic(this, &ATwinUiManager::SetCurrentBuilding);
		}
	}



}

// // Called every frame
// void ATwinUiManager::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);

// }


void ATwinUiManager::UpdateDayTimeFromSlider(float NewSliderValue)
{
	DayTime = NewSliderValue;

	OnDayTimeChanged.Broadcast(DayTime);
}




void ATwinUiManager::SetCurrentTap(EMenuTap NewTap)
{
	CurrentTap = NewTap;
	UpdateUIVisibility();
}

void ATwinUiManager::SetCurrentWeather(EWeather NewWeather)
{
	if (CurrentWeather != NewWeather)
	{
		CurrentWeather = NewWeather;
		OnWeatherChanged.Broadcast(NewWeather); // Broadcast the change
	}
}


void ATwinUiManager::UpdateUIVisibility()
{
	if (DayAndNightSlider)
	{
		DayAndNightSlider->SetVisibility(CurrentTap == EMenuTap::Atmosphere ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (WOverview)
	{
		WOverview->SetVisibility(CurrentTap == EMenuTap::Overview ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	if (WFilter) { WFilter->SetVisibility(CurrentTap == EMenuTap::Filters ? ESlateVisibility::Visible : ESlateVisibility::Collapsed); }
}



void ATwinUiManager::SetCurrentBuilding(ABulding* NewBuilding)
{
	if (CurrentBuilding != NewBuilding)
	{
		CurrentBuilding = NewBuilding;
		OnCurrentBuildingChanged.Broadcast(CurrentBuilding);
	}
}


void ATwinUiManager::ClearCurrentBuilding()
{
	CurrentBuilding = nullptr;
	OnCurrentBuildingChanged.Broadcast(CurrentBuilding);
}



void ATwinUiManager::ResetCameraAndBuilding()
{
	// Clear current building
	ClearCurrentBuilding();

	OnResetRequested.Broadcast();
}



void ATwinUiManager::OnIntroSequenceFinished()
{
	if (MenuWidgetClass)
	{
		Menu = CreateWidget<UMenuWidget>(GetWorld(), MenuWidgetClass);
		if (Menu)
		{
			Menu->AddToViewport();
			Menu->OnTapChanged.AddDynamic(this, &ATwinUiManager::SetCurrentTap);
			// Set UiManager pointer for the menu
			Menu->UiManager = this;
		}
	}

	SetCurrentTap(CurrentTap);

}