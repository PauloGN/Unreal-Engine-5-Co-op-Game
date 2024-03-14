// Fill out your copyright notice in the Description page of Project Settings.

#include "SessionsHandle/MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

namespace 
{
	void PrintString(const FString& text)
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Yellow, text);
		}
	}
}

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
	PrintString(TEXT("Construction"));
}

void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	//PrintString(TEXT("Initialized"));
	IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get();

	if(onlineSubsystem)
	{
		// it could be any online subsystem service out there (NULL, steam, facebook, google play, etc.)
		FString subsystemName = onlineSubsystem->GetSubsystemName().ToString();
		PrintString(subsystemName);

		bIsLanConnection = subsystemName == FString("Steam") ? false : true;

		//Getting the session interface
		sessionInterface = onlineSubsystem->GetSessionInterface();
		if(sessionInterface.IsValid())
		{
			PrintString(TEXT("Valid Session"));
		}
	}
}

void UMultiplayerSessionsSubsystem::Deinitialize()
{
	Super::Deinitialize();

	UE_LOG(LogTemp, Warning, TEXT("Deinitialize"));
}

void UMultiplayerSessionsSubsystem::CreateServer(const FString& serverName)
{

	if(serverName.IsEmpty())
	{
		PrintString(TEXT("Server Name can not be empty: ") + serverName);
		return;
	}

	/**
	 *\To create a Server:
	 *	1 - need a Session
	 *
	 */

	FName mySessionName = FName("Co-op ChronoQuest");
	FOnlineSessionSettings sessionSettings;

	sessionSettings.bAllowJoinInProgress = true;
	sessionSettings.bIsDedicated = false;
	sessionSettings.bShouldAdvertise = true;
	sessionSettings.NumPublicConnections = 3;
	sessionSettings.bUseLobbiesIfAvailable = true;
	sessionSettings.bUsesPresence = true;
	sessionSettings.bAllowJoinViaPresence = true;

	sessionSettings.bIsLANMatch = bIsLanConnection;

	if(bIsLanConnection)
	{
		PrintString(TEXT("True"));
		return;
	}

	PrintString(TEXT("false"));

	sessionInterface->CreateSession(0, mySessionName, sessionSettings);

}

void UMultiplayerSessionsSubsystem::FindServer(const FString& serverName)
{
	PrintString(TEXT("Find Server Called for: ") + serverName);
}
