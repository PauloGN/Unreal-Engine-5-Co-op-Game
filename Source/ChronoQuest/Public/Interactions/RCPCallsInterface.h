// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RCPCallsInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URCPCallsInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CHRONOQUEST_API IRCPCallsInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastRPC_SpawnEffects() = 0;
};
