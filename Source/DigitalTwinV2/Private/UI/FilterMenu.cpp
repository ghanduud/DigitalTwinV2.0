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

    // Add sold checkboxes to an array for easy access
    SoldCheckBoxes = {
        SoldStandAloneVillaCheck,
        SoldOneStoryVillaCheck,
        SoldPalaceCheck
    };
    // Bind sold checkboxes
    if (SoldStandAloneVillaCheck)
        SoldStandAloneVillaCheck->OnCheckStateChanged.AddDynamic(this, &UFilterMenu::SoldStandAloneVillaCheckChanged);
    if (SoldOneStoryVillaCheck)
        SoldOneStoryVillaCheck->OnCheckStateChanged.AddDynamic(this, &UFilterMenu::SoldOneStoryVillaCheckChanged);
    if (SoldPalaceCheck)
        SoldPalaceCheck->OnCheckStateChanged.AddDynamic(this, &UFilterMenu::SoldPalaceCheckChanged);
    // Disable all sold checkboxes initially
    for (UCheckBox* Box : SoldCheckBoxes)
    {
        if (Box) Box->SetIsEnabled(false);
    }

    // Bind buttons to checkboxes
    if (StandalonevillaButton && StandAloneVillaCheck)
    {
        StandalonevillaButton->OnClicked.AddDynamic(this, &UFilterMenu::OnStandalonevillaButtonClicked);
    }
    if (OneStoryvillaButton && OneStoryVillaCheck)
    {
        OneStoryvillaButton->OnClicked.AddDynamic(this, &UFilterMenu::OnOneStoryvillaButtonClicked);
    }
    if (PalaceButton && PalaceCheck)
    {
        PalaceButton->OnClicked.AddDynamic(this, &UFilterMenu::OnPalaceButtonClicked);
    }
}

// Individual handlers
void UFilterMenu::StandAloneVillaCheckChanged(bool bIsChecked)
{
    UE_LOG(LogTemp, Warning, TEXT("[FilterMenu] StandAloneVillaCheckChanged called: %s"), bIsChecked ? TEXT("Checked") : TEXT("Unchecked"));
    if (bIsChecked)
    {
        HandleExclusiveCheck(StandAloneVillaCheck);
        // Enable only the relevant availability checkboxB
        if (AvailableStandAloneVillaCheck) AvailableStandAloneVillaCheck->SetIsEnabled(true);
        if (AvailableOneStoryVillaCheck) AvailableOneStoryVillaCheck->SetIsEnabled(false); if (AvailableOneStoryVillaCheck) AvailableOneStoryVillaCheck->SetIsChecked(false);
        if (AvailablePalaceCheck) AvailablePalaceCheck->SetIsEnabled(false); if (AvailablePalaceCheck) AvailablePalaceCheck->SetIsChecked(false);
        // Enable only the relevant sold checkbox
        if (SoldStandAloneVillaCheck) SoldStandAloneVillaCheck->SetIsEnabled(true);
        if (SoldOneStoryVillaCheck) { SoldOneStoryVillaCheck->SetIsEnabled(false); SoldOneStoryVillaCheck->SetIsChecked(false); }
        if (SoldPalaceCheck) { SoldPalaceCheck->SetIsEnabled(false); SoldPalaceCheck->SetIsChecked(false); }
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
        if (SoldStandAloneVillaCheck) SoldStandAloneVillaCheck->SetIsEnabled(false); if (SoldStandAloneVillaCheck) SoldStandAloneVillaCheck->SetIsChecked(false);
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
        // Enable only the relevant sold checkbox
        if (SoldOneStoryVillaCheck) SoldOneStoryVillaCheck->SetIsEnabled(true);
        if (SoldStandAloneVillaCheck) { SoldStandAloneVillaCheck->SetIsEnabled(false); SoldStandAloneVillaCheck->SetIsChecked(false); }
        if (SoldPalaceCheck) { SoldPalaceCheck->SetIsEnabled(false); SoldPalaceCheck->SetIsChecked(false); }
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
        if (SoldOneStoryVillaCheck) SoldOneStoryVillaCheck->SetIsEnabled(false); if (SoldOneStoryVillaCheck) SoldOneStoryVillaCheck->SetIsChecked(false);
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
        // Enable only the relevant sold checkbox
        if (SoldPalaceCheck) SoldPalaceCheck->SetIsEnabled(true);
        if (SoldStandAloneVillaCheck) { SoldStandAloneVillaCheck->SetIsEnabled(false); SoldStandAloneVillaCheck->SetIsChecked(false); }
        if (SoldOneStoryVillaCheck) { SoldOneStoryVillaCheck->SetIsEnabled(false); SoldOneStoryVillaCheck->SetIsChecked(false); }
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
        if (SoldPalaceCheck) SoldPalaceCheck->SetIsEnabled(false); if (SoldPalaceCheck) SoldPalaceCheck->SetIsChecked(false);
    }
}

// --- Availability Check Handlers ---
void UFilterMenu::AvailableStandAloneVillaCheckChanged(bool bIsChecked)
{
    UWorld* World = GetWorld();
    if (!World) return;
    for (TActorIterator<ABulding> It(World); It; ++It)
    {
        ABulding* Building = *It;
        if (Building && Building->BuildingType == EBuildingType::StandAloneVilla)
        {
            if (bIsChecked && Building->bIsAvailable)
                Building->ShowHighlightBox(FLinearColor(0.2f, 1.0f, 0.2f, 0.5f)); // Light green
            else
                Building->ShowHighlightBox(FLinearColor(0.1f, 0.4f, 1.0f, 0.5f)); // Light blue
        }
        else if (Building && Building->HighlightBox)
        {
            Building->HighlightBox->SetVisibility(false);
            Building->HighlightBox->SetHiddenInGame(true);
        }
    }
}

