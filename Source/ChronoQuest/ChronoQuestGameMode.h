// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ChronoQuestGameMode.generated.h"

UCLASS(minimalapi)
class AChronoQuestGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	AChronoQuestGameMode();

	UFUNCTION(BlueprintCallable)
	void GoToNextLevel(const FString mapFullPath);

	UFUNCTION(BlueprintCallable)
	void HostLanGame();

	UFUNCTION(BlueprintCallable)
	void JoinLanGame();
};
