// Fill out your copyright notice in the Description page of Project Settings.


#include "Buldings/Bulding.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"

// Sets default values
ABulding::ABulding()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneRoot;

	// Create Static Mesh Component and attach to root
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	// Create Box Collider and attach to root or mesh (your choice)
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	BoxCollider->SetupAttachment(RootComponent); // or Mesh, if you prefer
	BoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxCollider->SetCollisionObjectType(ECC_WorldStatic);
	BoxCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

}


// Called when the game starts or when spawned
void ABulding::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABulding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

