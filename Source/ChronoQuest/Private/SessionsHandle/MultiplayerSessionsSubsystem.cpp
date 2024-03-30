// Fill out your copyright notice in the Description page of Project Settings.

#include "SessionsHandle/MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include <Online/OnlineSessionNames.h>

#include "ChronoQuest/ChronoQuestGameMode.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
	//LOG
	UE_LOG(LogTemp, Display, TEXT("UMultiplayerSessionsSubsystem: Construction"));
	bCreateServerOnDestroy = false;
	lastServerName = "";
	serverNameToFind = "";
	mySessionName = FName("Co-op ChronoQuest");
	bAlreadyStartedAsessionBefore = false;
}

void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get();

	gameMapsName.Add("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");
	gameMapsName.Add("/Game/ThirdPerson/Maps/Chapter01?listen");
	gameMapsName.Add("/Game/ThirdPerson/Maps/Lobby?listen");

	if(onlineSubsystem)
	{
		// it could be any online subsystem service out there (NULL, steam, facebook, google play, etc.)
		FString subsystemName = onlineSubsystem->GetSubsystemName().ToString();
		//LOG
		UE_LOG(LogTemp, Display, TEXT("UMultiplayerSessionsSubsystem: Subsystem name = %s"), *subsystemName);

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
		//LOG
		UE_LOG(LogTemp, Display, TEXT("UMultiplayerSessionsSubsystem: Server name can not be empty"));

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
	sessionSettings.NumPublicConnections = 3;//********************************** THREE CONNECTIONS
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
		//LOG
		UE_LOG(LogTemp, Display, TEXT("UMultiplayerSessionsSubsystem: Server name can not be empty"));
		return;
	}

	if (bAlreadyStartedAsessionBefore)
	{
		DestroyLastServer();
		bAlreadyStartedAsessionBefore = false;
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

void UMultiplayerSessionsSubsystem::DestroyLastServer()
{
	//check if session already exists
	FNamedOnlineSession* existingSession = sessionInterface->GetNamedSession(mySessionName);
	if (existingSession)
	{
		bCreateServerOnDestroy = false;
		sessionInterface->DestroySession(mySessionName);
	}
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

int UMultiplayerSessionsSubsystem::GetMapIndex()
{
	return mapIndex;
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasuccessful)
{
	//LOG
	UE_LOG(LogTemp, Display, TEXT("Resul: %d"), bWasuccessful);

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
		//LOG
		UE_LOG(LogTemp, Display, TEXT("UMultiplayerSessionsSubsystem: %s"), *msg);

		for (FOnlineSessionSearchResult& result : results)
		{
			if(result.IsValid())
			{
				FString serverName = "none";
				result.Session.SessionSettings.Get(FName("SERVER_NAME"), serverName);

				if(serverName.Equals(serverNameToFind))
				{
					correctResult = &result;
					//LOG
					UE_LOG(LogTemp, Display, TEXT("UMultiplayerSessionsSubsystem Server name: %s"), *serverName);
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
			//LOG
			UE_LOG(LogTemp, Display, TEXT("UMultiplayerSessionsSubsystem: Nope no server found"));
		}

		return;
	}
	serverJoinDelegate.Broadcast(false);
	//LOG
	UE_LOG(LogTemp, Display, TEXT("UMultiplayerSessionsSubsystem: Nope no session"));
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	if(result == EOnJoinSessionCompleteResult::Success)
	{
		//LOG
		UE_LOG(LogTemp, Display, TEXT("UMultiplayerSessionsSubsystem: You've got a session"));
		FString ipAddress;
		if(sessionInterface->GetResolvedConnectString(mySessionName, ipAddress))
		{
			//LOG
			UE_LOG(LogTemp, Display, TEXT("UMultiplayerSessionsSubsystem IP: %s"), *ipAddress);
			APlayerController* pc = GetGameInstance()->GetFirstLocalPlayerController();

			if(pc)
			{
				bAlreadyStartedAsessionBefore = true;
				pc->ClientTravel(ipAddress, TRAVEL_Absolute);
			}

		}else
		{
			serverJoinDelegate.Broadcast(false);
			//LOG
			UE_LOG(LogTemp, Display, TEXT("UMultiplayerSessionsSubsystem IP: Failed"));
		}

	}else
	{
		serverJoinDelegate.Broadcast(false);
		//LOG
		UE_LOG(LogTemp, Display, TEXT("UMultiplayerSessionsSubsystem : Failed to joind session"));
	}
}

void UMultiplayerSessionsSubsystem::OnEndSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		// Session ended successfully
		UE_LOG(LogTemp, Warning, TEXT("Session '%s' ended successfully."), *SessionName.ToString());
	}
	else
	{
		// Failed to end session
		UE_LOG(LogTemp, Warning, TEXT("Failed to end session '%s'."), *SessionName.ToString());
	}

	// Remove the delegate binding
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->ClearOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegateHandle);
		}
	}
}

void UMultiplayerSessionsSubsystem::DisconnectFromSession(const FName& SessionName)
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			// Bind the delegate function
			OnEndSessionCompleteDelegateHandle = SessionInterface->AddOnEndSessionCompleteDelegate_Handle(FOnEndSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnEndSessionComplete));

			// Start the session ending process
			SessionInterface->EndSession(SessionName);
		}
		else
		{
			// Session interface not valid
			UE_LOG(LogTemp, Error, TEXT("Session interface is not valid."));
		}
	}
	else
	{
		// Online subsystem not available
		UE_LOG(LogTemp, Error, TEXT("Online subsystem is not available."));
	}
}

void UMultiplayerSessionsSubsystem::GoToNextLevel(const int index)
{
	SetMapIndex(index);

	UWorld* world = GetWorld();
	if (world)
	{
		world->GetAuthGameMode()->bUseSeamlessTravel = true;
		world->ServerTravel(*gameMapsName[mapIndex]);
	}
}
