// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactions/InteractInterface.h"
#include "PushableObject.generated.h"


UCLASS()
class APushableObject : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APushableObject();

	void HandleInteraction(AChronoQuestCharacter* myCharacter);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Environment")
	TObjectPtr<UStaticMeshComponent> Mesh;

	//Go through all existing PushTransform and returns the one nearest the player
	int32 FindClosestPushTransform(FVector2D CharacterCurrentLocation, float PushRange);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Overriden function from the interface that defines what should happen on the character interaction
	virtual void OnInteracted(AChronoQuestCharacter* myCharacter) override;

	UPROPERTY(BlueprintReadonly, EditInstanceOnly, meta = (MakeEditWidget = "true"))
	TArray<FTransform>  PushTransforms;

private:

	//Gets the world transform of an specific PushTransform by its index 
	FTransform GetWorldPushTransform(const int32 Index);

	//Variable saves the position that character should be alined to in order to push the object avoiding weird position.
	UPROPERTY(Replicated)
	FTransform CharacterPushTransform;

	UPROPERTY(Replicated)
	bool bReadyAndGoodToPush;

	bool CheckAreaByCapsuleTracedByChanel(AChronoQuestCharacter* myCharacter);

	bool CheckFowardObjectWithLineTraceByChanel(AChronoQuestCharacter* myCharacter);
};
