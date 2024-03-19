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
	mySessionName = FName("Co-op ChronoQuest");
}

void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get();

	gameMapsName.Add("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");
	gameMapsName.Add("/Game/ThirdPerson/Maps/Chapter01?listen");

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
			sessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &ThisClass::OnJoinSessionComplete);
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
		serverCreateDelegate.Broadcast(false);
		return;
	}

	/**
	 *\To create a Server:
	 *	1 - need a Session
	 *
	 */

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
		serverJoinDelegate.Broadcast(false);
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

void UMultiplayerSessionsSubsystem::SetMapIndex(const int index)
{

	if(index < 0 || index > gameMapsName.Num())
	{
		mapIndex = 0;
		return;
	}
	mapIndex = index;
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasuccessful)
{
	PrintString(FString::Printf(TEXT("Resul: %d"), bWasuccessful));

	serverCreateDelegate.Broadcast(bWasuccessful);
	if(bWasuccessful)
	{
		GetWorld()->ServerTravel(*gameMapsName[mapIndex]);
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
		serverJoinDelegate.Broadcast(false);
		return;
	}

	if(serverNameToFind.IsEmpty())
	{
		serverJoinDelegate.Broadcast(false);
		return;
	}

	TArray<FOnlineSessionSearchResult> results = sessionSearch->SearchResults;
	FOnlineSessionSearchResult* correctResult = nullptr;
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

				if(serverName.Equals(serverNameToFind))
				{
					correctResult = &result;
					PrintString(FString::Printf(TEXT("SERVER NAME: %s"), *serverName));
					break;
				}
			}
		}

		if(correctResult)
		{
			sessionInterface->JoinSession(0, mySessionName, *correctResult);

		}else
		{
			serverJoinDelegate.Broadcast(false);
			PrintString("...Nope no server....");
		}

		return;
	}
	serverJoinDelegate.Broadcast(false);
	PrintString("Nope no sessions");
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	if(result == EOnJoinSessionCompleteResult::Success)
	{
		PrintString("you got it");
		FString ipAddress;
		if(sessionInterface->GetResolvedConnectString(mySessionName, ipAddress))
		{
			PrintString(ipAddress);
			APlayerController* pc = GetGameInstance()->GetFirstLocalPlayerController();

			if(pc)
			{
				pc->ClientTravel(ipAddress, TRAVEL_Absolute);
			}

		}else
		{
			serverJoinDelegate.Broadcast(false);
			PrintString("Did not get the ip address...");
		}

	}else
	{
		serverJoinDelegate.Broadcast(false);
		PrintString(TEXT("Fail to join..."));
	}
}
