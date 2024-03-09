// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CPP_Box.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACPP_Box::ACPP_Box()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	replicatedFloat = 100.0f;
}

// Called when the game starts or when spawned
void ACPP_Box::BeginPlay()
{
	Super::BeginPlay();
	SetReplicates(true);
	SetReplicateMovement(true);
	
	if (HasAuthority())
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("Server"));
	}else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Client"));
	}

}

// Called every frame
void ACPP_Box::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACPP_Box::OnRep_ReplicatedFloat()
{
	if(HasAuthority())
	{
		FVector newLocation = GetActorLocation() + FVector(0.0f, 0.0f, 200.0f);
		SetActorLocation(newLocation);
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Server notify"));
	}else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Client %d: Notify"), static_cast<int>(GPlayInEditorID)));
	}

}

void ACPP_Box::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Especial Macro called for each variable that we designate to be replicated
	DOREPLIFETIME(ACPP_Box, replicatedFloat);

}

