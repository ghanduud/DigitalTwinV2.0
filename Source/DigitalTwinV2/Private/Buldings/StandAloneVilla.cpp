// Fill out your copyright notice in the Description page of Project Settings.


#include "Buldings/StandAloneVilla.h"

AStandAloneVilla::AStandAloneVilla()
{
    BuildingType = EBuildingType::StandAloneVilla;
    FootPrintArea = 128.f;
    BuldingUnitArea = 255.f;
    GroundFloorArea = 93.f;
    FirstFloorArea = 121.f;
    RoofFloorArea = 41.f;
    Status = EBuildingStatus::Available;
    Discount = 10.f;
    Price = 1500000.f;
    DisplayName = TEXT("Stand Alone Villa");
    NumberOfFloors = TEXT("3");
}

void AStandAloneVilla::BeginPlay()
{
    Super::BeginPlay();
    // Additional logic if needed
}

