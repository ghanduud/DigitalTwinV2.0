// Fill out your copyright notice in the Description page of Project Settings.


#include "GolfGame/UI/GolfGameUI.h"
#include "GameMode/GolfGameGameMode.h"
#include "Kismet/GameplayStatics.h"           // For UGameplayStatics::GetGameMode
#include "Components/Button.h"  

void UGolfGameUI::NativeConstruct()
{
    Super::NativeConstruct();

    if (LongShotBtn)
    {
        LongShotBtn->OnClicked.AddDynamic(this, &UGolfGameUI::HandleLongShotClicked);
    }

    if (ShortShotBtn)
    {
        ShortShotBtn->OnClicked.AddDynamic(this, &UGolfGameUI::HandleShortShotClicked);
    }

    if (ChipShotBtn)
    {
        ChipShotBtn->OnClicked.AddDynamic(this, &UGolfGameUI::HandleChipShotClicked);
    }
}



void UGolfGameUI::HandleLongShotClicked()
{
    if (AGolfGameGameMode* GM = Cast<AGolfGameGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
    {
        GM->SetShotTypeToLong();
    }
}

void UGolfGameUI::HandleShortShotClicked()
{
    if (AGolfGameGameMode* GM = Cast<AGolfGameGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
    {
        GM->SetShotTypeToShort();
    }
}

void UGolfGameUI::HandleChipShotClicked()
{
    if (AGolfGameGameMode* GM = Cast<AGolfGameGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
    {
        GM->SetShotTypeToChip();
    }
}