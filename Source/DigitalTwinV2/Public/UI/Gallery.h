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

	// Array to hold image paths for the gallery, can be set from C++ or Blueprint
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gallery")
	TArray<FString> GalleryImagesArray;
};
