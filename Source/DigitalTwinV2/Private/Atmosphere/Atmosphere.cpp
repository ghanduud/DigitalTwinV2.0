// Fill out your copyright notice in the Description page of Project Settings.



#include "Atmosphere/Atmosphere.h"
#include "Components/DirectionalLightComponent.h"
#include "UI/TwinUiManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "NiagaraComponent.h"

// Sets default values
AAtmosphere::AAtmosphere()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AAtmosphere::BeginPlay()
{
	Super::BeginPlay();

	TArray<UDirectionalLightComponent*> Components;
	GetComponents<UDirectionalLightComponent>(Components);

	for (UDirectionalLightComponent* LightComp : Components)
	{
		if (LightComp)
		{

			if (LightComp->ComponentHasTag(FName("SunLight")))
			{
				SunLight = LightComp;
			}
			else if (LightComp->ComponentHasTag(FName("MoonLight")))
			{
				MoonLight = LightComp;
			}
		}
	}

	TArray<UExponentialHeightFogComponent*> FogComponents;
	GetComponents<UExponentialHeightFogComponent>(FogComponents);

	for (UExponentialHeightFogComponent* FogComp : FogComponents)
	{
		if (FogComp)
		{
			if (FogComp->ComponentHasTag(FName("Fog")))
			{
				ExponentialFog = FogComp;
				break;
			}
		}
	}


	ATwinUiManager* UiManager = Cast<ATwinUiManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATwinUiManager::StaticClass()));

	if (UiManager)
	{
		UpdateLightRotation(UiManager->DayTime);
		UiManager->OnDayTimeChanged.AddDynamic(this, &AAtmosphere::HandleDayTimeChanged);

		// Bind to weather change event
		UiManager->OnWeatherChanged.AddDynamic(this, &AAtmosphere::HandleWeatherChanged);
	}

}


// void AAtmosphere::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);

// }



void AAtmosphere::UpdateLightRotation(float TimeOfDay)
{
	TimeOfDay = FMath::Fmod(TimeOfDay, 24.0f);

	float SunPitch = ((TimeOfDay - 6.0f) / 24.0f) * 360.0f + 180;

	float MoonPitch = FMath::Fmod(SunPitch + 180.0f, 360.0f);

	if (SunLight)
	{
		SunLight->SetWorldRotation(FRotator(SunPitch, 0.0f, 0.0f));
	}

	if (MoonLight)
	{
		MoonLight->SetWorldRotation(FRotator(MoonPitch, 0.0f, 0.0f));
	}
}





void AAtmosphere::HandleDayTimeChanged(float NewValue)
{
	UpdateLightRotation(NewValue);
}




void AAtmosphere::HandleWeatherChanged(EWeather NewWeather)
{
	switch (NewWeather)
	{
	case EWeather::Sunny:
		SetSunnyWeather();
		DestroyRainEffect();
		break;

	case EWeather::Rain:
		SpawnRainEffectAttachedToCamera();
		break;

	case EWeather::Fog:
		SetFogWeather();
		DestroyRainEffect();
		break;

	default:
		break;
	}
}



void AAtmosphere::SetFogWeather()
{
	if (ExponentialFog)
	{
		ExponentialFog->FogDensity = 0.05f;
		ExponentialFog->FogHeightFalloff = 0.001f;

		ExponentialFog->MarkRenderStateDirty();
	}
}

void AAtmosphere::SetSunnyWeather()
{
	if (ExponentialFog)
	{
		ExponentialFog->FogDensity = 0.02f;
		ExponentialFog->FogHeightFalloff = 0.2f;
		ExponentialFog->MarkRenderStateDirty();
	}
}





void AAtmosphere::SpawnRainEffectAttachedToCamera()
{
	if (!RainEffect) return;

	if (ActiveRainComponent) return;

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerController) return;

	APawn* PlayerPawn = PlayerController->GetPawn();
	if (!PlayerPawn) return;

	USpringArmComponent* SpringArm = PlayerPawn->FindComponentByClass<USpringArmComponent>();
	if (!SpringArm) return;

	UCameraComponent* CameraComp = nullptr;
	for (USceneComponent* Child : SpringArm->GetAttachChildren())
	{
		CameraComp = Cast<UCameraComponent>(Child);
		if (CameraComp)
		{
			break;
		}
	}

	ActiveRainComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
		RainEffect,
		CameraComp,
		NAME_None,
		FVector(0.f, 0.f, 0.f),
		FRotator::ZeroRotator,
		EAttachLocation::KeepRelativeOffset,
		false,
		true,
		ENCPoolMethod::None,
		true
	);

	if (ActiveRainComponent)
	{
		ActiveRainComponent->SetUsingAbsoluteRotation(true);
		ActiveRainComponent->SetRelativeRotation(FRotator::ZeroRotator);
	}
}


void AAtmosphere::DestroyRainEffect()
{
	if (ActiveRainComponent)
	{
		ActiveRainComponent->Deactivate();
		ActiveRainComponent->DestroyComponent();
		ActiveRainComponent = nullptr;
	}
}
