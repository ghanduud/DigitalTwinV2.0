#include "UI/FilterMenu.h"
#include "Components/CheckBox.h"
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

    // Add all checkboxes to the array for easy access
    AllCheckBoxes = {
        StandAloneVillaCheck,
        OneStoryVillaCheck,
        PalaceCheck
    };

    // Bind with lambda that captures the checkbox pointer
    if (StandAloneVillaCheck)
        StandAloneVillaCheck->OnCheckStateChanged.AddDynamic(this, &UFilterMenu::StandAloneVillaCheckChanged);
    if (OneStoryVillaCheck)
        OneStoryVillaCheck->OnCheckStateChanged.AddDynamic(this, &UFilterMenu::OneStoryVillaCheckChanged);
    if (PalaceCheck)
        PalaceCheck->OnCheckStateChanged.AddDynamic(this, &UFilterMenu::PalaceCheckChanged);

    // Add availability checkboxes to an array for easy access
    AvailabilityCheckBoxes = {
        AvailableStandAloneVillaCheck,
        AvailableOneStoryVillaCheck,
        AvailablePalaceCheck
    };
    // Bind availability checkboxes
    if (AvailableStandAloneVillaCheck)
        AvailableStandAloneVillaCheck->OnCheckStateChanged.AddDynamic(this, &UFilterMenu::AvailableStandAloneVillaCheckChanged);
    if (AvailableOneStoryVillaCheck)
        AvailableOneStoryVillaCheck->OnCheckStateChanged.AddDynamic(this, &UFilterMenu::AvailableOneStoryVillaCheckChanged);
    if (AvailablePalaceCheck)
        AvailablePalaceCheck->OnCheckStateChanged.AddDynamic(this, &UFilterMenu::AvailablePalaceCheckChanged);

    // Disable all availability checkboxes initially
    for (UCheckBox* Box : AvailabilityCheckBoxes)
    {
        if (Box) Box->SetIsEnabled(false);
    }
}