void UFilterMenu::AvailableOneStoryVillaCheckChanged(bool bIsChecked)
{
    UWorld* World = GetWorld();
    if (!World) return;
    for (TActorIterator<ABulding> It(World); It; ++It)
    {
        ABulding* Building = *It;
        if (Building && Building->BuildingType == EBuildingType::OneStoryVilla)
        {
            if (bIsChecked && Building->bIsAvailable)
                Building->ShowHighlightBox(FLinearColor(0.2f, 1.0f, 0.2f, 0.5f)); // Light green
            else
                Building->ShowHighlightBox(FLinearColor(0.1f, 0.4f, 1.0f, 0.5f)); // Light blue
        }
        else if (Building && Building->HighlightBox)
        {
            Building->HighlightBox->SetVisibility(false);
            Building->HighlightBox->SetHiddenInGame(true);
        }
    }
}

void UFilterMenu::AvailablePalaceCheckChanged(bool bIsChecked)
{
    UWorld* World = GetWorld();
    if (!World) return;
    for (TActorIterator<ABulding> It(World); It; ++It)
    {
        ABulding* Building = *It;
        if (Building && Building->BuildingType == EBuildingType::Palace)
        {
            if (bIsChecked && Building->bIsAvailable)
                Building->ShowHighlightBox(FLinearColor(0.2f, 1.0f, 0.2f, 0.5f)); // Light green
            else
                Building->ShowHighlightBox(FLinearColor(0.1f, 0.4f, 1.0f, 0.5f)); // Light blue
        }
        else if (Building && Building->HighlightBox)
        {
            Building->HighlightBox->SetVisibility(false);
            Building->HighlightBox->SetHiddenInGame(true);
        }
    }
}

// --- Sold Check Handlers ---
void UFilterMenu::SoldStandAloneVillaCheckChanged(bool bIsChecked)
{
    UWorld* World = GetWorld();
    if (!World) return;
    for (TActorIterator<ABulding> It(World); It; ++It)
    {
        ABulding* Building = *It;
        if (Building && Building->BuildingType == EBuildingType::StandAloneVilla)
        {
            if (bIsChecked && Building->Status == EBuildingStatus::Sold)
                Building->ShowHighlightBox(FLinearColor(1.0f, 0.2f, 0.2f, 0.5f)); // Light red
            else
                Building->ShowHighlightBox(FLinearColor(0.1f, 0.4f, 1.0f, 0.5f)); // Light blue
        }
        else if (Building && Building->HighlightBox)
        {
            Building->HighlightBox->SetVisibility(false);
            Building->HighlightBox->SetHiddenInGame(true);
        }
    }
}

void UFilterMenu::SoldOneStoryVillaCheckChanged(bool bIsChecked)
{
    UWorld* World = GetWorld();
    if (!World) return;
    for (TActorIterator<ABulding> It(World); It; ++It)
    {
        ABulding* Building = *It;
        if (Building && Building->BuildingType == EBuildingType::OneStoryVilla)
        {
            if (bIsChecked && Building->Status == EBuildingStatus::Sold)
                Building->ShowHighlightBox(FLinearColor(1.0f, 0.2f, 0.2f, 0.5f)); // Light red
            else
                Building->ShowHighlightBox(FLinearColor(0.1f, 0.4f, 1.0f, 0.5f)); // Light blue
        }
        else if (Building && Building->HighlightBox)
        {
            Building->HighlightBox->SetVisibility(false);
            Building->HighlightBox->SetHiddenInGame(true);
        }
    }
}

void UFilterMenu::SoldPalaceCheckChanged(bool bIsChecked)
{
    UWorld* World = GetWorld();
    if (!World) return;
    for (TActorIterator<ABulding> It(World); It; ++It)
    {
        ABulding* Building = *It;
        if (Building && Building->BuildingType == EBuildingType::Palace)
        {
            if (bIsChecked && Building->Status == EBuildingStatus::Sold)
                Building->ShowHighlightBox(FLinearColor(1.0f, 0.2f, 0.2f, 0.5f)); // Light red
            else
                Building->ShowHighlightBox(FLinearColor(0.1f, 0.4f, 1.0f, 0.5f)); // Light blue
        }
        else if (Building && Building->HighlightBox)
        {
            Building->HighlightBox->SetVisibility(false);
            Building->HighlightBox->SetHiddenInGame(true);
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
    if (StandAloneVillaCheck)
    {
        HandleExclusiveCheck(StandAloneVillaCheck);
        StandAloneVillaCheck->SetIsChecked(false); // Force state change
        StandAloneVillaCheck->SetIsChecked(true);  // Always triggers OnCheckStateChanged
    }
}

void UFilterMenu::OnOneStoryvillaButtonClicked()
{
    if (OneStoryVillaCheck)
    {
        HandleExclusiveCheck(OneStoryVillaCheck);
        OneStoryVillaCheck->SetIsChecked(false);
        OneStoryVillaCheck->SetIsChecked(true);
    }
}

void UFilterMenu::OnPalaceButtonClicked()
{
    if (PalaceCheck)
    {
        HandleExclusiveCheck(PalaceCheck);
        PalaceCheck->SetIsChecked(false);
        PalaceCheck->SetIsChecked(true);
    }
}
