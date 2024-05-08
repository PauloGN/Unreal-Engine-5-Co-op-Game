// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PushableActor.h"

#include "ChronoQuest/ChronoQuestCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

// Sets default values
APushableActor::APushableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	NetUpdateFrequency = 100;
	bReplicates = true;
    AActor::SetReplicateMovement(true);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetIsReplicated(true);

	Mesh->SetSimulatePhysics(true);
	Mesh->SetLinearDamping(5.0f);
	SetRootComponent(Mesh);

	PhysicsComponent = CreateDefaultSubobject<UPhysicsConstraintComponent>("PhysicsConstraint");
	PhysicsComponent->SetIsReplicated(true);

	PhysicsComponent->SetLinearXLimit(LCM_Free, 0.0f);
	PhysicsComponent->SetLinearYLimit(LCM_Free, 0.0f);
	PhysicsComponent->SetLinearZLimit(LCM_Free, 0.0f);

	PhysicsComponent->SetAngularSwing1Limit(ACM_Locked, 0.0f);
	PhysicsComponent->SetAngularSwing2Limit(ACM_Locked, 0.0f);
	PhysicsComponent->SetAngularTwistLimit(ACM_Locked, 0.0f);

	FConstrainComponentPropName ComponentProName;
	ComponentProName.ComponentName = "Mesh";
	PhysicsComponent->ComponentName1 = ComponentProName;
	PhysicsComponent->SetupAttachment(Mesh);


	Tags.Add("pushable");

	triggerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TriggerMesh"));
	triggerMesh->SetupAttachment(GetRootComponent());
	triggerMesh->SetIsReplicated(true);

}

// Called when the game starts or when spawned
void APushableActor::BeginPlay()
{
	Super::BeginPlay();

	SetReplicates(true);
	SetReplicateMovement(true);

	triggerMesh->SetVisibility(false);
	triggerMesh->SetCollisionProfileName(FName("OverlapAll"));
}

// Called every frame
void APushableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		TArray<AActor*> overlapingActors;
		AActor* triggerActor = nullptr;
		triggerMesh->GetOverlappingActors(overlapingActors);

		for (int i = 0; i < overlapingActors.Num(); ++i)
		{
			AActor* actor = overlapingActors[i];
			if (actor != nullptr && actor->ActorHasTag("TriggerActor"))
			{
				triggerActor = actor;
				break;
			}
		}

		//Check if we got a valid actor pointer

		if (triggerActor)
		{
			if (!bCanPushObj)
			{
				bCanPushObj = true;
				//Fire delegate

				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "CanPush");
				if (Character)
				{
					return;
				}

				Character = Cast<AChronoQuestCharacter>(triggerActor);

				if (Character)
				{
					Character->bCanPushObj = true;
					Character->PushableActor = this;
					bCanPushObj = true;
					InteractionsCount++;
				}
			}
		}
		else
		{
			if (bCanPushObj)
			{
				bCanPushObj = false;
				//Fire delegate
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "NOT PUSH");

				if (Character)
				{
					Character->bCanPushObj = false;
					Character->PushableActor = nullptr;
					Character->bIsInteracting = false;
					bCanPushObj = false;
					InteractionsCount--;
					Character = nullptr;
				}
			}
		}
	}

}

void APushableActor::RPCSERVER_ActorOverlapping_Implementation(AActor* OverlappingActor)
{
	if(Character)
	{
		return;	
	}

	Character = Cast<AChronoQuestCharacter>(OverlappingActor);

	if(Character)
	{
		Character->bCanPushObj = true;
		Character->PushableActor = this;
		bCanPushObj = true;
		InteractionsCount++;
	}
}

void APushableActor::RPCSERVER_ActorExitOverlapping_Implementation()
{
	if (Character)
	{
		Character->bCanPushObj = false;
		Character->PushableActor = nullptr;
		bCanPushObj = false;
		InteractionsCount--;
	}
}

