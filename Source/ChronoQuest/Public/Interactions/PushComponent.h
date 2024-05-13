// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PushComponent.generated.h"

class APushableObject;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CHRONOQUEST_API UPushComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPushComponent();

	UPROPERTY(Replicated)
	TObjectPtr<APushableObject> CurrentPushable;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void PushingLogic(APushableObject* PushableObject);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pushing Set up")
	float PushSpeed = 50.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pushing Set up")
	float PushRange = 100.0f;

	UFUNCTION(Server, Reliable)
	void ServerRPCPushingLogicCall(APushableObject* PushableObject);

	UFUNCTION()
	void BeginPush(APushableObject* PushableObject);

	UFUNCTION()
	void EndPush();
};
