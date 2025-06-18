#include "UI/FilterMenu.h"
#include "Components/CheckBox.h"
#include "Components/Button.h"
#include "EngineUtils.h"
#include "Buldings/Bulding.h"
#include "Buldings/OneStoryVilla.h"
#include "Buldings/StandAloneVilla.h"
#include "Buldings/Palace.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"

void UFilterMenu::NativeConstruct()
{
    Super::NativeConstruct();

    AllCheckBoxes.Empty();
    if (StandAloneVillaCheck) AllCheckBoxes.Add(StandAloneVillaCheck);
    if (OneStoryVillaCheck) AllCheckBoxes.Add(OneStoryVillaCheck);
    if (PalaceCheck) AllCheckBoxes.Add(PalaceCheck);

    if (StandAloneVillaCheck)
        StandAloneVillaCheck->OnCheckStateChanged.AddDynamic(this, &UFilterMenu::StandAloneVillaCheckChanged);
    if (OneStoryVillaCheck)
        OneStoryVillaCheck->OnCheckStateChanged.AddDynamic(this, &UFilterMenu::OneStoryVillaCheckChanged);
    if (PalaceCheck)
        PalaceCheck->OnCheckStateChanged.AddDynamic(this, &UFilterMenu::PalaceCheckChanged);

    if (AvailableCheck)
        AvailableCheck->OnCheckStateChanged.AddDynamic(this, &UFilterMenu::AvailableCheckChanged);
    if (SoldCheck)
        SoldCheck->OnCheckStateChanged.AddDynamic(this, &UFilterMenu::SoldCheckChanged);

    if (StandalonevillaButton && StandAloneVillaCheck)
        StandalonevillaButton->OnClicked.AddDynamic(this, &UFilterMenu::OnStandalonevillaButtonClicked);
    if (OneStoryvillaButton && OneStoryVillaCheck)
        OneStoryvillaButton->OnClicked.AddDynamic(this, &UFilterMenu::OnOneStoryvillaButtonClicked);
    if (PalaceButton && PalaceCheck)
        PalaceButton->OnClicked.AddDynamic(this, &UFilterMenu::OnPalaceButtonClicked);
}

// Individual handlers
void UFilterMenu::StandAloneVillaCheckChanged(bool bIsChecked)
{
    UE_LOG(LogTemp, Warning, TEXT("[FilterMenu] StandAloneVillaCheckChanged called: %s"), bIsChecked ? TEXT("Checked") : TEXT("Unchecked"));
    UWorld* World = GetWorld();
    if (!World) return;
    if (bIsChecked)
    {
        HandleExclusiveCheck(StandAloneVillaCheck);
        // Highlight all StandAloneVilla buildings with light blue
        for (TActorIterator<ABulding> It(World); It; ++It)
        {
            ABulding* Building = *It;
            if (Building && Building->BuildingType == EBuildingType::StandAloneVilla)
            {
                Building->ShowHighlightBox(FLinearColor(0.1f, 0.4f, 1.0f, 0.5f));
            }
            else if (Building && Building->HighlightBox)
            {
                Building->HighlightBox->SetVisibility(false);
                Building->HighlightBox->SetHiddenInGame(true);
            }
        }
    }
    else
    {
        // Remove highlight for StandAloneVilla buildings
        for (TActorIterator<ABulding> It(World); It; ++It)
        {
            ABulding* Building = *It;
            if (Building && Building->BuildingType == EBuildingType::StandAloneVilla && Building->HighlightBox)
            {
                Building->HighlightBox->SetVisibility(false);
                Building->HighlightBox->SetHiddenInGame(true);
            }
        }
    }
}

void UFilterMenu::OneStoryVillaCheckChanged(bool bIsChecked)
{
    UWorld* World = GetWorld();
    if (!World) return;
    if (bIsChecked)
    {
        HandleExclusiveCheck(OneStoryVillaCheck);
        // Highlight all OneStoryVilla buildings with light blue
        for (TActorIterator<ABulding> It(World); It; ++It)
        {
            ABulding* Building = *It;
            if (Building && Building->BuildingType == EBuildingType::OneStoryVilla)
            {
                Building->ShowHighlightBox(FLinearColor(0.1f, 0.4f, 1.0f, 0.5f));
            }
            else if (Building && Building->HighlightBox)
            {
                Building->HighlightBox->SetVisibility(false);
                Building->HighlightBox->SetHiddenInGame(true);
            }
        }
    }
    else
    {
        // Remove highlight for OneStoryVilla buildings
        for (TActorIterator<ABulding> It(World); It; ++It)
        {
            ABulding* Building = *It;
            if (Building && Building->BuildingType == EBuildingType::OneStoryVilla && Building->HighlightBox)
            {
                Building->HighlightBox->SetVisibility(false);
                Building->HighlightBox->SetHiddenInGame(true);
            }
        }
    }
}

