// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/CollectableKey.h"
#include <Components/CapsuleComponent.h>

#include "ChronoQuest/ChronoQuestCharacter.h"
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

	if(HasAuthority())
	{
		TArray<AActor*> overlapingActors;
		capsule->GetOverlappingActors(overlapingActors, AChronoQuestCharacter::StaticClass());

		if(overlapingActors.Num() > 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, "Pegou");
			if(!bIscollected)
			{
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
	Destroy();
}