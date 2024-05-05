// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "PushableActor.generated.h"

class UPhysicsConstraintComponent;
class AChronoQuestCharacter;
class UCurveFloat;

UCLASS()
class APushableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APushableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Environment")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = "Environment")
	UPhysicsConstraintComponent* PhysicsComponent;

	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;


private:

	bool CanPush();

	TObjectPtr<AChronoQuestCharacter> Character;

#pragma region TIME LINE


	UPROPERTY(EditAnywhere, Category = "TimeLineProps")
	TObjectPtr<UCurveFloat> CurveFloatAmim;

	FTimeline MoveTimeLine;

	//Functions to bind
	void BindingTimeLine();

	//CallBack
	UFUNCTION()
	void CallBackFunc(float value);

	UFUNCTION()
	void Play();

# pragma endregion
};
