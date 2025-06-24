// Fill out your copyright notice in the Description page of Project Settings.


#include "GolfGame/Character/GolfPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
AGolfPlayer::AGolfPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


}

// Called when the game starts or when spawned
void AGolfPlayer::BeginPlay()
{
	Super::BeginPlay();

	// FVector ForwardDirection = GetActorForwardVector();
	// FVector StartLocation = GetActorLocation();
	// FVector TargetLocation = StartLocation + ForwardDirection * 2000.0f;

	// MoveTo(TargetLocation);
}


void AGolfPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsMoving)
	{
		FVector CurrentLocation = GetActorLocation();
		FVector Direction = (MoveTarget - CurrentLocation).GetSafeNormal();
		FVector NewLocation = CurrentLocation + Direction * MoveSpeed * DeltaTime;

		// Ensure walk montage is playing while moving
		if (WalkMontage)
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance && !AnimInstance->Montage_IsPlaying(WalkMontage))
			{
				AnimInstance->Montage_Play(WalkMontage);
			}
		}

		// Check if close enough
		if (FVector::Dist(NewLocation, MoveTarget) <= 5.0f)
		{
			SetActorLocation(MoveTarget);
			// Stop the walk montage instantly before setting bIsMoving to false
			if (WalkMontage)
			{
				UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
				if (AnimInstance && AnimInstance->Montage_IsPlaying(WalkMontage))
				{
					AnimInstance->Montage_Stop(0.0f, WalkMontage); // Instantly stop
				}
			}
			bIsMoving = false;
		}
		else
		{
			SetActorLocation(NewLocation);
		}
	}
}


// Called to bind functionality to input
void AGolfPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}



void AGolfPlayer::MoveTo(const FVector& TargetLocation)
{
	MoveTarget = TargetLocation;
	bIsMoving = true;

	// Play walk montage from C++ if assigned
	if (WalkMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && !AnimInstance->Montage_IsPlaying(WalkMontage))
		{
			AnimInstance->Montage_Play(WalkMontage);
		}
	}
}

