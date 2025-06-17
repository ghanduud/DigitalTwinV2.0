// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TwinUiManager.generated.h"

UENUM(BlueprintType)
enum class EMenuTap : uint8
{
	Overview    UMETA(DisplayName = "Overview"),
	Filters     UMETA(DisplayName = "Filters"),
	Atmosphere  UMETA(DisplayName = "Atmosphere"),
};

UENUM(BlueprintType)
enum class EWeather : uint8
{
	Sunny    UMETA(DisplayName = "Sunny"),
	Rain     UMETA(DisplayName = "Rain"),
	Fog  	 UMETA(DisplayName = "Fog"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDayTimeChanged, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeatherChangedUI, EWeather, NewWeather);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentBuildingChanged, ABulding*, NewBuilding);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResetRequested);



UCLASS()
class DIGITALTWINV2_API ATwinUiManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATwinUiManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// public:	
	// 	// Called every frame
	// virtual void Tick(float DeltaTime) override;



public:

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UDaynNightSlider> DayNightSliderClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UMenuWidget> MenuWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UOverviewWidget> OverviewWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UFilterMenu> FilterWidgetClass;





	// Actual created widgets
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	class UDaynNightSlider* DayAndNightSlider;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	class UOverviewWidget* WOverview;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	class UFilterMenu* WFilter;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	class UMenuWidget* Menu;





	//Menu UI
	UPROPERTY(BlueprintReadOnly, Category = "Menu")
	EMenuTap CurrentTap = EMenuTap::Overview;

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void SetCurrentTap(EMenuTap NewTap);

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void UpdateUIVisibility();


	/** Currently selected building */
	UPROPERTY(BlueprintReadOnly, Category = "Building", meta = (AllowPrivateAccess = "true"))
	ABulding* CurrentBuilding = nullptr;

	/** Sets the current building */
	UPROPERTY(BlueprintAssignable, Category = "Building")
	FOnCurrentBuildingChanged OnCurrentBuildingChanged;


	UPROPERTY(BlueprintAssignable)
	FOnResetRequested OnResetRequested;

	/** Sets the current building */
	UFUNCTION(BlueprintCallable, Category = "Building")
	void SetCurrentBuilding(ABulding* NewBuilding);

	/** Clears the current building */
	UFUNCTION(BlueprintCallable, Category = "Building")
	void ClearCurrentBuilding();


	//Atmosphere UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere")
	float DayTime = 12;

	UFUNCTION(BlueprintCallable, Category = "Atmosphere")
	void UpdateDayTimeFromSlider(float NewSliderValue);

	UPROPERTY(BlueprintAssignable, Category = "Atmosphere")
	FOnDayTimeChanged OnDayTimeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Weather")
	FOnWeatherChangedUI OnWeatherChanged;

	UPROPERTY(BlueprintReadOnly, Category = "Atmosphere")
	EWeather CurrentWeather = EWeather::Sunny;

	UFUNCTION(BlueprintCallable, Category = "Atmosphere")
	void SetCurrentWeather(EWeather NewWeather);


	UFUNCTION()
	void ResetCameraAndBuilding();

	UFUNCTION()
	void OnIntroSequenceFinished();

	UFUNCTION()
	void OnGolfGameButtonClicked();

};


