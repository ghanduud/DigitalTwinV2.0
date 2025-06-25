// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gallery.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Components/Button.h"
#include "UI/TwinUiManager.h"

// Explicitly include the header for the UiManager property
#include "n:\UE5_courseGame\DigitalTwinV2.0\Source\DigitalTwinV2\Public\UI\Gallery.h"

UGallery::UGallery(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Constructor logic (if needed)
}

void UGallery::NativeConstruct()
{
	Super::NativeConstruct();
	// Widget setup logic (if needed)
	// Example: Call SetGalleryImages with a test array or previously set array
	// You should set GalleryImagesArray before this call, or override this in Blueprint to provide the images.
	if (GalleryImagesArray.Num() > 0)
	{
		SetGalleryImages(GalleryImagesArray);
	}
}

void UGallery::SetGalleryImages(const TArray<FString>& ImagePaths)
{
    TArray<UImage*> ImageWidgets = { GalleryImage1, GalleryImage2, GalleryImage3, GalleryImage4, GalleryImage5 };
    for (int32 i = 0; i < ImageWidgets.Num(); ++i)
    {
        if (ImageWidgets[i])
        {
            if (ImagePaths.IsValidIndex(i) && !ImagePaths[i].IsEmpty())
            {
                UE_LOG(LogTemp, Warning, TEXT("Gallery: Loading image %d: %s"), i + 1, *ImagePaths[i]);
                FSoftObjectPath TexturePath(ImagePaths[i]);
                FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
                Streamable.RequestAsyncLoad(TexturePath, [ImageWidget = ImageWidgets[i], TexturePath, i]()
                {
                    UObject* LoadedAsset = TexturePath.ResolveObject();
                    if (!LoadedAsset)
                    {
                        LoadedAsset = TexturePath.TryLoad();
                    }
                    if (UTexture2D* LoadedTexture = Cast<UTexture2D>(LoadedAsset))
                    {
                        FSlateBrush Brush;
                        Brush.SetResourceObject(LoadedTexture);
                        Brush.ImageSize = FVector2D(LoadedTexture->GetSizeX(), LoadedTexture->GetSizeY());
                        ImageWidget->SetBrush(Brush);
                        UE_LOG(LogTemp, Warning, TEXT("Gallery: Image %d loaded and set!"), i + 1);
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("Gallery: Failed to load texture for image %d: %s"), i + 1, *TexturePath.ToString());
                    }
                });
            }
            else
            {
                ImageWidgets[i]->SetBrush(FSlateBrush()); // Clear if not enough images
                UE_LOG(LogTemp, Warning, TEXT("Gallery: Image %d is empty or missing!"), i + 1);
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Gallery: ImageWidget %d is null!"), i + 1);
        }
    }
}

void UGallery::OnExitButtonClicked()
{
    // Use UiManager property directly from the header
    if (this->UiManager)
    {
        SetVisibility(ESlateVisibility::Collapsed);
        this->UiManager->ShowOverview();
    }
    else
    {
        SetVisibility(ESlateVisibility::Collapsed);
    }
}

