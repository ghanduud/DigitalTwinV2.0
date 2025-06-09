// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TwinAppGameMode.generated.h"

/**
 *
 */
UCLASS()
class DIGITALTWINV2_API ATwinAppGameMode : public AGameModeBase
{
	GENERATED_BODY()


protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	class ABirdEye* BirdEyePawn;

	void StartIntroSequence();

	// To handle smooth zoom timer
	FTimerHandle IntroSequenceTimerHandle;

	// For interpolation over time
	float IntroSequenceDuration = 3.f;
	float IntroSequenceElapsed = 0.f;

	void UpdateIntroSequence();

};
