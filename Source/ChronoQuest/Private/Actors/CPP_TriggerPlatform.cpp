// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/CPP_TriggerPlatform.h"
#include "CPP_Transporter.h"

// Sets default values
ACPP_TriggerPlatform::ACPP_TriggerPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
    AActor::SetReplicateMovement(true);

	activated = false;

	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(rootComp);
	triggerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TriggerMesh"));
	triggerMesh->SetupAttachment(rootComp);
	triggerMesh->SetIsReplicated(true);

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	mesh->SetupAttachment(rootComp);
	mesh->SetIsReplicated(true);
	//self move settings
	transporter = CreateDefaultSubobject<UCPP_Transporter>(TEXT("Transporter"));
	transporter->moveTime = 0.15f;
}

// Called when the game starts or when spawned
void ACPP_TriggerPlatform::BeginPlay()
{
	Super::BeginPlay();

	SetReplicates(true);
	SetReplicateMovement(true);

	triggerMesh->SetVisibility(false);
	triggerMesh->SetCollisionProfileName(FName("OverlapAll"));
	//self move settings
	FVector endPoint = GetActorLocation() + FVector(0.0f, 0.0f, -10.0f);
	transporter->SetPoints(GetActorLocation(), endPoint);
}

// Called every frame
void ACPP_TriggerPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(HasAuthority())
	{
		TArray<AActor*> overlapingActors;
		AActor* triggerActor = nullptr;
		triggerMesh->GetOverlappingActors(overlapingActors);

		for (int i = 0; i < overlapingActors.Num(); ++i)
		{
			AActor* actor = overlapingActors[i];
			if(actor != nullptr && actor->ActorHasTag("TriggerActor"))
			{
				triggerActor = actor;
				break;
			}
		}

		//Check if we got a valid actor pointer

		if(triggerActor)
		{
			if(!activated)
			{
				activated = true;
				//Fire delegate
				OnTriggerActivated.Broadcast();
				//Self move activation
				transporter->bAllTriggerActorsTriggered = true;
				//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("Bao"));
			}
		}else
		{
			if (activated)
			{
				activated = false;
				//Fire delegate
				OnTriggerDeactivated.Broadcast();
				//self move activation
				transporter->bAllTriggerActorsTriggered = false;
				//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Ruim"));
			}
		}
	}
}