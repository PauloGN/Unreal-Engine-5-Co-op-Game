#include "Replication/ReplicationTesting.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include <ChronoQuest/ChronoQuestCharacter.h>

// Sets default values
AReplicationTesting::AReplicationTesting()
{
    // Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
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
    // Replicate any necessary properties here
}

// Called when the game starts or when spawned
void AReplicationTesting::BeginPlay()
{
    Super::BeginPlay();
    SetReplicates(true);
}

void AReplicationTesting::OnInteracted(AChronoQuestCharacter* MyCharacter)
{
    int32 ID = static_cast<int32>(GPlayInEditorID);
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("%d : Has interface"), ID));

    APlayerController* PlayerController = Cast<APlayerController>(MyCharacter->GetController());
	this->SetOwner(MyCharacter->GetController());
    if (PlayerController)
    {
        if (HasAuthority())
        {
            MulticastRPC_Testing();
        }
        else
        {
            SetOwner( GetGameInstance()->GetWorld()->GetFirstPlayerController());

            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("%d : Client "), ID));
            SERVERRPC_Testing();
        }
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No valid player controller found"));
    }
}

void AReplicationTesting::MulticastRPC_Testing_Implementation()
{
    int Id = static_cast<int32>(GPlayInEditorID);
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Multicast id: %d"), Id));

    // Emit particles on both server and clients
    if (!IsRunningDedicatedServer())
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleEffect, GetActorLocation(), FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
    }
}

void AReplicationTesting::SERVERRPC_Testing_Implementation()
{
    // Only the server should call the multicast function to ensure it propagates to all clients
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, TEXT("SERVERRPC_Testing called on Server"));
    MulticastRPC_Testing();
}


// Called every frame
void AReplicationTesting::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

