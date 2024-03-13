// Fill out your copyright notice in the Description page of Project Settings.

#include "SessionsHandle/MultiplayerSessionsSubsystem.h"

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

	PrintString(TEXT("Initialized"));
}

void UMultiplayerSessionsSubsystem::Deinitialize()
{
	Super::Deinitialize();

	UE_LOG(LogTemp, Warning, TEXT("Deinitialize"));
}
