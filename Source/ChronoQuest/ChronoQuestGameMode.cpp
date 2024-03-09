// Copyright Epic Games, Inc. All Rights Reserved.

#include "ChronoQuestGameMode.h"
#include "ChronoQuestCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerController.h"

AChronoQuestGameMode::AChronoQuestGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AChronoQuestGameMode::HostLanGame()
{
	//lounch the game as a listen server
	GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");
}

void AChronoQuestGameMode::JoinLanGame()
{
	APlayerController* pc = GetGameInstance()->GetFirstLocalPlayerController();
	if(pc)
	{
		pc->ClientTravel("192.168.1.162",TRAVEL_Absolute);
	}
}
