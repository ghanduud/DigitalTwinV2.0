// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.generated.h"
class UButton; 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTapChanged, EMenuTap, NewTap);

/**
 *
 */
UCLASS()
class DIGITALTWINV2_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintAssignable, Category = "Menu")
	FOnTapChanged OnTapChanged;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	class ATwinUiManager* UiManager;
	UPROPERTY(meta = (BindWidget))
	UButton* GolfGameButton;


protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* OverviewButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* FiltersButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* AtmosphereButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* ResetButton;



private:
	UFUNCTION()
	void OnOverviewClicked();

	UFUNCTION()
	void OnFiltersClicked();

	UFUNCTION()
	void OnAtmosphereClicked();

	UFUNCTION()
	void OnResetClicked();

};
