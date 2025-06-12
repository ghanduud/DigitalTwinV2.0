// Fill out your copyright notice in the Description page of Project Settings.


#include "Buldings/Bulding.h"
#include "Buldings/StandAloneVilla.h"

AStandAloneVilla::AStandAloneVilla()
{
    BuildingType = EBuildingType::StandAloneVilla;
}

void AStandAloneVilla::BeginPlay()
{
    Super::BeginPlay();
    // Additional logic if needed
}

