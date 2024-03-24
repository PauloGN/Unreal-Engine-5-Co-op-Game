// Fill out your copyright notice in the Description page of Project Settings.


#include "GameRules/WinBox.h"

#include "ChronoQuest/ChronoQuestCharacter.h"
#include "Components/BoxComponent.h"

// Sets default values
AWinBox::AWinBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	boxComponet = CreateDefaultSubobject<UBoxComponent>(TEXT("Win Area"));
	SetRootComponent(boxComponet);

	bHasFinished = false;

}

// Called when the game starts or when spawned
void AWinBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWinBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(HasAuthority())
	{
		if(!bHasFinished)
		{
			TArray<AActor*> overlapActors;
			boxComponet->GetOverlappingActors(overlapActors, AChronoQuestCharacter::StaticClass());
			bHasFinished = overlapActors.Num() >= 2;

			if(bHasFinished)
			{
				GEngine->AddOnScreenDebugMessage(-1, 25, FColor::Emerald, "WINNER...");
			}
		}
	}
}

