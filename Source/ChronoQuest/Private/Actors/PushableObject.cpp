// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/PushableObject.h"

#include "CPP_TriggerPlatform.h"
#include "ChronoQuest/ChronoQuestCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interactions/PushComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APushableObject::APushableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	AActor::SetReplicateMovement(true);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetIsReplicated(true);
	SetRootComponent(Mesh);

	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

bool APushableObject::IsBusy()
{
	return bIsBeingPushed;
}

void APushableObject::SetBusy(bool value)
{
	bIsBeingPushed = value;
}

UStaticMeshComponent* APushableObject::GetMesh() const
{
	return Mesh;
}

FTransform APushableObject::GetWorldPushTransform(const int32 Index)
{
	return PushTransforms[Index] * GetActorTransform();
}

bool APushableObject::CheckAreaByCapsuleTracedByChanel(AChronoQuestCharacter* myCharacter) const
{

	bool CanPush = false;

	// Set up parameters for the trace
	FCollisionQueryParams TraceParams(FName(TEXT("Trace")), true, nullptr);
	TraceParams.bTraceComplex = true; // Trace against complex geometry

	// Specify channels to ignore
	TraceParams.AddIgnoredActor(GetOwner()); // Ignore self
	TraceParams.AddIgnoredActor(myCharacter); // Ignore Main Character
	// You can add more actors to ignore using TraceParams.AddIgnoredActor()

	// Define the start and end points of the trace
	const float X = CharacterPushTransform.GetLocation().X;
	const float Y = CharacterPushTransform.GetLocation().Y;
	const float Z_Start = CharacterPushTransform.GetLocation().Z + 10.0f;
	const float Z_End = CharacterPushTransform.GetLocation().Z + 20.0f;
	FVector StartTrace = FVector(X, Y, Z_Start); // Starting point of the trace
	FVector EndTrace = FVector(X, Y, Z_End); // Ending point of the trace

	// Set up parameters for the capsule shape
	float Radius = myCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius(); // Radius of the capsule
	float HalfHeight = myCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(); // Half the height of the capsule

	// Perform the trace
	FHitResult HitResult;
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		StartTrace,
		EndTrace,
		FQuat::Identity,
		ECC_GameTraceChannel1, // Use your desired collision channel here
		FCollisionShape::MakeCapsule(Radius, HalfHeight),
		TraceParams
	);

	const float CharWalkableFloorZ = myCharacter->GetCharacterMovement()->GetWalkableFloorZ();
	const float HitResultImpactNormalZ = HitResult.ImpactNormal.Z;

	const bool bIsFloorWalkable = CharWalkableFloorZ < HitResultImpactNormalZ;

	CanPush = (bHit && bIsFloorWalkable) || !bHit;

	// Check if floor is good to walk
	if (bIsFloorWalkable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Walkable"));
	}

	// Check if there was a hit
	if (bHit)
	{
		// Handle the hit
		// For example, you might print some debug information
		FString HitActorName = HitResult.GetActor() ? HitResult.GetActor()->GetName() : FString(TEXT("None"));
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActorName);
	}

	// Draw debug visualization of the trace
	DrawDebugCapsule(
		GetWorld(),
		StartTrace,
		HalfHeight,
		Radius,
		FQuat::Identity,
		bHit ? FColor::Red : FColor::Green, // Color depending on whether there was a hit or not
		false, // Persistent lines
		2.0f // Duration (in seconds)
	);

	// Check if floor is good to walk
	if (CanPush)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pushable"));
	}

	return CanPush;
}

bool APushableObject::CheckFowardObjectWithLineTraceByChanel(AChronoQuestCharacter* myCharacter) const
{
	FVector StartLocation = GetActorLocation(); // Start location of the trace
	FVector EndLocation = CharacterPushTransform.GetLocation(); // End location of the trace

	FHitResult HitResult; // Variable to store the result of the trace
	FCollisionQueryParams TraceParams(FName(TEXT("Trace")), true, this); // Parameters for the trace
	TraceParams.AddIgnoredActor(this);
	TraceParams.AddIgnoredActor(myCharacter);

	TraceParams.bTraceComplex = true; // Trace against complex geometry
	TraceParams.bReturnPhysicalMaterial = false; // Return the physical material of the hit

	// Perform the line trace
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult, // Hit result
		StartLocation, // Start location
		EndLocation, // End location
		ECC_GameTraceChannel1, // Trace channel
		TraceParams // Additional parameters
	);

	if (bHit)
	{
		if (AActor* HitActor = Cast<ACPP_TriggerPlatform>(HitResult.GetActor()))
		{
			GEngine->AddOnScreenDebugMessage(33, 1.0f, FColor::Green, "Trigger");
			return false;// Do something with the hit actor
		}

		// Draw debug line for visualization
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1.f, 0, 2.0f);
	}
	else
	{
		// If we didn't hit anything, handle accordingly
	}

	return bHit;
}

void APushableObject::HandleInteraction(AChronoQuestCharacter* myCharacter)
{
	if(!myCharacter || IsBusy())
	{
		return;
	}

	const FTransform MyCharacterTransform = myCharacter->GetActorTransform();
	const FVector2D MyCharacterGroundLocation(MyCharacterTransform.GetLocation().X, MyCharacterTransform.GetLocation().Y);

	if(UPushComponent* CharacterPushComponent =  Cast<UPushComponent>(myCharacter->GetComponentByClass(UPushComponent::StaticClass())))
	{
		//Calculates the nearest pushTransform for character interact with
		const int32 BestIndex = FindClosestPushTransform(MyCharacterGroundLocation, CharacterPushComponent->PushRange);

		if(BestIndex >= 0)
		{
			const FTransform WorldTransform = GetWorldPushTransform(BestIndex);
			CharacterPushTransform = WorldTransform;

			const float ZOffset = myCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

			CharacterPushTransform.SetLocation({ WorldTransform.GetLocation().X, WorldTransform.GetLocation().Y, WorldTransform.GetLocation().Z + ZOffset });

			CharacterPushTransform.SetScale3D(myCharacter->GetActorScale3D());

			DrawDebugSphere(GetWorld(), CharacterPushTransform.GetLocation(), 30, 10, FColor::Purple, false, 5.0f, 0, 3.0f);

			//Check for Obstacles and check for clear area to teleport to.

			const bool HasEnoughSpaceToPush = CheckAreaByCapsuleTracedByChanel(myCharacter);
			const bool HasHisSomething = CheckFowardObjectWithLineTraceByChanel(myCharacter);

			bReadyAndGoodToPush = (HasEnoughSpaceToPush && !HasHisSomething);

			if (bReadyAndGoodToPush)
			{
				bReadyAndGoodToPush = false;

				// If on the server, directly set the actor's transform
				if (HasAuthority())
				{
					myCharacter->SetActorTransform(CharacterPushTransform, false, nullptr, ETeleportType::TeleportPhysics);
				}
				else // If on the client, call the server RPC function
				{
					myCharacter->ServerSetActorTransform(CharacterPushTransform);

					if (myCharacter->IsLocallyControlled())
					{
						// Update the character's transform locally
						myCharacter->SetActorTransform(CharacterPushTransform, false, nullptr, ETeleportType::TeleportPhysics);
					}
				}

				//Move
				CharacterPushComponent->BeginPush(this);
			}
		}
	}
}

// Called when the game starts or when spawned
void APushableObject::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);
	}
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

void APushableObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APushableObject, bIsBeingPushed);
	DOREPLIFETIME(APushableObject, bReadyAndGoodToPush);
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
