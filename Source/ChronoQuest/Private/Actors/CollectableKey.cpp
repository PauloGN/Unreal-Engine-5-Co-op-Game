// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/CollectableKey.h"
#include <Components/CapsuleComponent.h>

#include "CollectableKeyHolder.h"
#include "ChronoQuest/ChronoQuestCharacter.h"
#include "Components/AudioComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACollectableKey::ACollectableKey()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
    AActor::SetReplicateMovement(true);

	capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Comp"));
	SetRootComponent(capsule);
	capsule->SetIsReplicated(true);
	capsule->SetCollisionProfileName(FName("OverlapAllDynamic"));
	capsule->SetCapsuleRadius(100.0f);

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	mesh->SetupAttachment(capsule);
	mesh->SetIsReplicated(true);
	mesh->SetCollisionProfileName(FName("OverlapAllDynamic"));

	collectedCue = CreateDefaultSubobject<UAudioComponent>(TEXT("Collected Audio"));
	collectedCue->SetupAttachment(capsule);
	collectedCue->SetAutoActivate(false);
	RotationSpeed = 0.0f;

	bIscollected = false;
}

void ACollectableKey::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACollectableKey, bIscollected);
}

void ACollectableKey::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACollectableKey::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIscollected) return;

	if(HasAuthority())
	{
		if(RotationSpeed != 0.0f)
		{
			mesh->AddRelativeRotation(FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));
		}

		TArray<AActor*> overlapingActors;
		capsule->GetOverlappingActors(overlapingActors, AChronoQuestCharacter::StaticClass());

		if(overlapingActors.Num() > 0)
		{
			if(!bIscollected)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, "Pegou");
				bIscollected = true;
				//calling from the server
				OnRep_bIscollected();
			}
		}
	}
}

//this is going to be automatically called from the client but not from server
void ACollectableKey::OnRep_bIscollected()
{

	if(HasAuthority())
	{
		if(bIscollected)
		{
			OnCollected.Broadcast();
		}
	}

	collectedCue->Play();
	mesh->SetVisibility(false);

	if(keyHolder && bIscollected)
	{
		keyHolder->ActivateKeyMesh();
	}
	//Destroy();
}