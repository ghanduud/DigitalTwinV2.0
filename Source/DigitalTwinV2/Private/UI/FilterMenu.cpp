#include "UI/FilterMenu.h"
#include "Components/CheckBox.h"

void UFilterMenu::NativeConstruct()
{
    Super::NativeConstruct();

    // Add all checkboxes to the array for easy access
    AllCheckBoxes = {
        AvailableCheck,
        SoldCheck,
        DiscountCheck,
        StandAloneVillaCheck,
        OneStoryVillaCheck,
        PalaceCheck
    };

    // Bind with lambda that captures the checkbox pointer
    if (AvailableCheck)
        AvailableCheck->OnCheckStateChanged.AddDynamic(this, &UFilterMenu::AvailableCheckChanged);
    if (SoldCheck)
        SoldCheck->OnCheckStateChanged.AddDynamic(this, &UFilterMenu::SoldCheckChanged);
    if (DiscountCheck)
        DiscountCheck->OnCheckStateChanged.AddDynamic(this, &UFilterMenu::DiscountCheckChanged);
    if (StandAloneVillaCheck)
        StandAloneVillaCheck->OnCheckStateChanged.AddDynamic(this, &UFilterMenu::StandAloneVillaCheckChanged);
    if (OneStoryVillaCheck)
        OneStoryVillaCheck->OnCheckStateChanged.AddDynamic(this, &UFilterMenu::OneStoryVillaCheckChanged);
    if (PalaceCheck)
        PalaceCheck->OnCheckStateChanged.AddDynamic(this, &UFilterMenu::PalaceCheckChanged);
}

// Individual handlers
void UFilterMenu::AvailableCheckChanged(bool bIsChecked)
{
    if (bIsChecked)
        HandleExclusiveCheck(AvailableCheck);
}

void UFilterMenu::SoldCheckChanged(bool bIsChecked)
{
    if (bIsChecked)
        HandleExclusiveCheck(SoldCheck);
}

void UFilterMenu::DiscountCheckChanged(bool bIsChecked)
{
    if (bIsChecked)
        HandleExclusiveCheck(DiscountCheck);
}

void UFilterMenu::StandAloneVillaCheckChanged(bool bIsChecked)
{
    if (bIsChecked)
        HandleExclusiveCheck(StandAloneVillaCheck);
}

void UFilterMenu::OneStoryVillaCheckChanged(bool bIsChecked)
{
    if (bIsChecked)
        HandleExclusiveCheck(OneStoryVillaCheck);
}

void UFilterMenu::PalaceCheckChanged(bool bIsChecked)
{
    if (bIsChecked)
        HandleExclusiveCheck(PalaceCheck);
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
