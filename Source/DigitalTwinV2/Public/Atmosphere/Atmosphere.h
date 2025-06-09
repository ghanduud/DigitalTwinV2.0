// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Atmosphere.generated.h"


UCLASS()
class DIGITALTWINV2_API AAtmosphere : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAtmosphere();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	// virtual void Tick(float DeltaTime) override;



public:

	UPROPERTY(EditAnywhere, Category = "Effects")
	class UNiagaraSystem* RainEffect;

	UPROPERTY()
	class UNiagaraComponent* ActiveRainComponent = nullptr;


	UPROPERTY()
	class UDirectionalLightComponent* SunLight = nullptr;

	UPROPERTY()
	class UDirectionalLightComponent* MoonLight = nullptr;

	UPROPERTY()
	class UExponentialHeightFogComponent* ExponentialFog = nullptr;

	UFUNCTION()
	void HandleDayTimeChanged(float NewValue);

	UFUNCTION()
	void UpdateLightRotation(float DayTimeValue);

	UFUNCTION()
	void HandleWeatherChanged(EWeather NewWeather);

	UFUNCTION(BlueprintCallable, Category = "Weather")
	void SetFogWeather();

	UFUNCTION(BlueprintCallable, Category = "Weather")
	void SetSunnyWeather();

	UFUNCTION(BlueprintCallable, Category = "Weather")
	void DestroyRainEffect();

	UFUNCTION(BlueprintCallable, Category = "Weather")
	void SpawnRainEffectAttachedToCamera();


};
