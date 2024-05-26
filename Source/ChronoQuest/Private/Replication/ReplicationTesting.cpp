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

    const APlayerController* PlayerController = Cast<APlayerController>(MyCharacter->GetController());
	this->SetOwner(MyCharacter->GetController());
    if (PlayerController)
    {
        if (MyCharacter->HasAuthority())
        {
            GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Green, FString::Printf(TEXT("%d : Spawn "), ID));
            MulticastRPC_OnInteracted();
        }
        else if(MyCharacter->IsLocallyControlled())
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%d : Client "), ID));
            MyCharacter->Server_InteractionCall(this);
        }
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No valid player controller found"));
    }
}

void AReplicationTesting::MulticastRPC_OnInteracted_Implementation()
{
    int Id = static_cast<int32>(GPlayInEditorID);
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Multicast id: %d"), Id));

    if(ActorHasTag("Explosion"))
    {
	    // Emit particles on both server and clients
	    if (!IsRunningDedicatedServer())
	    {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("Explosion: %d"), Id));
	        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleEffect, GetActorLocation(), FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
	    }
    }
}

// Called every frame
void AReplicationTesting::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
