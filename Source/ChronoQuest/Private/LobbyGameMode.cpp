// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

void ALobbyGameMode::GoToNextLevel(const FString mapFullPath)
{
	bUseSeamlessTravel = true;

	UWorld* world = GetWorld();
	FString nextLevel = FString::Printf(TEXT("%s?listen"), *mapFullPath);
	if (world)
	{
		world->ServerTravel(nextLevel);
	}
}
