// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GolfController.generated.h"


struct FInputActionValue;
class UInputMappingContext;
class UInputAction;

/**
 *
 */
UCLASS()
class DIGITALTWINV2_API AGolfController : public APlayerController
{
	GENERATED_BODY()


protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	void HandleScreenPress(const FInputActionValue& Value);
	void HandleScreenRelease(const FInputActionValue& Value);
	void HandleScreenMove(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_ScreenPress;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_ScreenMove;
};
