// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PushableObject.h"

#include "ChronoQuest/ChronoQuestCharacter.h"
#include "Interactions/PushComponent.h"

// Sets default values
APushableObject::APushableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NetUpdateFrequency = 100;
	bReplicates = true;
	AActor::SetReplicateMovement(true);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetIsReplicated(true);
	SetRootComponent(Mesh);

	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

}

void APushableObject::HandleInteraction(AChronoQuestCharacter* myCharacter)
{

	if(!myCharacter)
	{
		return;
	}

	FTransform MyCharacterTransform = myCharacter->GetActorTransform();
	FVector2D MyCharacterGroundLocation(MyCharacterTransform.GetLocation().X, MyCharacterTransform.GetLocation().Y);



	UPushComponent* CharacterPushComponent =  Cast<UPushComponent>(myCharacter->GetComponentByClass(UPushComponent::StaticClass()));

	if(CharacterPushComponent)
	{

		GEngine->AddOnScreenDebugMessage(3, 2.0f, FColor::Green, FString::Printf(TEXT("%f"), CharacterPushComponent->PushSpeed));
		CharacterPushComponent->Teest();

		//Calculates the nearest pushTransform for character interact with
		const int32 BestIndex = FindClosestPushTransform(MyCharacterGroundLocation, CharacterPushComponent->PushRange);
	}

}

// Called when the game starts or when spawned
void APushableObject::BeginPlay()
{
	Super::BeginPlay();
	
}

int32 APushableObject::FindClosestPushTransform(FVector2D CharacterCurrentLocation, float PushRange)
{
	return 0;
}

// Called every frame
void APushableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APushableObject::OnInteracted(AChronoQuestCharacter* myCharacter)
{
	GEngine->AddOnScreenDebugMessage(33, 1.0f, FColor::Green, "Interaction Called");

	HandleInteraction(myCharacter);
}

