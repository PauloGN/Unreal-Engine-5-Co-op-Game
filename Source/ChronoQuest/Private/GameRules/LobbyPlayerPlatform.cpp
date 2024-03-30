// Fill out your copyright notice in the Description page of Project Settings.


#include "GameRules/LobbyPlayerPlatform.h"
#include "Components/ArrowComponent.h"

// Sets default values
ALobbyPlayerPlatform::ALobbyPlayerPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Comp"));
	SetRootComponent(rootComp);

	platform = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PLatform"));
	platform->SetupAttachment(rootComp);

	playerTransform = CreateDefaultSubobject<UArrowComponent>(TEXT("Player Transform"));
	playerTransform->SetupAttachment(rootComp);
}

// Called when the game starts or when spawned
void ALobbyPlayerPlatform::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALobbyPlayerPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

