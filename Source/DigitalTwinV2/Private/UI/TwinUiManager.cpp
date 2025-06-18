// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineUtils.h"

#include "UI/TwinUiManager.h"
#include "Blueprint/UserWidget.h"
#include "UI/DaynNightSlider.h"
#include "UI/MenuWidget.h"
#include "UI/FilterMenu.h"
#include "UI/OverviewWidget.h"
#include "Atmosphere/Atmosphere.h"
#include "Kismet/GameplayStatics.h"
#include "Buldings/Bulding.h"
#include "Buldings/GolfClubBuilding.h"
#include "Camera/BirdEye.h"
#include "Components/Button.h"

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

    // Only show Overview if Overview tab is selected AND a valid building is selected
    if (WOverview)
    {
        bool bShowOverview = (CurrentTap == EMenuTap::Overview) && CurrentBuilding &&
            (CurrentBuilding->BuildingType == EBuildingType::OneStoryVilla ||
             CurrentBuilding->BuildingType == EBuildingType::Palace ||
             CurrentBuilding->BuildingType == EBuildingType::StandAloneVilla);
        WOverview->SetVisibility(bShowOverview ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
    if (WFilter) {
        WFilter->SetVisibility(CurrentTap == EMenuTap::Filters ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
        // Hide all highlights if not in Filters tab
        if (CurrentTap != EMenuTap::Filters) {
            UWorld* World = GetWorld();
            if (World && WFilter) {
                for (TActorIterator<ABulding> It(World); It; ++It) {
                    ABulding* Building = *It;
                    if (Building && Building->HighlightBox) {
                        Building->HighlightBox->SetVisibility(false);
                        Building->HighlightBox->SetHiddenInGame(true);
                    }
                }
            }
        }
    }
}



void ATwinUiManager::SetCurrentBuilding(ABulding* NewBuilding)
{
    if (CurrentBuilding != NewBuilding)
    {
        CurrentBuilding = NewBuilding;
        OnCurrentBuildingChanged.Broadcast(CurrentBuilding);

        // Check if the current building is a GolfClubBuilding
        if (Menu && Menu->GolfGameButton)
        {
            if (Cast<AGolfClubBuilding>(CurrentBuilding))
            {
                if (UWidget* GolfWidget = Cast<UWidget>(Menu->GolfGameButton))
				{
					GolfWidget->SetVisibility(ESlateVisibility::Visible);
				}
            }
            else
           {
				if (UWidget* GolfWidget = Cast<UWidget>(Menu->GolfGameButton))
				{
					GolfWidget->SetVisibility(ESlateVisibility::Collapsed);
				}
			}
        }
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
            Menu->UiManager = this;

            // Bind GolfGameButton click event
            if (Menu->GolfGameButton)
            {
                Menu->GolfGameButton->OnClicked.AddDynamic(this, &ATwinUiManager::OnGolfGameButtonClicked);
            }
        }
    }

    SetCurrentTap(CurrentTap);
}
void ATwinUiManager::OnGolfGameButtonClicked()
{
    UGameplayStatics::OpenLevel(this, FName("GolfScene"));
}