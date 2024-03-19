// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/OverheadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (displayText)
	{
		displayText->SetText(FText::FromString(TextToDisplay));
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
