// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CPP_Transporter.h"
#include "CPP_TriggerPlatform.h"
#include "CollectableKey.h"
#include "TheFireKey.h"

UCPP_Transporter::UCPP_Transporter()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	moveTime = 3.0f;
	activateTriggerActorsCount = 0;
	bAllTriggerActorsTriggered = false;

	startPoint = FVector::Zero();
	endPoint = FVector::Zero();
	bArePointsSet = false;
	bMoveWithOneTrigger = false;
}

void UCPP_Transporter::SetPoints(const FVector& start, const FVector& end)
{
	if(start.Equals(end))
	{
		return;
	}
	
	startPoint = start;
	endPoint = end;
	bArePointsSet = true;
}

void UCPP_Transporter::OntriggerActivated()
{
	++activateTriggerActorsCount;

	if(bMoveWithOneTrigger)
	{
		bAllTriggerActorsTriggered = true;
	}

	if(activateTriggerActorsCount >= triggerActors.Num())
	{
		bAllTriggerActorsTriggered = true;
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Black, FString::Printf(TEXT("AEWWWW %d"), activateTriggerActorsCount));
	}
}

void UCPP_Transporter::OntriggerDeactivated()
{
	--activateTriggerActorsCount;
	const int numOfNeededTriggers = bMoveWithOneTrigger ? 1 : triggerActors.Num();

	if (activateTriggerActorsCount < numOfNeededTriggers)
	{
		bAllTriggerActorsTriggered = false;
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("AEWWWW %d"), activateTriggerActorsCount));
	}
}

void UCPP_Transporter::BeginPlay()
{
	Super::BeginPlay();

	for (AActor* actor : triggerActors)
	{
		ACPP_TriggerPlatform* tp = Cast<ACPP_TriggerPlatform>(actor);
		if(tp)
		{
			tp->OnTriggerActivated.AddDynamic(this, &ThisClass::OntriggerActivated);
			tp->OnTriggerDeactivated.AddDynamic(this, &ThisClass::OntriggerDeactivated);
			continue;
		}

		ACollectableKey* ck = Cast<ACollectableKey>(actor);
		if(ck)
		{
			ck->OnCollected.AddDynamic(this, &ThisClass::OntriggerActivated);
			continue;
		}

		ATheFireKey* fk = Cast<ATheFireKey>(actor);
		if (fk)
		{
			fk->OnFireLights.AddDynamic(this, &ThisClass::OntriggerActivated);
		}
	}
}

void UCPP_Transporter::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* owner = GetOwner();
	if(owner && owner->HasAuthority() && bArePointsSet)
	{
		//moving actor
		FVector currentLocation = owner->GetActorLocation();
		const float speed = FVector::Distance(startPoint, endPoint) / moveTime;

		FVector targetLocation = bAllTriggerActorsTriggered ? endPoint : startPoint;
		if(!currentLocation.Equals(targetLocation))
		{
			FVector newLocation = FMath::VInterpConstantTo(currentLocation, targetLocation, DeltaTime, speed);
			owner->SetActorLocation(newLocation);
		}
	}
}