// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GolfEndGameMenu.generated.h"

/**
 *
 */
UCLASS()
class DIGITALTWINV2_API UGolfEndGameMenu : public UUserWidget
{
	GENERATED_BODY()


public:
	virtual void NativeConstruct() override;

	// In CongratsMenu.h
	UPROPERTY(meta = (BindWidget))
	class UButton* RestartButton;



	UFUNCTION()
	void HandleEndGame();

};
