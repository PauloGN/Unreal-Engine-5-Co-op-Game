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
class APushableActor;
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

public:
	AChronoQuestCharacter();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for Interaction input */
	void IA_Interaction(const FInputActionValue& Value);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	virtual void Tick(float DeltaSeconds) override;

#pragma region PUSHING

	UPROPERTY(Replicated, BlueprintReadWrite) bool bIsInteracting = false;
	UPROPERTY(Replicated, BlueprintReadWrite) bool bCanPushObj = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SprintSpeed = 600.0f;
	UPROPERTY(EditAnywhere ,BlueprintReadWrite) float WalkSpeed = 100.0f;
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_Walk(const float speed);

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector2D MovementVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TObjectPtr<APushableActor> PushableActor;
	
	UFUNCTION()
	void PushingAnimSettings();

	////////////////////////// Pushable Objects

	/**Search and interaction/radius range*/
	float PushRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactions")
	UPushComponent* PushComponent;

	UFUNCTION(Server, Reliable)
	void ServerSetActorTransform(FTransform CharacterPushTransform);

private:

	void SphereInteraction();

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
