#include "UI/OverviewWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Buldings/Bulding.h"

UOverviewWidget::UOverviewWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Set a default image asset path (can be overridden in Blueprint)
    DefaultImagePath = TEXT("/Game/TwinApp/Buldings/Images/island.island"); // Note the `.island` at the end
}

void UOverviewWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Load the default image when the widget is constructed
    SetUnitImageFromPath(DefaultImagePath);
}

void UOverviewWidget::SetUnitImageFromPath(const FString& AssetPath)
{
    FSoftObjectPath TexturePath(AssetPath);
    FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

    Streamable.RequestAsyncLoad(TexturePath, [this, TexturePath]()
                                {
                                    UObject* LoadedAsset = TexturePath.ResolveObject();

                                    // Fallback: Try loading directly if not already loaded
                                    if (!LoadedAsset)
                                    {
                                        LoadedAsset = TexturePath.TryLoad();
                                    }

                                    if (UTexture2D* LoadedTexture = Cast<UTexture2D>(LoadedAsset))
                                    {
                                        if (UnitImage)
                                        {
                                            FSlateBrush Brush;
                                            Brush.SetResourceObject(LoadedTexture);
                                            Brush.ImageSize = FVector2D(LoadedTexture->GetSizeX(), LoadedTexture->GetSizeY());
                                            UnitImage->SetBrush(Brush);
                                        }
                                    }
                                });
}

void UOverviewWidget::SetBuilding(ABulding* NewBuilding)
{
    if (!NewBuilding)
    {
        SetUnitImageFromPath(DefaultImagePath);
        return;
    }

     // Update image
    SetUnitImageFromPath(NewBuilding->ImagePath.IsEmpty() ? DefaultImagePath : NewBuilding->ImagePath);

    // Update display name
    if (TextBlock_Title)
    {
        TextBlock_Title->SetText(FText::FromString(NewBuilding->DisplayName));
    }

    if (TextBlock_FootprintArea)
        TextBlock_FootprintArea->SetText(NewBuilding->FootPrintArea == 0.f ? FText::FromString(TEXT("N/A")) : FText::AsNumber(NewBuilding->FootPrintArea));

    if (TextBlock_UnitArea)
        TextBlock_UnitArea->SetText(NewBuilding->BuldingUnitArea == 0.f ? FText::FromString(TEXT("N/A")) : FText::AsNumber(NewBuilding->BuldingUnitArea));

    if (TextBlock_GroundFloor)
        TextBlock_GroundFloor->SetText(NewBuilding->GroundFloorArea == 0.f ? FText::FromString(TEXT("N/A")) : FText::AsNumber(NewBuilding->GroundFloorArea));

    if (TextBlock_FirstFloor)
        TextBlock_FirstFloor->SetText(NewBuilding->FirstFloorArea == 0.f ? FText::FromString(TEXT("N/A")) : FText::AsNumber(NewBuilding->FirstFloorArea));

    if (TextBlock_RoofFloor)
        TextBlock_RoofFloor->SetText(NewBuilding->RoofFloorArea == 0.f ? FText::FromString(TEXT("N/A")) : FText::AsNumber(NewBuilding->RoofFloorArea));

    if (TextBlock_Price)
    {
        FString PriceStr = FString::Printf(TEXT("EGP %.0f"), NewBuilding->Price);
        TextBlock_Price->SetText(FText::FromString(PriceStr));
    }

    if (TextBlock_Discount)
    {
        if (NewBuilding->Discount == 0.f)
            TextBlock_Discount->SetText(FText::FromString(TEXT("N/A")));
        else
            TextBlock_Discount->SetText(FText::Format(NSLOCTEXT("Overview", "DiscountFormat", "{0}%"), FText::AsNumber(NewBuilding->Discount)));
    }

    if (TextBlock_Availability)
        TextBlock_Availability->SetText(FText::FromString(NewBuilding->GetStatusAsString()));

    if (NumberOfFloors)
        NumberOfFloors->SetText(FText::FromString(NewBuilding->NumberOfFloors));
}

