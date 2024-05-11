// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PushableObject.h"

#include "ChronoQuest/ChronoQuestCharacter.h"
#include "Components/CapsuleComponent.h"
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

FTransform APushableObject::GetWorldPushTransform(const int32 Index)
{
	return PushTransforms[Index] * GetActorTransform();
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
		//Calculates the nearest pushTransform for character interact with
		const int32 BestIndex = FindClosestPushTransform(MyCharacterGroundLocation, CharacterPushComponent->PushRange);

		if(BestIndex >= 0)
		{
			const FTransform WorldTransform = GetWorldPushTransform(BestIndex);
			CharacterPushTransform = WorldTransform;

			const float ZOffset = myCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

			CharacterPushTransform.SetLocation({ WorldTransform.GetLocation().X, WorldTransform.GetLocation().Y, WorldTransform.GetLocation().Z + ZOffset });

			DrawDebugSphere(GetWorld(), CharacterPushTransform.GetLocation(), 30, 10, FColor::Purple, false, 5.0f, 0, 3.0f);
		}
	}
}

// Called when the game starts or when spawned
void APushableObject::BeginPlay()
{
	Super::BeginPlay();
	
}

int32 APushableObject::FindClosestPushTransform(FVector2D CharacterCurrentLocation, float PushRange)
{
	int16 BestIndex = -1;

	float MinRangeSqrd = FMath::Square(PushRange);

	for (int i = 0 ;i < PushTransforms.Num(); ++i)
	{
		const FTransform WorldTransform = GetWorldPushTransform(i);
		const FVector2D TransformWorldLocation (WorldTransform.GetLocation().X, WorldTransform.GetLocation().Y);

		const float CurrentDistSqrd = FVector2D::DistSquared(TransformWorldLocation, CharacterCurrentLocation);

		if(CurrentDistSqrd > FMath::Square(PushRange))
		{
			continue;
		}

		if(CurrentDistSqrd <= MinRangeSqrd)
		{
			BestIndex = i;
			MinRangeSqrd = CurrentDistSqrd;
		}
	}
	return BestIndex;
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

