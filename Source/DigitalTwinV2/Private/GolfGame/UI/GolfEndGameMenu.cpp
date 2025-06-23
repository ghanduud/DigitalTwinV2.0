// Fill out your copyright notice in the Description page of Project Settings.


#include "GolfGame/UI/GolfEndGameMenu.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"

void UGolfEndGameMenu::NativeConstruct()
{
    if (RestartButton)
    {
        RestartButton->OnClicked.AddDynamic(this, &UGolfEndGameMenu::HandleEndGame);
    }
}



void UGolfEndGameMenu::HandleEndGame()
{
    // Reload the current level
    UWorld* World = GetWorld();
    if (World)
    {
        FName CurrentLevel = *World->GetName();
        UGameplayStatics::OpenLevel(this, CurrentLevel, false);
    }
}