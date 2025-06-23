// Fill out your copyright notice in the Description page of Project Settings.


#include "GolfGame/UI/GolfStartMenu.h"
#include "GolfGame/GolfGameManager.h"
#include "Kismet/GameplayStatics.h"           // For UGameplayStatics::GetGameMode
#include "Components/Button.h"

void UGolfStartMenu::NativeConstruct()
{
    Super::NativeConstruct();

    if (StartBtn)
    {
        StartBtn->OnClicked.AddDynamic(this, &UGolfStartMenu::HandelStartGame);
    }

}


void UGolfStartMenu::HandelStartGame()
{
    if (AGolfGameManager* GM = AGolfGameManager::Get())
    {
        GM->StartGameSequence();  // Start the game
    }

    RemoveFromParent();  // Hide menu
}