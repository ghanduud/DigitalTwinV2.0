// Fill out your copyright notice in the Description page of Project Settings.


#include "Buldings/Palace.h"

APalace::APalace()
{
    BuildingType = EBuildingType::Palace;
    FootPrintArea = 600.f;
    BuldingUnitArea = 1547.f;
    GroundFloorArea = 498.f;
    FirstFloorArea = 529.f;
    RoofFloorArea = 204.f;
    Status = EBuildingStatus::Sold;
    Discount = 0.f;
    Price = 5000000.f;
    DisplayName = TEXT("Palace");
    NumberOfFloors = TEXT("3");
}

void APalace::BeginPlay()
{
    Super::BeginPlay();
    // Additional logic if needed
}

