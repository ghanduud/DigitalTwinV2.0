#pragma once

#include "CoreMinimal.h"
#include "HoleData.generated.h"

USTRUCT(BlueprintType)
struct FHoleData
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 HoleNumber;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* StartPoint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* HoleTarget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* HoleBounds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Par;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    int32 ShotsPlayed = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    int32 Score = 0;



    // Default constructor
    FHoleData()
        : HoleNumber(0), StartPoint(nullptr), HoleTarget(nullptr), HoleBounds(nullptr), Par(3), ShotsPlayed(0), Score(0)
    {
    }

    // Custom constructor with optional values
    FHoleData(int32 InHoleNumber, AActor* InStart = nullptr, AActor* InTarget = nullptr, AActor* InBounds = nullptr, int32 InPar = 3)
        : HoleNumber(InHoleNumber), StartPoint(InStart), HoleTarget(InTarget), HoleBounds(InBounds), Par(InPar), ShotsPlayed(0), Score(0)
    {
    }
};

