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
	if (Button_Exit)
	{
		Button_Exit->OnClicked.AddDynamic(this, &UGallery::OnExitButtonClicked);
	}
	if (ButtonImage1)
	{
		ButtonImage1->OnClicked.AddDynamic(this, &UGallery::OnGalleryImage1Clicked);
	}
	if (ButtonImage2)
	{
		ButtonImage2->OnClicked.AddDynamic(this, &UGallery::OnGalleryImage2Clicked);
	}
	if (ButtonImage3)
	{
		ButtonImage3->OnClicked.AddDynamic(this, &UGallery::OnGalleryImage3Clicked);
	}
	if (ButtonImage4)
	{
		ButtonImage4->OnClicked.AddDynamic(this, &UGallery::OnGalleryImage4Clicked);
	}
	if (ButtonImage5)
	{
		ButtonImage5->OnClicked.AddDynamic(this, &UGallery::OnGalleryImage5Clicked);
	}
	if (Button_Arrow_Right)
	{
		Button_Arrow_Right->OnClicked.AddDynamic(this, &UGallery::OnArrowRightClicked);
	}
	if (Button_Arrow_Left)
	{
		Button_Arrow_Left->OnClicked.AddDynamic(this, &UGallery::OnArrowLeftClicked);
	}
	if (GalleryImagesArray.Num() > 0)
	{
		SetGalleryImages(GalleryImagesArray);
	}
	// Set default large image to match GalleryImage1 after images are loaded
	if (GalleryImage1 && Image_Large)
	{
		Image_Large->SetBrush(GalleryImage1->GetBrush());
		CurrentLargeImageIndex = 0;
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

void UGallery::OnArrowRightClicked()
{
    // There are 5 images, wrap around
    CurrentLargeImageIndex = (CurrentLargeImageIndex + 1) % 5;
    ShowLargeImageByIndex(CurrentLargeImageIndex);
}

void UGallery::OnArrowLeftClicked()
{
    // Wrap around backwards
    CurrentLargeImageIndex = (CurrentLargeImageIndex + 4) % 5;
    ShowLargeImageByIndex(CurrentLargeImageIndex);
}

void UGallery::ShowLargeImageByIndex(int32 Index)
{
    UImage* Images[5] = { GalleryImage1, GalleryImage2, GalleryImage3, GalleryImage4, GalleryImage5 };
    if (Image_Large && Images[Index])
    {
        Image_Large->SetBrush(Images[Index]->GetBrush());
    }
}

void UGallery::OnGalleryImage1Clicked()
{
    if (GalleryImage1 && Image_Large)
    {
        CurrentLargeImageIndex = 0;
        Image_Large->SetBrush(GalleryImage1->GetBrush());
    }
}
void UGallery::OnGalleryImage2Clicked()
{
    if (GalleryImage2 && Image_Large)
    {
        CurrentLargeImageIndex = 1;
        Image_Large->SetBrush(GalleryImage2->GetBrush());
    }
}
void UGallery::OnGalleryImage3Clicked()
{
    if (GalleryImage3 && Image_Large)
    {
        CurrentLargeImageIndex = 2;
        Image_Large->SetBrush(GalleryImage3->GetBrush());
    }
}
void UGallery::OnGalleryImage4Clicked()
{
    if (GalleryImage4 && Image_Large)
    {
        CurrentLargeImageIndex = 3;
        Image_Large->SetBrush(GalleryImage4->GetBrush());
    }
}
void UGallery::OnGalleryImage5Clicked()
{
    if (GalleryImage5 && Image_Large)
    {
        CurrentLargeImageIndex = 4;
        Image_Large->SetBrush(GalleryImage5->GetBrush());
    }
}

