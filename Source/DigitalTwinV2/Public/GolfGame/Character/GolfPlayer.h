// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GolfPlayer.generated.h"

UCLASS()
class DIGITALTWINV2_API AGolfPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGolfPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* WalkMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* BallAimLongMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* BallShootLongMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* BallAimChipMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* BallShootChipMontage;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MoveTo(const FVector& TargetLocation);
	UFUNCTION(BlueprintImplementableEvent, Category = "Animation")
	void Walkanim();
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayBallAimLongMontage();
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void StopBallAimLongMontage();
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayBallAimChipMontage();
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayBallShootLongMontage();


private:
	FVector MoveTarget;
	bool bIsMoving = false;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 300.0f; // Adjust as needed


};
