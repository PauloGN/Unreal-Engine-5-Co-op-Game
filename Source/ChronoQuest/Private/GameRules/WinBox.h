// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WinBox.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWinAreaOnWinCondition);

UCLASS()
class AWinBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWinBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool bHasFinished;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class UBoxComponent* boxComponet;

	//RPC
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCWin();

	UPROPERTY(BlueprintAssignable)
	FWinAreaOnWinCondition OnWinCondition;

};
