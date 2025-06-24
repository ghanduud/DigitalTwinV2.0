// Fill out your copyright notice in the Description page of Project Settings.


#include "GolfGame/Inputs/GolfController.h"



#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GolfGame/GolfGameManager.h" // Make sure this is included

void AGolfController::BeginPlay()
{
    Super::BeginPlay();

    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(InputMappingContext, 0);
    }
}

void AGolfController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInput->BindAction(IA_ScreenPress, ETriggerEvent::Started, this, &AGolfController::HandleScreenPress);
        EnhancedInput->BindAction(IA_ScreenPress, ETriggerEvent::Completed, this, &AGolfController::HandleScreenRelease);

        EnhancedInput->BindAction(IA_ScreenMove, ETriggerEvent::Triggered, this, &AGolfController::HandleScreenMove);
    }
}



void AGolfController::HandleScreenPress(const FInputActionValue& Value)
{
    UE_LOG(LogTemp, Log, TEXT("Started adjusting shot."));

    if (AGolfGameManager* GM = AGolfGameManager::Get())
    {
        GM->BeginAdjustShot();
    }
}

void AGolfController::HandleScreenRelease(const FInputActionValue& Value)
{
    if (AGolfGameManager* GM = AGolfGameManager::Get())
    {
        GM->Shoot();
    }
}

void AGolfController::HandleScreenMove(const FInputActionValue& Value)
{
    if (AGolfGameManager* GM = AGolfGameManager::Get())
    {
        FVector2D Delta = Value.Get<FVector2D>();
        GM->AdjustShot(Delta);
    }
}
