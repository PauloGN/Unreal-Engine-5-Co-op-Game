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

}

// Called when the game starts or when spawned
void APushableActor::BeginPlay()
{
	Super::BeginPlay();


}

// Called every frame
void APushableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

