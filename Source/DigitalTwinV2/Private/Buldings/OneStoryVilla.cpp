// Fill out your copyright notice in the Description page of Project Settings.


#include "Buldings/OneStoryVilla.h"

AOneStoryVilla::AOneStoryVilla()
{
    BuildingType = EBuildingType::OneStoryVilla;
    FootPrintArea = 228.f;
    BuldingUnitArea = 228.f;
    GroundFloorArea = 228.f;
    FirstFloorArea = 0.f;
    RoofFloorArea = 0.f;
    Status = EBuildingStatus::Available;
    Discount = 5.f;
    Price = 1000000.f;
    DisplayName = TEXT("One Story Villa");
    NumberOfFloors = TEXT("1");
    
}

void AOneStoryVilla::BeginPlay()
{
    Super::BeginPlay();
    // Additional logic if needed
}

