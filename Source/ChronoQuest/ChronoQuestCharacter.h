// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ChronoQuestCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UPushComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AChronoQuestCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Interaction Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Interaction;

	/** WalkRun Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* WalkRun;

	/** WalkRun Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Action;

public:
	AChronoQuestCharacter();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for Jump input */
	void GoWalk();

	/** Called for Jump input */
	void GoRun();

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for Jump input */
	virtual  void Jump () override;

	/** Called for Interaction input */
	void IA_Interaction(const FInputActionValue& Value);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay() override;

public:
	/** Returns CameraBoom subObject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subObject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	virtual void Tick(float DeltaSeconds) override;

#pragma region PUSHING

	////////////////////////// Pushable Objects
	/**Search and interaction/radius range*/
	float PushRange;

	UPROPERTY(replicated, EditAnywhere, BlueprintReadOnly, Category = "Interactions")
	UPushComponent* PushComponent;

	UFUNCTION(Server, Reliable)
	void ServerSetActorTransform(FTransform CharacterPushTransform);

	UFUNCTION()
	void ClientSetActorTransform(FTransform CharacterPushTransform);

private:

	void SphereInteraction();

#pragma endregion

#pragma region Walk Run

	UFUNCTION(Server, Reliable, WithValidation)
	void SERVERRPC_SetWalkSpeed(const float Speed);

	void SetSpeed(const float Speed);

	UPROPERTY(EditAnywhere, Category = "Character Properties")
	float WalkSpeed = 100;

	UPROPERTY(EditAnywhere, Category = "Character Properties")
	float RunSpeed = 700;

	bool bIsWalking = false;

#pragma endregion

#pragma region RPC

	//UFUNCTION(BlueprintCallable)
	//void SpawnSphere();
	///**
	// * \RPC - Remote Procedure Calls And Static Mesh
	// */

	//UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	//void ServerRPCFunction(const int num);

	////Client RPC
	//UFUNCTION(Client, Reliable, BlueprintCallable)
	//void ClientRPCCall();

	////Multicast RPC
	//FTimerHandle testTimer;
	//UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	//void MulticastRPCExplode();

	////Holder for assets
	//UPROPERTY(EditAnywhere)
	//UParticleSystem* explosionEffect;

	//UPROPERTY(EditAnywhere)
	//UParticleSystem* SmokeEffect;

	//UPROPERTY(EditAnywhere)
	//UStaticMesh* sphereMesh;

#pragma endregion

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* overHeadWidget = nullptr;

	UPROPERTY(Replicated, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bStartAction;
};
