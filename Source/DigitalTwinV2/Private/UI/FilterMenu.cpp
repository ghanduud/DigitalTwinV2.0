#include "UI/FilterMenu.h"
#include "Components/Button.h"
#include "EngineUtils.h"
#include "Buldings/Bulding.h"
#include "Buldings/OneStoryVilla.h"
#include "Buldings/StandAloneVilla.h"
#include "Buldings/Palace.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "Components/Widget.h"
#include "Components/PrimitiveComponent.h"

bool bMainFilterActive = false;

// Add per-type toggles at the top
bool bStandaloneVillaMainFilterActive = false;
bool bOneStoryVillaMainFilterActive = false;
bool bPalaceMainFilterActive = false;

// Helper to enable/disable sub-filters for a type (now UButton)
void UFilterMenu::SetSubFiltersEnabled(const TArray<UButton*>& SubFilters, bool bEnabled)
{
    for (UButton* Btn : SubFilters)
    {
        if (Btn) Btn->SetIsEnabled(bEnabled);
    }
}

void UFilterMenu::NativeConstruct()
{
    Super::NativeConstruct();

    // --- Setup sub-filter arrays ---
    StandAloneSubFilters = {AvailableStandAlone, ReservedStandAlone, SoldStandAlone};
    OneStorySubFilters = {AvailableOneStory, ReservedOneStory, SoldOneStory};
    PalaceSubFilters = {AvailablePalace, ReservedPalace, SoldPalace};

    if (StandalonevillaButton)
        StandalonevillaButton->OnClicked.AddDynamic(this, &UFilterMenu::OnStandalonevillaButtonClicked);
    if (OneStoryvillaButton)
        OneStoryvillaButton->OnClicked.AddDynamic(this, &UFilterMenu::OnOneStoryvillaButtonClicked);
    if (PalaceButton)
        PalaceButton->OnClicked.AddDynamic(this, &UFilterMenu::OnPalaceButtonClicked);

    // Bind sub-filter button handlers
    if (AvailableStandAlone)
        AvailableStandAlone->OnClicked.AddDynamic(this, &UFilterMenu::OnAvailableStandAloneClicked);
    if (ReservedStandAlone)
        ReservedStandAlone->OnClicked.AddDynamic(this, &UFilterMenu::OnReservedStandAloneClicked);
    if (SoldStandAlone)
        SoldStandAlone->OnClicked.AddDynamic(this, &UFilterMenu::OnSoldStandAloneClicked);
    if (AvailableOneStory)
        AvailableOneStory->OnClicked.AddDynamic(this, &UFilterMenu::OnAvailableOneStoryClicked);
    if (ReservedOneStory)
        ReservedOneStory->OnClicked.AddDynamic(this, &UFilterMenu::OnReservedOneStoryClicked);
    if (SoldOneStory)
        SoldOneStory->OnClicked.AddDynamic(this, &UFilterMenu::OnSoldOneStoryClicked);
    if (AvailablePalace)
        AvailablePalace->OnClicked.AddDynamic(this, &UFilterMenu::OnAvailablePalaceClicked);
    if (ReservedPalace)
        ReservedPalace->OnClicked.AddDynamic(this, &UFilterMenu::OnReservedPalaceClicked);
    if (SoldPalace)
        SoldPalace->OnClicked.AddDynamic(this, &UFilterMenu::OnSoldPalaceClicked);
}

// --- Main Button Handlers ---
void UFilterMenu::OnStandalonevillaButtonClicked()
{
    bStandaloneVillaMainFilterActive = !bStandaloneVillaMainFilterActive;
    SetSubFiltersEnabled(StandAloneSubFilters, bStandaloneVillaMainFilterActive);
    ApplyStatusFilters();
}

void UFilterMenu::OnOneStoryvillaButtonClicked()
{
    bOneStoryVillaMainFilterActive = !bOneStoryVillaMainFilterActive;
    SetSubFiltersEnabled(OneStorySubFilters, bOneStoryVillaMainFilterActive);
    ApplyStatusFilters();
}