// Individual handlers
void UFilterMenu::StandAloneVillaCheckChanged(bool bIsChecked)
{
    if (bIsChecked)
    {
        HandleExclusiveCheck(StandAloneVillaCheck);
        // Enable only the relevant availability checkboxB
        if (AvailableStandAloneVillaCheck) AvailableStandAloneVillaCheck->SetIsEnabled(true);
        if (AvailableOneStoryVillaCheck) AvailableOneStoryVillaCheck->SetIsEnabled(false); if (AvailableOneStoryVillaCheck) AvailableOneStoryVillaCheck->SetIsChecked(false);
        if (AvailablePalaceCheck) AvailablePalaceCheck->SetIsEnabled(false); if (AvailablePalaceCheck) AvailablePalaceCheck->SetIsChecked(false);
        // Highlight all StandAloneVilla buildings with light blue
        UWorld* World = GetWorld();
        if (World)
        {
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
    }
    else
    {
        if (AvailableStandAloneVillaCheck) AvailableStandAloneVillaCheck->SetIsEnabled(false); if (AvailableStandAloneVillaCheck) AvailableStandAloneVillaCheck->SetIsChecked(false);
    }
}

void UFilterMenu::OneStoryVillaCheckChanged(bool bIsChecked)
{
    if (bIsChecked)
    {
        HandleExclusiveCheck(OneStoryVillaCheck);
        if (AvailableOneStoryVillaCheck) AvailableOneStoryVillaCheck->SetIsEnabled(true);
        if (AvailableStandAloneVillaCheck) AvailableStandAloneVillaCheck->SetIsEnabled(false); if (AvailableStandAloneVillaCheck) AvailableStandAloneVillaCheck->SetIsChecked(false);
        if (AvailablePalaceCheck) AvailablePalaceCheck->SetIsEnabled(false); if (AvailablePalaceCheck) AvailablePalaceCheck->SetIsChecked(false);
        // Highlight all OneStoryVilla buildings with light blue
        UWorld* World = GetWorld();
        if (World)
        {
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
    }
    else
    {
        if (AvailableOneStoryVillaCheck) AvailableOneStoryVillaCheck->SetIsEnabled(false); if (AvailableOneStoryVillaCheck) AvailableOneStoryVillaCheck->SetIsChecked(false);
    }
}

void UFilterMenu::PalaceCheckChanged(bool bIsChecked)
{
    if (bIsChecked)
    {
        HandleExclusiveCheck(PalaceCheck);
        if (AvailablePalaceCheck) AvailablePalaceCheck->SetIsEnabled(true);
        if (AvailableStandAloneVillaCheck) AvailableStandAloneVillaCheck->SetIsEnabled(false); if (AvailableStandAloneVillaCheck) AvailableStandAloneVillaCheck->SetIsChecked(false);
        if (AvailableOneStoryVillaCheck) AvailableOneStoryVillaCheck->SetIsEnabled(false); if (AvailableOneStoryVillaCheck) AvailableOneStoryVillaCheck->SetIsChecked(false);
        // Highlight all Palace buildings with light blue
        UWorld* World = GetWorld();
        if (World)
        {
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
    }
    else
    {
        if (AvailablePalaceCheck) AvailablePalaceCheck->SetIsEnabled(false); if (AvailablePalaceCheck) AvailablePalaceCheck->SetIsChecked(false);
    }
}

// --- Availability Check Handlers ---
void UFilterMenu::AvailableStandAloneVillaCheckChanged(bool bIsChecked)
{
    if (bIsChecked)
    {
        // Uncheck other availability checkboxes
        if (AvailableOneStoryVillaCheck) AvailableOneStoryVillaCheck->SetIsChecked(false);
        if (AvailablePalaceCheck) AvailablePalaceCheck->SetIsChecked(false);
        // Only proceed if StandAloneVillaCheck is checked
        if (StandAloneVillaCheck && StandAloneVillaCheck->IsChecked())
        {
            UWorld* World = GetWorld();
            if (World)
            {
                for (TActorIterator<ABulding> It(World); It; ++It)
                {
                    ABulding* Building = *It;
                    if (Building && Building->BuildingType == EBuildingType::StandAloneVilla && Building->bIsAvailable) // bIsAvailable should be set from JSON
                    {
                        Building->ShowHighlightBox(FLinearColor(0.2f, 1.0f, 0.2f, 0.5f)); // Light green
                    }
                    else if (Building && Building->HighlightBox)
                    {
                        Building->HighlightBox->SetVisibility(false);
                        Building->HighlightBox->SetHiddenInGame(true);
                    }
                }
            }
        }
        else
        {
            // If type not checked, uncheck this
            if (AvailableStandAloneVillaCheck) AvailableStandAloneVillaCheck->SetIsChecked(false);
        }
    }
}

void UFilterMenu::AvailableOneStoryVillaCheckChanged(bool bIsChecked)
{
    if (bIsChecked)
    {
        if (AvailableStandAloneVillaCheck) AvailableStandAloneVillaCheck->SetIsChecked(false);
        if (AvailablePalaceCheck) AvailablePalaceCheck->SetIsChecked(false);
        if (OneStoryVillaCheck && OneStoryVillaCheck->IsChecked())
        {
            UWorld* World = GetWorld();
            if (World)
            {
                for (TActorIterator<ABulding> It(World); It; ++It)
                {
                    ABulding* Building = *It;
                    if (Building && Building->BuildingType == EBuildingType::OneStoryVilla && Building->bIsAvailable)
                    {
                        Building->ShowHighlightBox(FLinearColor(0.2f, 1.0f, 0.2f, 0.5f));
                    }
                    else if (Building && Building->HighlightBox)
                    {
                        Building->HighlightBox->SetVisibility(false);
                        Building->HighlightBox->SetHiddenInGame(true);
                    }
                }
            }
        }
        else
        {
            if (AvailableOneStoryVillaCheck) AvailableOneStoryVillaCheck->SetIsChecked(false);
        }
    }
}

void UFilterMenu::AvailablePalaceCheckChanged(bool bIsChecked)
{
    if (bIsChecked)
    {
        if (AvailableStandAloneVillaCheck) AvailableStandAloneVillaCheck->SetIsChecked(false);
        if (AvailableOneStoryVillaCheck) AvailableOneStoryVillaCheck->SetIsChecked(false);
        if (PalaceCheck && PalaceCheck->IsChecked())
        {
            UWorld* World = GetWorld();
            if (World)
            {
                for (TActorIterator<ABulding> It(World); It; ++It)
                {
                    ABulding* Building = *It;
                    if (Building && Building->BuildingType == EBuildingType::Palace && Building->bIsAvailable)
                    {
                        Building->ShowHighlightBox(FLinearColor(0.2f, 1.0f, 0.2f, 0.5f));
                    }
                    else if (Building && Building->HighlightBox)
                    {
                        Building->HighlightBox->SetVisibility(false);
                        Building->HighlightBox->SetHiddenInGame(true);
                    }
                }
            }
        }
        else
        {
            if (AvailablePalaceCheck) AvailablePalaceCheck->SetIsChecked(false);
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
