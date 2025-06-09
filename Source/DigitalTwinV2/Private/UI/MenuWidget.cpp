// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MenuWidget.h"
#include "Components/Button.h"
#include "UI/TwinUiManager.h"

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
