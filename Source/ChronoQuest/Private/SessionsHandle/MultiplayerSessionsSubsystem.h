// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerSessionsSubsystem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerCreateDelegate, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerJoinDelegate, bool, bWasSuccessful);

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
	FName mySessionName;
	FString lastServerName;
	FString serverNameToFind;
	bool bCreateServerOnDestroy;
	TSharedPtr<FOnlineSessionSearch> sessionSearch;

	UFUNCTION(BlueprintCallable)
	void SetMapIndex(const int index);

	//Delegate calls
	void OnCreateSessionComplete(FName SessionName, bool bWasuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasuccessful);
	void OnFindSessionsComplete(bool bWasuccessful);
	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);
	//Custom delegates
	UPROPERTY(BlueprintAssignable)
	FServerCreateDelegate serverCreateDelegate;
	UPROPERTY(BlueprintAssignable)
	FServerJoinDelegate serverJoinDelegate;

	UPROPERTY(BlueprintReadWrite)
	TArray<FString> gameMapsName;

private:

	int mapIndex = 0;

};
