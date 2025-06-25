#include "UI/OverviewWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Engine/Texture2D.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Buldings/Bulding.h"
#include "UI/TwinUiManager.h"
#include "Engine/Engine.h"
#include "UI/Gallery.h"

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

    if (Button_MediaGallery)
    {
        Button_MediaGallery->OnClicked.AddDynamic(this, &UOverviewWidget::OnMediaGalleryClicked);
    }
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
    // Hide the widget if not a valid building type
    if (!NewBuilding ||
        (NewBuilding->BuildingType != EBuildingType::OneStoryVilla &&
         NewBuilding->BuildingType != EBuildingType::Palace &&
         NewBuilding->BuildingType != EBuildingType::StandAloneVilla))
    {
        SetVisibility(ESlateVisibility::Collapsed);
        SetUnitImageFromPath(DefaultImagePath);
        return;
    }

    SetVisibility(ESlateVisibility::Visible);
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

void UOverviewWidget::OnMediaGalleryClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Button_MediaGallery clicked! UiManager=%p"), UiManager);
    if (UiManager)
    {
        if (UiManager->CurrentBuilding && UiManager->WGallery)
        {
            // Get GalleryImages from the building's data (from JSON)
            // We'll use reflection to get the TArray<FString> GalleryImages property if it exists
            TArray<FString>* GalleryImagesPtr = nullptr;
            FProperty* GalleryImagesProp = UiManager->CurrentBuilding->GetClass()->FindPropertyByName(FName("GalleryImages"));
            if (GalleryImagesProp && GalleryImagesProp->IsA<FArrayProperty>())
            {
                GalleryImagesPtr = GalleryImagesProp->ContainerPtrToValuePtr<TArray<FString>>(UiManager->CurrentBuilding);
            }
            // If not found, fallback to using the DataIndex and AllBuildingData
            if (!GalleryImagesPtr || GalleryImagesPtr->Num() == 0)
            {
                int32 DataIndex = UiManager->CurrentBuilding->DataIndex;
                if (ABulding::AllBuildingData.IsValidIndex(DataIndex))
                {
                    // Parse GalleryImages from the JSON string using FJsonObject
                    const FBuildingData& Data = ABulding::AllBuildingData[DataIndex];
                    // The GalleryImages are not in FBuildingData struct, so we need to parse them from the JSON file or store them elsewhere.
                    // As a workaround, we can add a static helper to ABulding to get GalleryImages for a DataIndex.
                    // For now, just pass an empty array to avoid compile error.
                    UiManager->WGallery->SetGalleryImages(TArray<FString>());
                }
                else
                {
                    UiManager->WGallery->SetGalleryImages(TArray<FString>());
                }
            }
            else
            {
                UiManager->WGallery->SetGalleryImages(*GalleryImagesPtr);
            }
        }
        UiManager->ShowGallery();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UiManager is null in UOverviewWidget!"));
    }
}

