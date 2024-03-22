// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actors/CPP_Transporter.h"
#include <Components/ArrowComponent.h>
#include "MovableActor.generated.h"

UCLASS()
class AMovableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	USceneComponent* rootComp;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UArrowComponent* Point1;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UArrowComponent* Point2;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UStaticMeshComponent* mesh;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UCPP_Transporter* transportComponent;
};
