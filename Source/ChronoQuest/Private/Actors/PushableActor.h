// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "PushableActor.generated.h"

class UPhysicsConstraintComponent;
class AChronoQuestCharacter;
class UCurveFloat;

UCLASS()
class APushableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APushableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Environment")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = "Environment")
	UPhysicsConstraintComponent* PhysicsComponent;

private:

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "References")
	TObjectPtr<AChronoQuestCharacter> Character;
};
