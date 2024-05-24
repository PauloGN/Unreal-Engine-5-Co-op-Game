// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/GameplayStatics.h"
#include "Replication/ReplicationTesting.h"

// Sets default values
AReplicationTesting::AReplicationTesting()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
    AActor::SetReplicateMovement(true);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetIsReplicated(true);

	SetRootComponent(Mesh);

	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

void AReplicationTesting::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

// Called when the game starts or when spawned
void AReplicationTesting::BeginPlay()
{
	Super::BeginPlay();
		
}

void AReplicationTesting::OnInteracted(AChronoQuestCharacter* myCharacter)
{
	int32 ID = static_cast<int32>(GPlayInEditorID);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("%d : Has interface"), ID));
	MulticastRPC_Testing();
}

void AReplicationTesting::MulticastRPC_Testing_Implementation()
{
	if(HasAuthority())
	{
		int Id = (GPlayInEditorID);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Server id: %d "), Id));

	}else
	{
		int Id = (GPlayInEditorID);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Client id: %d "), Id));
	}

	//Emit particles

	if(!IsRunningDedicatedServer())
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleEffect, GetActorLocation(), FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
	}
}

void AReplicationTesting::SERVERRPC_Testing_Implementation()
{
	int Id = (GPlayInEditorID);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Server id: %d "), Id));
}

// Called every frame
void AReplicationTesting::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