void UFilterMenu::OnPalaceButtonClicked()
{
    bPalaceMainFilterActive = !bPalaceMainFilterActive;
    SetSubFiltersEnabled(PalaceSubFilters, bPalaceMainFilterActive);
    ApplyStatusFilters();
}

// Sub-filter button handlers
#define define_toggle_handler(FuncName, Flag) \
void FuncName() { Flag = !Flag; ApplyStatusFilters(); }

define_toggle_handler(UFilterMenu::OnAvailableStandAloneClicked, bShowAvailableStandAlone)
define_toggle_handler(UFilterMenu::OnReservedStandAloneClicked, bShowReservedStandAlone)
define_toggle_handler(UFilterMenu::OnSoldStandAloneClicked, bShowSoldStandAlone)
define_toggle_handler(UFilterMenu::OnAvailableOneStoryClicked, bShowAvailableOneStory)
define_toggle_handler(UFilterMenu::OnReservedOneStoryClicked, bShowReservedOneStory)
define_toggle_handler(UFilterMenu::OnSoldOneStoryClicked, bShowSoldOneStory)
define_toggle_handler(UFilterMenu::OnAvailablePalaceClicked, bShowAvailablePalace)
define_toggle_handler(UFilterMenu::OnReservedPalaceClicked, bShowReservedPalace)
define_toggle_handler(UFilterMenu::OnSoldPalaceClicked, bShowSoldPalace)

void UFilterMenu::ApplyStatusFilters()
{
    UWorld* World = GetWorld();
    if (!World) return;

    for (TActorIterator<ABulding> It(World); It; ++It)
    {
        ABulding* Building = *It;
        if (!Building || !Building->HighlightBox) continue;

        // Hide highlight by default
        Building->HighlightBox->SetVisibility(false);
        Building->HighlightBox->SetHiddenInGame(true);

        // Show highlight by status if type and sub-filter are active
        if (Building->BuildingType == EBuildingType::StandAloneVilla && bStandaloneVillaMainFilterActive)
        {
            if (Building->Status == EBuildingStatus::Available && bShowAvailableStandAlone)
                Building->ShowHighlightBox(FLinearColor::Green);
            else if (Building->Status == EBuildingStatus::Sold && bShowSoldStandAlone)
                Building->ShowHighlightBox(FLinearColor::Red);
            else if (Building->Status == EBuildingStatus::Reserved && bShowReservedStandAlone)
                Building->ShowHighlightBox(FLinearColor(1.0f, 1.0f, 0.2f));
        }
        else if (Building->BuildingType == EBuildingType::OneStoryVilla && bOneStoryVillaMainFilterActive)
        {
            if (Building->Status == EBuildingStatus::Available && bShowAvailableOneStory)
                Building->ShowHighlightBox(FLinearColor::Green);
            else if (Building->Status == EBuildingStatus::Sold && bShowSoldOneStory)
                Building->ShowHighlightBox(FLinearColor::Red);
            else if (Building->Status == EBuildingStatus::Reserved && bShowReservedOneStory)
                Building->ShowHighlightBox(FLinearColor(1.0f, 1.0f, 0.2f));
        }
        else if (Building->BuildingType == EBuildingType::Palace && bPalaceMainFilterActive)
        {
            if (Building->Status == EBuildingStatus::Available && bShowAvailablePalace)
                Building->ShowHighlightBox(FLinearColor::Green);
            else if (Building->Status == EBuildingStatus::Sold && bShowSoldPalace)
                Building->ShowHighlightBox(FLinearColor::Red);
            else if (Building->Status == EBuildingStatus::Reserved && bShowReservedPalace)
                Building->ShowHighlightBox(FLinearColor(1.0f, 1.0f, 0.2f));
        }
    }
}
