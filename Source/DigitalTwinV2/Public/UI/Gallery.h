// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gallery.generated.h"

/**
 * 
 */
UCLASS()
class DIGITALTWINV2_API UGallery : public UUserWidget
{
	GENERATED_BODY()

public:
	UGallery(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Gallery")
	void SetGalleryImages(const TArray<FString>& ImagePaths);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UImage* GalleryImage1;
	UPROPERTY(meta = (BindWidget))
	class UImage* GalleryImage2;
	UPROPERTY(meta = (BindWidget))
	class UImage* GalleryImage3;
	UPROPERTY(meta = (BindWidget))
	class UImage* GalleryImage4;
	UPROPERTY(meta = (BindWidget))
	class UImage* GalleryImage5;
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Exit;

	// Array to hold image paths for the gallery, can be set from C++ or Blueprint
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gallery")
	TArray<FString> GalleryImagesArray;

	// Add this property to allow access to the UI manager from the gallery
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gallery")
	class ATwinUiManager* UiManager;

	UFUNCTION()
	void OnExitButtonClicked();

	// Add these to the protected section:
	UPROPERTY(meta = (BindWidget))
	class UButton* ButtonImage1;
	UPROPERTY(meta = (BindWidget))
	class UButton* ButtonImage2;
	UPROPERTY(meta = (BindWidget))
	class UButton* ButtonImage3;
	UPROPERTY(meta = (BindWidget))
	class UButton* ButtonImage4;
	UPROPERTY(meta = (BindWidget))
	class UButton* ButtonImage5;
	UPROPERTY(meta = (BindWidget))
	class UImage* Image_Large;

	UFUNCTION()
	void OnGalleryImage1Clicked();
	UFUNCTION()
	void OnGalleryImage2Clicked();
	UFUNCTION()
	void OnGalleryImage3Clicked();
	UFUNCTION()
	void OnGalleryImage4Clicked();
	UFUNCTION()
	void OnGalleryImage5Clicked();

	// Add these to the protected section:
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Arrow_Right;
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Arrow_Left;

	// Track the current index of the large image
	int32 CurrentLargeImageIndex = 0;

	UFUNCTION()
	void OnArrowRightClicked();
	UFUNCTION()
	void OnArrowLeftClicked();
};
