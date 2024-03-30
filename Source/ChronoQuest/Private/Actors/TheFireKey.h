// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TheFireKey.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnfireIsLight);

UCLASS()
class ATheFireKey : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATheFireKey();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UStaticMeshComponent* rocks;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UStaticMeshComponent* wood;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UStaticMeshComponent* ash;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class UParticleSystemComponent* smoke;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UParticleSystemComponent* fire;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class UPointLightComponent* ambientLight;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UPointLightComponent* shadowedLight;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class UCapsuleComponent* capsule;

	UPROPERTY(ReplicatedUsing = OnRep_bIsFireOn, BlueprintReadWrite, VisibleAnywhere)
	bool bIsFireOn;

	UFUNCTION()
	void OnRep_bIsFireOn();

	UFUNCTION(Server,Reliable ,BlueprintCallable)
	void LightFire();

	FOnfireIsLight OnFireLights;

};
