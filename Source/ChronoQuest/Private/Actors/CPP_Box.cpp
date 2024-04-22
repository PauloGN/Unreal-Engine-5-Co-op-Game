// Fill out your copyright notice in the Description page of Project Settings.
#include "Actors/CPP_Box.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACPP_Box::ACPP_Box()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	replicatedFloat = 100.0f;
	CountingDownFloat = 0.0f;
}

// Called when the game starts or when spawned
void ACPP_Box::BeginPlay()
{
	Super::BeginPlay();
	//SetReplicates(true);
	SetReplicateMovement(true);
	
	if (HasAuthority())
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("Box Say: Hello from Server"));
	}else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Client:  Hello from client"));
	}

	if(HasAuthority())
	{
		CountingDownFloat = 0.0f;

		GetWorld()->GetTimerManager().SetTimer(TimerCountingDownHandle, this, &ThisClass::DecreaseReplicatedFloat, 2.0f, false);
		GetWorld()->GetTimerManager().SetTimer(testTimer, this, &ThisClass::MulticastRPCExplode, 1.0f, false);
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
	DOREPLIFETIME(ACPP_Box, CountingDownFloat);
}

void ACPP_Box::OnRep_CountingDownFloat()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("ID %d: CountingDown"), static_cast<int>(GPlayInEditorID)));
}

void ACPP_Box::DecreaseReplicatedFloat()
{
	if(HasAuthority())
	{
		CountingDownFloat += 1.0f;
		//calling on the server
		OnRep_CountingDownFloat();
		if(CountingDownFloat < 9999.0f)
		{
			GetWorld()->GetTimerManager().SetTimer(TimerCountingDownHandle, this, &ThisClass::DecreaseReplicatedFloat, 1.0f, false);
		}
	}
}

void ACPP_Box::MulticastRPCExplode_Implementation()
{
	if(HasAuthority())
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("Server %d: Multicast call"), static_cast<int>(GPlayInEditorID)));
		GetWorld()->GetTimerManager().SetTimer(testTimer, this, &ThisClass::MulticastRPCExplode, 7.0f, false);

	}else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Client %d: Multicast call"), static_cast<int>(GPlayInEditorID)));
	}


	if(!IsRunningDedicatedServer() && explosionEffect != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), explosionEffect, GetActorLocation(), FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
	}
}
