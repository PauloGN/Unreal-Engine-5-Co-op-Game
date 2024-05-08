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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Environment")
	TObjectPtr<UStaticMeshComponent> Mesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnInteracted(AChronoQuestCharacter* myCharacter) override;

};
