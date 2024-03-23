// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CollectableKeyHolder.h"

// Sets default values
ACollectableKeyHolder::ACollectableKeyHolder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsKeyCollected = false;
	KeyRotationSpeed = 0.0f;
	bReplicates = true;
    AActor::SetReplicateMovement(true);

	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Comp"));
	SetRootComponent(rootComp);

	holderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Holder Mesh"));
	holderMesh->SetupAttachment(rootComp);
	holderMesh->SetIsReplicated(true);
	holderMesh->SetCollisionProfileName("BlockAllDynamic");
	keyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Key Mesh"));
	keyMesh->SetupAttachment(rootComp);
	keyMesh->SetIsReplicated(true);
	keyMesh->SetCollisionProfileName("OverlapAllDynamic");
}

void ACollectableKeyHolder::ActivateKeyMesh()
{
	bIsKeyCollected = true;
	keyMesh->SetVisibility(true);
}

// Called when the game starts or when spawned
void ACollectableKeyHolder::BeginPlay()
{
	Super::BeginPlay();

	keyMesh->SetVisibility(false);

}

// Called every frame
void ACollectableKeyHolder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsKeyCollected) return;
	keyMesh->AddRelativeRotation(FRotator(0.0f, KeyRotationSpeed * DeltaTime, 0.0f));
}

