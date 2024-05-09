// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactions/PushComponent.h"

// Sets default values for this component's properties
UPushComponent::UPushComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}


void UPushComponent::Teest()
{
	GEngine->AddOnScreenDebugMessage(323, 1.0f, FColor::Green, "void UPushComponent::Teest()");
}

// Called when the game starts
void UPushComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPushComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

