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

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UStaticMeshComponent* triggerMesh;

	UPROPERTY(EditAnywhere, Category = "Environment")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = "Environment")
	UPhysicsConstraintComponent* PhysicsComponent;

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void RPCSERVER_ActorOverlapping(AActor* OverlappingActor);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void RPCSERVER_ActorExitOverlapping();

	UPROPERTY(BlueprintReadWrite) bool bCanPushObj = false;

private:

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "References")
	TObjectPtr<AChronoQuestCharacter> Character;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "References")
	int32 InteractionsCount = 0;
};
