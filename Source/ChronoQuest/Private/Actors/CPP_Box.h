// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_Box.generated.h"

UCLASS()
class ACPP_Box : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACPP_Box();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Replication VARIABLES
	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedFloat, BlueprintReadWrite)
	float replicatedFloat;

	UFUNCTION(BlueprintCallable)
	void OnRep_ReplicatedFloat();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
