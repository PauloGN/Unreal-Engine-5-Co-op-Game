// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactions/PushComponent.h"
#include <ChronoQuest/ChronoQuestCharacter.h>

#include "Actors/PushableObject.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"

// Sets default values for this component's properties
UPushComponent::UPushComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

// Called when the game starts
void UPushComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	SetComponentTickEnabled(false);
}

// Called every frame
void UPushComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	GEngine->AddOnScreenDebugMessage(13333, 1.f, FColor::Blue, "Go");
	// ...

	if (CurrentPushable)
	{
		const FVector ForwardVector = GetOwner()->GetActorForwardVector();

		FVector NewLocation = ForwardVector * DeltaTime * PushSpeed;

		CurrentPushable->AddActorWorldOffset(NewLocation, true);

	}

}

void UPushComponent::BeginPush(APushableObject* PushableObject)
{

	if(CurrentPushable)
	{
		return;
	}

	//Set Current Pushable object being used
	AChronoQuestCharacter* myCharacter = Cast<AChronoQuestCharacter>(GetOwner());
	CurrentPushable = PushableObject;

	//Set attachment 
	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepWorld, true);
	AttachmentTransformRules.bWeldSimulatedBodies = true;
	myCharacter->AttachToActor(CurrentPushable, AttachmentTransformRules);

	//Constraint character movements
	myCharacter->GetMovementComponent()->SetPlaneConstraintEnabled(true);
	myCharacter->GetMovementComponent()->SetPlaneConstraintNormal(myCharacter->GetActorRightVector());
	myCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;

	SetComponentTickEnabled(true);

}

void UPushComponent::EndPush()
{
}

