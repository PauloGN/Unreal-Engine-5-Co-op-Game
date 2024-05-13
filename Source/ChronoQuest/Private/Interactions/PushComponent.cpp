// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactions/PushComponent.h"
#include <ChronoQuest/ChronoQuestCharacter.h>

#include "Actors/PushableObject.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Net/UnrealNetwork.h"

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

void UPushComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPushComponent, CurrentPushable);
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

void UPushComponent::PushingLogic(APushableObject* PushableObject)
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

void UPushComponent::EndPushingLogic()
{
	if (!CurrentPushable)
	{
		return;
	}

	//Get Component owner 
	AChronoQuestCharacter* myCharacter = Cast<AChronoQuestCharacter>(GetOwner());

	//Set Detach from 
	FDetachmentTransformRules DetachmentTransform(EDetachmentRule::KeepWorld, true);
	myCharacter->DetachFromActor(DetachmentTransform);

	//Constraint character movements
	myCharacter->GetMovementComponent()->SetPlaneConstraintEnabled(false);
	//myCharacter->GetMovementComponent()->SetPlaneConstraintNormal(myCharacter->GetActorRightVector());
	myCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;

	SetComponentTickEnabled(false);

	CurrentPushable = nullptr;
}

void UPushComponent::ServerRPCPushingLogicCall_Implementation(APushableObject* PushableObject)
{
	PushingLogic(PushableObject);
}

void UPushComponent::ServerRPCEndPushingLogicCall_Implementation()
{
	EndPushingLogic();
}

void UPushComponent::BeginPush(APushableObject* PushableObject)
{
	if(GetOwner()->HasAuthority())
	{
		PushingLogic(PushableObject);
		return;
	}

	ServerRPCPushingLogicCall(PushableObject);
}

void UPushComponent::EndPush()
{
	if (GetOwner()->HasAuthority())
	{
		EndPushingLogic();
		return;
	}

	ServerRPCEndPushingLogicCall();
}

bool UPushComponent::IsPushing()
{
	return static_cast<bool>(CurrentPushable);
}

