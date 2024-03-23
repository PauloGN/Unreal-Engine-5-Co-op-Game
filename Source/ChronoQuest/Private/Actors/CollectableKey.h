// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollectableKey.generated.h"

UCLASS()
class ACollectableKey : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACollectableKey();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RotationSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class ACollectableKeyHolder* keyHolder;

	UPROPERTY(ReplicatedUsing = OnRep_bIscollected, BlueprintReadWrite, VisibleAnywhere)
	bool bIscollected;

	UFUNCTION()
	void OnRep_bIscollected();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UStaticMeshComponent* mesh;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class UCapsuleComponent* capsule;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class UAudioComponent* collectedCue;
};
