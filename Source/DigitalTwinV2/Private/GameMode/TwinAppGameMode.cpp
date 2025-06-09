// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/TwinAppGameMode.h"
#include "Camera/BirdEye.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "UI/TwinUiManager.h"

void ATwinAppGameMode::BeginPlay()
{
    Super::BeginPlay();

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    BirdEyePawn = Cast<ABirdEye>(PlayerPawn);

    if (BirdEyePawn)
    {
        // Start intro sequence with max zoom and input disabled
        BirdEyePawn->SetCameraArmLength(40000.f);
        BirdEyePawn->SetInputEnabled(false);

        IntroSequenceElapsed = 0.f;
        GetWorldTimerManager().SetTimer(IntroSequenceTimerHandle, this, &ATwinAppGameMode::UpdateIntroSequence, 0.02f, true);
    }
}


void ATwinAppGameMode::StartIntroSequence()
{
    IntroSequenceElapsed = 0.f;

    // Set timer to update zoom every 0.02 seconds (50 FPS approx)
    GetWorldTimerManager().SetTimer(IntroSequenceTimerHandle, this, &ATwinAppGameMode::UpdateIntroSequence, 0.02f, true);
}

void ATwinAppGameMode::UpdateIntroSequence()
{
    if (!BirdEyePawn)
        return;

    IntroSequenceElapsed += 0.02f;

    float Alpha = FMath::Clamp(IntroSequenceElapsed / 5.0f, 0.f, 1.f); // Duration of 5 seconds

    // Ease In Out for smoother start and end
    float EasedAlpha = FMath::InterpEaseInOut(0.f, 1.f, Alpha, 3.0f);  // 3.0 is the exponent for smoothing

    float NewLength = FMath::Lerp(40000.f, 20000.f, EasedAlpha);
    BirdEyePawn->SetCameraArmLength(NewLength);

    if (Alpha >= 1.f)
    {
        BirdEyePawn->SetInputEnabled(true);
        GetWorldTimerManager().ClearTimer(IntroSequenceTimerHandle);

        // Notify UI manager that intro is finished
        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATwinUiManager::StaticClass(), FoundActors);

        if (FoundActors.Num() > 0)
        {
            ATwinUiManager* UiManager = Cast<ATwinUiManager>(FoundActors[0]);
            if (UiManager)
            {
                UiManager->OnIntroSequenceFinished();
            }
        }
    }
}