void UFilterMenu::PalaceCheckChanged(bool bIsChecked)
{
    UWorld* World = GetWorld();
    if (!World) return;
    if (bIsChecked)
    {
        HandleExclusiveCheck(PalaceCheck);
        // Highlight all Palace buildings with light blue
        for (TActorIterator<ABulding> It(World); It; ++It)
        {
            ABulding* Building = *It;
            if (Building && Building->BuildingType == EBuildingType::Palace)
            {
                Building->ShowHighlightBox(FLinearColor(0.1f, 0.4f, 1.0f, 0.5f));
            }
            else if (Building && Building->HighlightBox)
            {
                Building->HighlightBox->SetVisibility(false);
                Building->HighlightBox->SetHiddenInGame(true);
            }
        }
    }
    else
    {
        // Remove highlight for Palace buildings
        for (TActorIterator<ABulding> It(World); It; ++It)
        {
            ABulding* Building = *It;
            if (Building && Building->BuildingType == EBuildingType::Palace && Building->HighlightBox)
            {
                Building->HighlightBox->SetVisibility(false);
                Building->HighlightBox->SetHiddenInGame(true);
            }
        }
    }
}

// --- Availability Check Handlers ---
void UFilterMenu::AvailableCheckChanged(bool bIsChecked)
{
    if (bIsChecked && SoldCheck && SoldCheck->IsChecked()) {
        SoldCheck->SetIsChecked(false);
    }
    UWorld* World = GetWorld();
    if (!World) return;
    EBuildingType SelectedType = EBuildingType::StandAloneVilla;
    if (StandAloneVillaCheck && StandAloneVillaCheck->IsChecked()) SelectedType = EBuildingType::StandAloneVilla;
    else if (OneStoryVillaCheck && OneStoryVillaCheck->IsChecked()) SelectedType = EBuildingType::OneStoryVilla;
    else if (PalaceCheck && PalaceCheck->IsChecked()) SelectedType = EBuildingType::Palace;
    if (bIsChecked)
    {
        for (TActorIterator<ABulding> It(World); It; ++It)
        {
            ABulding* Building = *It;
            if (Building && Building->BuildingType == SelectedType && Building->bIsAvailable)
                Building->ShowHighlightBox(FLinearColor(0.2f, 1.0f, 0.2f, 0.5f)); // Green
            else if (Building && Building->BuildingType == SelectedType)
                Building->ShowHighlightBox(FLinearColor(0.1f, 0.4f, 1.0f, 0.5f)); // Blue
            else if (Building && Building->HighlightBox)
            {
                Building->HighlightBox->SetVisibility(false);
                Building->HighlightBox->SetHiddenInGame(true);
            }
        }
    }
    else
    {
        // Restore blue highlight for all buildings of selected type (remove green highlight only)
        for (TActorIterator<ABulding> It(World); It; ++It)
        {
            ABulding* Building = *It;
            if (Building && Building->BuildingType == SelectedType)
                Building->ShowHighlightBox(FLinearColor(0.1f, 0.4f, 1.0f, 0.5f)); // Blue
        }
    }
}

// --- Sold Check Handlers ---
void UFilterMenu::SoldCheckChanged(bool bIsChecked)
{
    if (bIsChecked && AvailableCheck && AvailableCheck->IsChecked()) {
        AvailableCheck->SetIsChecked(false);
    }
    UWorld* World = GetWorld();
    if (!World) return;
    EBuildingType SelectedType = EBuildingType::StandAloneVilla;
    if (StandAloneVillaCheck && StandAloneVillaCheck->IsChecked()) SelectedType = EBuildingType::StandAloneVilla;
    else if (OneStoryVillaCheck && OneStoryVillaCheck->IsChecked()) SelectedType = EBuildingType::OneStoryVilla;
    else if (PalaceCheck && PalaceCheck->IsChecked()) SelectedType = EBuildingType::Palace;
    if (bIsChecked)
    {
        for (TActorIterator<ABulding> It(World); It; ++It)
        {
            ABulding* Building = *It;
            if (Building && Building->BuildingType == SelectedType && Building->Status == EBuildingStatus::Sold)
                Building->ShowHighlightBox(FLinearColor(1.0f, 1.0f, 0.2f, 0.5f)); // Yellow
            else if (Building && Building->BuildingType == SelectedType)
                Building->ShowHighlightBox(FLinearColor(0.1f, 0.4f, 1.0f, 0.5f)); // Blue
            else if (Building && Building->HighlightBox)
            {
                Building->HighlightBox->SetVisibility(false);
                Building->HighlightBox->SetHiddenInGame(true);
            }
        }
    }
    else
    {
        // Restore blue highlight for all buildings of selected type (remove yellow highlight only)
        for (TActorIterator<ABulding> It(World); It; ++It)
        {
            ABulding* Building = *It;
            if (Building && Building->BuildingType == SelectedType)
                Building->ShowHighlightBox(FLinearColor(0.1f, 0.4f, 1.0f, 0.5f)); // Blue
        }
    }
}

void UFilterMenu::HandleExclusiveCheck(UCheckBox* CheckedOne)
{
    for (UCheckBox* Box : AllCheckBoxes)
    {
        if (Box && Box != CheckedOne)
        {
            Box->SetIsChecked(false);
        }
    }
}

void UFilterMenu::OnStandalonevillaButtonClicked()
{
    HandleExclusiveCheck(StandAloneVillaCheck);
    StandAloneVillaCheck->SetIsChecked(true);
    StandAloneVillaCheckChanged(true); // Directly call the filter logic
}

void UFilterMenu::OnOneStoryvillaButtonClicked()
{
    HandleExclusiveCheck(OneStoryVillaCheck);
    OneStoryVillaCheck->SetIsChecked(true);
    OneStoryVillaCheckChanged(true);
}

void UFilterMenu::OnPalaceButtonClicked()
{
    HandleExclusiveCheck(PalaceCheck);
    PalaceCheck->SetIsChecked(true);
    PalaceCheckChanged(true);
}
