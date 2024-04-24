// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CPP_Transporter.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UCPP_Transporter : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCPP_Transporter();

	UFUNCTION(BlueprintCallable)
	void SetPoints(const FVector& start, const FVector& end);

	UFUNCTION()
	void OntriggerActivated();

	UFUNCTION()
	void OntriggerDeactivated();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FVector startPoint;
	FVector endPoint;
	bool bArePointsSet;

	//How long it takes to move the owning actor from one point to another
	UPROPERTY(EditAnywhere)
	float moveTime;
	
	//Triggers responsible to activate this component functionality
	UPROPERTY(EditAnywhere)
	TArray<AActor*> triggerActors;

	//associated to the number of necessary triggers to be active
	UPROPERTY(VisibleAnywhere)
	int activateTriggerActorsCount;

	//True once all associated triggers are activated
	UPROPERTY(BlueprintReadWrite ,VisibleAnywhere)
	bool bAllTriggerActorsTriggered;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly)
	bool bMoveWith_X_Triggers;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly)
	int numOfTriggersToActivate = 1;
};
