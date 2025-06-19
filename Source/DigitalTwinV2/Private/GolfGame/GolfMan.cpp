// // Fill out your copyright notice in the Description page of Project Settings.


// #include "GolfGame/GolfMan.h"

// // Sets default values
// AGolfMan::AGolfMan()
// {
//  	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
// 	PrimaryActorTick.bCanEverTick = true;

// }

// // Called when the game starts or when spawned
// void AGolfMan::BeginPlay()
// {
// 	Super::BeginPlay();
	
// }

// // Called every frame
// void AGolfMan::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);

// }

// // Called to bind functionality to input
// void AGolfMan::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
// {
// 	Super::SetupPlayerInputComponent(PlayerInputComponent);

// }

// void AGolfMan::PlayGolfSwingMontageToHold()
// {
//     if (GolfMontage && GetMesh())
//     {
//         if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
//         {
//             Anim->Montage_Play(GolfMontage);
//             Anim->Montage_SetPosition(GolfMontage, HoldFrameTime);
//             Anim->Montage_SetPlayRate(GolfMontage, 0.0f);
//         }
//     }
// }

// void AGolfMan::ResumeGolfSwingMontage()
// {
//     if (GolfMontage && GetMesh())
//     {
//         if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
//         {
//             Anim->Montage_SetPlayRate(GolfMontage, 1.0f);
//         }
//     }
// }
