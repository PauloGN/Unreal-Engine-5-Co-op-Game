// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerSessionsSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UMultiplayerSessionsSubsystem();
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

	//wid Get
	UFUNCTION(BlueprintCallable)
	void CreateServer(const FString& serverName);
	UFUNCTION(BlueprintCallable)
	void FindServer(const FString& serverName);

	IOnlineSessionPtr sessionInterface;
	bool bIsLanConnection = false;

	//Session and server Settings
	FString lastServerName;
	bool bCreateServerOnDestroy;
	TSharedPtr<FOnlineSessionSearch> sessionSearch;

	//Delegate calls
	void OnCreateSessionComplete(FName SessionName, bool bWasuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasuccessful);
	void OnFindSessionsComplete(bool bWasuccessful);
};
