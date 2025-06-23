// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GolfStartMenu.generated.h"

/**
 *
 */
UCLASS()
class DIGITALTWINV2_API UGolfStartMenu : public UUserWidget
{
	GENERATED_BODY()



public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UButton* StartBtn;



	UFUNCTION()
	void HandelStartGame();
};
