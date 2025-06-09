#include "UI/OverviewWidget.h"
#include "Components/Image.h"
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
        // Show the default image if no building is selected
        SetUnitImageFromPath(DefaultImagePath);
        return;
    }

    // Show building image if path is valid, otherwise fallback to default image
    if (!NewBuilding->ImagePath.IsEmpty())
    {
        SetUnitImageFromPath(NewBuilding->ImagePath);
    }
    else
    {
        SetUnitImageFromPath(DefaultImagePath);
    }
}

