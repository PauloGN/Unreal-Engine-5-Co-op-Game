// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/OverheadWidget.h"

#include "OnlineSubsystem.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (displayText)
	{
		FString playerName = TextToDisplay;
		const IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get();
		if (onlineSubsystem)
		{
			// it could be any online subsystem service out there (NULL, steam, facebook, google play, etc.)
			const FString subsystemName = onlineSubsystem->GetSubsystemName().ToString();
			const bool bIsLanConnection = subsystemName == FString("Steam") ? false : true;
			if (bIsLanConnection)
			{
				const int textLength = TextToDisplay.Len();
				playerName = "";

				if(textLength >= 3)
				{
					for (int i = 0; i < 3; ++i)
					{
						playerName += TextToDisplay[i];
					}
					playerName += "-";
					for (int i = 0; i < 2; ++i)
					{
						playerName += TextToDisplay[textLength - 1 - i];
					}
				}else
				{
					playerName += TextToDisplay[0];
				}
			}
		}

		displayText->SetText(FText::FromString(playerName));
	}
}

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}

void UOverheadWidget::SetPlayerNameTimer(APawn* InPawn)
{
	FTimerDelegate TryAgainDelegate;
	TryAgainDelegate.BindUFunction(this, FName("ShowPlayerName"), InPawn);
	GetWorld()->GetTimerManager().SetTimer(GetPlayerStateTimer, TryAgainDelegate, GetPlayerNameInterval, false, 0.1f);

	TotalTime += GetPlayerNameInterval;
}

void UOverheadWidget::ShowPlayerName(APawn* InPawn)
{
	if (InPawn == nullptr)
	{
		return;
	}

	const APlayerState* PlayerState = InPawn->GetPlayerState();
	if (!PlayerState || !*PlayerState->GetPlayerName() && TotalTime < GetPlayerNameTimeout)
	{
		SetPlayerNameTimer(InPawn);
		return;
	}

	const FString PlayerName = InPawn->GetPlayerState()->GetPlayerName();
	SetDisplayText(PlayerName);
}
