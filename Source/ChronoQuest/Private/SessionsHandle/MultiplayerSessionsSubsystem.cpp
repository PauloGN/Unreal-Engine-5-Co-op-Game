// Fill out your copyright notice in the Description page of Project Settings.

#include "SessionsHandle/MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include <Online/OnlineSessionNames.h>

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
	bCreateServerOnDestroy = false;
	lastServerName = "";
	serverNameToFind = "";
}

void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

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
			//Bind Delegate functions
			sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &ThisClass::OnCreateSessionComplete);
			sessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &ThisClass::OnDestroySessionComplete);
			sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &ThisClass::OnFindSessionsComplete);
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

	//check if session already exists
	FNamedOnlineSession* existingSession = sessionInterface->GetNamedSession(mySessionName);
	if(existingSession)
	{
		lastServerName = serverName;
		bCreateServerOnDestroy = true;
		sessionInterface->DestroySession(mySessionName);
		return;
	}

	sessionSettings.bAllowJoinInProgress = true;
	sessionSettings.bIsDedicated = false;
	sessionSettings.bShouldAdvertise = true;
	sessionSettings.NumPublicConnections = 3;
	sessionSettings.bUseLobbiesIfAvailable = true;
	sessionSettings.bUsesPresence = true;
	sessionSettings.bAllowJoinViaPresence = true;
	sessionSettings.bIsLANMatch = bIsLanConnection;
	//session set server name
	sessionSettings.Set(FName("SERVER_NAME"), serverName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	sessionInterface->CreateSession(0, mySessionName, sessionSettings);
}

void UMultiplayerSessionsSubsystem::FindServer(const FString& serverName)
{
	if (serverName.IsEmpty())
	{
		PrintString(TEXT("Server Name can not be empty: ") + serverName);
		return;
	}

	sessionSearch = MakeShareable(new FOnlineSessionSearch);
	//how do we want the search to be done
	sessionSearch->bIsLanQuery = bIsLanConnection;
	sessionSearch->MaxSearchResults = 9999;
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	//Remember server name to find
	serverNameToFind = serverName;

	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasuccessful)
{
	PrintString(FString::Printf(TEXT("Resul: %d"), bWasuccessful));

	if(bWasuccessful)
	{
		GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");
	}
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasuccessful)
{
	if(bCreateServerOnDestroy)
	{
		bCreateServerOnDestroy = false;
		CreateServer(lastServerName);
	}
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasuccessful)
{
	if(!bWasuccessful)
	{
		return;
	}

	if(serverNameToFind.IsEmpty())
	{
		return;
	}

	TArray<FOnlineSessionSearchResult> results = sessionSearch->SearchResults;
	if(results.Num() > 0)
	{
		FString msg = FString::Printf(TEXT("Sessions found: %d"), results.Num());
		PrintString(msg);

		for (FOnlineSessionSearchResult& result : results)
		{
			if(result.IsValid())
			{
				FString serverName = "none";
				result.Session.SessionSettings.Get(FName("SERVER_NAME"), serverName);

				PrintString(FString::Printf(TEXT("SERVER NAME: %s"), *serverName));
			}
		}

		return;
	}
	PrintString("Nope no sessions");
}
