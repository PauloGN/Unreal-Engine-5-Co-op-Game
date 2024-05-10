// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactions/InteractInterface.h"
#include <Components/ArrowComponent.h>
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


	int32 FindClosestPushTransform(FVector2D CharacterCurrentLocation, float PushRange);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnInteracted(AChronoQuestCharacter* myCharacter) override;

	UPROPERTY(BlueprintReadonly, EditInstanceOnly, meta = (MakeEditWidget = "true"))
	TArray<FTransform>  PushTransforms;



private:
	FTransform GetWorldPushTransform(const int32 Index);

	FTransform CharacterPushTransform;
};
