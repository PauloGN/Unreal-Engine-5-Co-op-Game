// Copyright Epic Games, Inc. All Rights Reserved.

#include "ChronoQuestCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"
#include <Engine/StaticMeshActor.h>

#include "Actors/PushableActor.h"
#include "Components/WidgetComponent.h"
#include "Interactions/InteractInterface.h"
#include "Interactions/PushComponent.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AChronoQuestCharacter

AChronoQuestCharacter::AChronoQuestCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	overHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PlayerName"));
	overHeadWidget->SetupAttachment(RootComponent);

	GetCharacterMovement()->SetIsReplicated(true);
	GetCharacterMovement()->ForceReplicationUpdate();

#pragma region PUSH

	PushComponent = CreateDefaultSubobject<UPushComponent>("PushComponent");

#pragma endregion

}

void AChronoQuestCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AChronoQuestCharacter, bStartAction);
	DOREPLIFETIME(AChronoQuestCharacter, bIsInteracting);
	DOREPLIFETIME(AChronoQuestCharacter, bCanPushObj);
}

void AChronoQuestCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			PushRange = PushComponent->PushRange;
		}
	}
}

void AChronoQuestCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void AChronoQuestCharacter::PushingAnimSettings()
{



}

void AChronoQuestCharacter::ServerSetActorTransform_Implementation(FTransform CharacterPushTransform)
{
	SetActorTransform(CharacterPushTransform, false, nullptr, ETeleportType::TeleportPhysics);
}

void AChronoQuestCharacter::SphereInteraction()
{
	// Get a reference to the world
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    // Define the sphere parameters
	const float HalthZheight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    FVector SphereLocation = GetActorLocation(); // Check overlap around the character
	SphereLocation.Z -= HalthZheight;

    // Define object types you want to detect overlap with
    FCollisionObjectQueryParams ObjectTypes;
	ObjectTypes.AddObjectTypesToQuery(ECollisionChannel::ECC_Visibility); // Example: Detect overlap with Visibility
	ObjectTypes.AddObjectTypesToQuery(ECollisionChannel::ECC_PhysicsBody); // Example: Detect overlap with physics bodies

    // Define response parameters
    FCollisionQueryParams CollisionParams;
    CollisionParams.bReturnPhysicalMaterial = false; // Optionally return physical material

    // Perform the overlap check
    TArray<FOverlapResult> OverlapResults;
    World->OverlapMultiByObjectType(
        OverlapResults,
        SphereLocation,
        FQuat::Identity,
        ObjectTypes,
        FCollisionShape::MakeSphere(PushRange),
        CollisionParams
    );

    // Process overlap results
    for (const FOverlapResult& OverlapResult : OverlapResults)
    {
        AActor* OverlappingActor = OverlapResult.GetActor();
        // Check if the overlapping actor implements a specific interface
        if (OverlappingActor->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
        {
            // Cast to the interface
			IInteractInterface* YourInterface = Cast<IInteractInterface>(OverlappingActor);
            if (YourInterface)
            {
                // Call interface function
                YourInterface->OnInteracted(this);
				break;
            }
        }
    }

	DrawDebugSphere(GetWorld(), SphereLocation, PushRange, 36, FColor::Green, false, 2.0f);
}

void AChronoQuestCharacter::ServerRPC_Walk_Implementation(const float speed)
{
	GetCharacterMovement()->MaxWalkSpeed = speed;
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, "WALK Speed Called");

	if(speed == WalkSpeed)
	{
		bIsInteracting = true;

		if (PushableActor)
		{
			//PushableActor->Mesh->SetSimulatePhysics(true);
		}
	}else
	{
		bIsInteracting = false;
		if (PushableActor)
		{
			//PushableActor->Mesh->SetSimulatePhysics(false);
		}
	}
}

#pragma region RPC

//void AChronoQuestCharacter::SpawnSphere()
//{
//	if(HasAuthority())
//	{
//		if(sphereMesh == nullptr)
//		{
//			return;
//		}
//		//Ownership
//		FActorSpawnParameters SpawnParameters;
//		SpawnParameters.Owner = this;
//		//Creates and sets actor settings
//		if(AStaticMeshActor* staticMeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(SpawnParameters))
//		{
//			//staticMeshActor->SetOwner();
//			staticMeshActor->SetReplicates(true);
//			staticMeshActor->SetReplicateMovement(true);
//			staticMeshActor->SetMobility(EComponentMobility::Movable);
//
//			const FVector forwardingVector = GetActorLocation() + GetActorRotation().Vector() * 100.0f;
//			const FVector UpwardingdingVector = GetActorUpVector() * 50.0f;
//			const FVector spawnLocation = forwardingVector + UpwardingdingVector;
//
//			staticMeshActor->SetActorLocation(spawnLocation);
//			//Setting up component
//			UStaticMeshComponent* meshComponentFromActor = staticMeshActor->GetStaticMeshComponent();
//			if(meshComponentFromActor)
//			{
//				meshComponentFromActor->SetIsReplicated(true);
//				meshComponentFromActor->SetSimulatePhysics(true);
//				if(sphereMesh != nullptr)
//				{
//					meshComponentFromActor->SetStaticMesh(sphereMesh);
//				}
//			}
//		}
//	}
//}
//
//void AChronoQuestCharacter::ClientRPCCall_Implementation()
//{
//	if(!IsRunningDedicatedServer() && SmokeEffect != nullptr)
//	{
//		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SmokeEffect, GetActorLocation(), FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
//	}
//}
//
//void AChronoQuestCharacter::MulticastRPCExplode_Implementation()
//{
//	if (!IsRunningDedicatedServer() && explosionEffect != nullptr)
//	{
//		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), explosionEffect, GetActorLocation(), FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
//		GetWorld()->GetTimerManager().SetTimer(testTimer, this, &ThisClass::MulticastRPCExplode, 2.0f, false);
//	}
//}
//
////Server RPC should be only executed on the server
////Validation are executed before the implementation if it fails the implementation will not be executed, and the client who called the failed one will be disconnected from the game
//bool AChronoQuestCharacter::ServerRPCFunction_Validate(const int num)
//{
//	return (num < 100 && num >= 0);
//}
//void AChronoQuestCharacter::ServerRPCFunction_Implementation(const int num)
//{
//	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("Server RPC, %d"), num));
//	SpawnSphere();
//}

#pragma endregion

//////////////////////////////////////////////////////////////////////////
// Input

void AChronoQuestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AChronoQuestCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AChronoQuestCharacter::Look);

		// Interaction
		EnhancedInputComponent->BindAction(Interaction, ETriggerEvent::Triggered, this, &AChronoQuestCharacter::IA_Interaction);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AChronoQuestCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AChronoQuestCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AChronoQuestCharacter::IA_Interaction(const FInputActionValue& Value)
{

	SphereInteraction();

	if(bCanPushObj)
	{
		if(bIsInteracting)
		{
			GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
			ServerRPC_Walk(SprintSpeed);
			bIsInteracting = false;

			if(PushableActor)
			{
				//PushableActor->Mesh->SetSimulatePhysics(false);
			}
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
			ServerRPC_Walk(WalkSpeed);
			bIsInteracting = true;

			if (PushableActor)
			{
				//PushableActor->Mesh->SetSimulatePhysics(true);
			}
		}
	}
}
