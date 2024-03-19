// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadWidget.generated.h"

/**
 * 
 */
UCLASS()
class UOverheadWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	//Handle player name
	UPROPERTY(EditAnywhere, Category = "Overhead Widget Properties", meta = (AllowPrivateAccess = true, Units = "Seconds"))
	float GetPlayerNameTimeout = 30.f;

	UPROPERTY(EditAnywhere, Category = "Overhead Widget Properties", meta = (AllowPrivateAccess = true, Units = "Seconds"))
	float GetPlayerNameInterval = 0.1f;

	float TotalTime = -0.1f;
	FTimerHandle GetPlayerStateTimer;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* displayText;

	void SetDisplayText(FString TextToDisplay);

protected:

	virtual void NativeDestruct() override;

	//Handle player name
	void SetPlayerNameTimer(APawn* InPawn);

public:
	//Handle player name
	UFUNCTION(BlueprintCallable)
	void ShowPlayerName(APawn* InPawn);

};
