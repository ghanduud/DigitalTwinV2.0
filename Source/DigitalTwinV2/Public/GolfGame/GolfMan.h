// // Fill out your copyright notice in the Description page of Project Settings.

// #pragma once

// #include "CoreMinimal.h"
// #include "GameFramework/Character.h"
// #include "GolfMan.generated.h"

// UCLASS()
// class DIGITALTWINV2_API AGolfMan : public ACharacter
// {
// 	GENERATED_BODY()

// public:
// 	// Sets default values for this character's properties
// 	AGolfMan();

//     UFUNCTION(BlueprintCallable)
//     void PlayGolfSwingMontageToHold();

//     UFUNCTION(BlueprintCallable)
//     void ResumeGolfSwingMontage();
// protected:
// 	// Called when the game starts or when spawned
// 	virtual void BeginPlay() override;

// public:	
// 	// Called every frame
// 	virtual void Tick(float DeltaTime) override;

// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Golf")
//     UAnimMontage* GolfMontage;

//     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Golf")
//     float HoldFrameTime = 0.5f; // Around frame 15
// };
