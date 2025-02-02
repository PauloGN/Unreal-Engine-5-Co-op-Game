// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_TriggerPlatform.generated.h"

//Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTriggerActivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTriggerDeactivated);

UCLASS()
class ACPP_TriggerPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACPP_TriggerPlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Default variables
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	USceneComponent* rootComp;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UStaticMeshComponent* triggerMesh;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UStaticMeshComponent* mesh;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool activated;

	//Delegate variables
	UPROPERTY(BlueprintAssignable)
	FOnTriggerActivated OnTriggerActivated;

	UPROPERTY(BlueprintAssignable)
	FOnTriggerDeactivated OnTriggerDeactivated;
	//self move settings
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class UCPP_Transporter* transporter;
};
