// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/TheFireKey.h"

#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ATheFireKey::ATheFireKey()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	AActor::SetReplicateMovement(true);

	rocks = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rocks"));
	SetRootComponent(rocks);

	wood = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wood"));
	wood->SetupAttachment(rocks);

	ash = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ash"));
	ash->SetupAttachment(rocks);

	smoke = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Smoke"));
	smoke->SetupAttachment(rocks);
	smoke->AddRelativeLocation(rocks->GetComponentLocation() + FVector(0.0f, 0.0f, 60.0f));

	fire = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Fire"));
	fire->SetupAttachment(rocks);
	fire->SetVisibility(false);

	ambientLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("AmbientLight"));
	ambientLight->SetupAttachment(rocks);
	ambientLight->SetVisibility(false);

	shadowedLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("ShadowedLight"));
	shadowedLight->SetupAttachment(rocks);
	shadowedLight->SetVisibility(false);
	
	capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Comp"));
	capsule->SetupAttachment(rocks);
	capsule->SetIsReplicated(true);
	capsule->SetCollisionProfileName(FName("OverlapAllDynamic"));
	capsule->SetCapsuleRadius(54.0f);

	bIsFireOn = false;
}

void ATheFireKey::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATheFireKey, bIsFireOn);
}

// Called when the game starts or when spawned
void ATheFireKey::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATheFireKey::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
//
//void ATheFireKey::LightFire_Inplemented()
//{
//	if(!bIsFireOn && !fire->GetVisibleFlag())
//	{
//		bIsFireOn = true;
//		OnRep_bIsFireOn();
//	}
//}

void ATheFireKey::OnRep_bIsFireOn()
{
	if (HasAuthority())
	{
		if (bIsFireOn)
		{
			OnFireLights.Broadcast();
			//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, "Server calling fire trigger");
		}
	}

	fire->SetVisibility(true);
	ambientLight->SetVisibility(true);
	shadowedLight->SetVisibility(true);
	smoke->AddRelativeLocation(FVector(0.0f, 0.0f, -10.f));
}

void ATheFireKey::LightFire_Implementation()
{
	if(!bIsFireOn && !fire->GetVisibleFlag())
	{
		bIsFireOn = true;
		OnRep_bIsFireOn();
	}
}
