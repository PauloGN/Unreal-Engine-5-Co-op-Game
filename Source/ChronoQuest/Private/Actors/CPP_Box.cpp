// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CPP_Box.h"

// Sets default values
ACPP_Box::ACPP_Box()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACPP_Box::BeginPlay()
{
	Super::BeginPlay();


	
}

// Called every frame
void ACPP_Box::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("Server"));
	}else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Client"));
	}
}

