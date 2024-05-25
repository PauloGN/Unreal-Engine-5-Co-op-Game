// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactions/InteractInterface.h"
#include "Interactions/RCPCallsInterface.h"
#include "ReplicationTesting.generated.h"

UCLASS()
class CHRONOQUEST_API AReplicationTesting : public AActor, public IInteractInterface, public IRCPCallsInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AReplicationTesting();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnInteracted(AChronoQuestCharacter* myCharacter) override;
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	virtual void MulticastRPC_SpawnEffects() override;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ParticleEffect;
};
