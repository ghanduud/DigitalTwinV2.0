// Fill out your copyright notice in the Description page of Project Settings.


#include "Buldings/GolfClubBuilding.h"


AGolfClubBuilding::AGolfClubBuilding()
{
    BuildingType = EBuildingType::GolfClub;
}

void AGolfClubBuilding::BeginPlay()
{
    Super::BeginPlay();
    // Additional logic if needed
}