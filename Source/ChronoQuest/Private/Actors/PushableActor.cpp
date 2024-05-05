// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PushableActor.h"

#include "ChronoQuest/ChronoQuestCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

// Sets default values
APushableActor::APushableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
    AActor::SetReplicateMovement(true);
	NetUpdateFrequency = 100;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetIsReplicated(true);

	Mesh->SetSimulatePhysics(true);
	Mesh->SetLinearDamping(5.0f);
	SetRootComponent(Mesh);

	PhysicsComponent = CreateDefaultSubobject<UPhysicsConstraintComponent>("PhysicsConstraint");
	PhysicsComponent->SetIsReplicated(true);

	PhysicsComponent->SetLinearXLimit(LCM_Free, 0.0f);
	PhysicsComponent->SetAngularSwing1Limit(ACM_Locked, 0.0f);
	PhysicsComponent->SetAngularSwing2Limit(ACM_Locked, 0.0f);
	PhysicsComponent->SetAngularTwistLimit(ACM_Locked, 0.0f);

	FConstrainComponentPropName ComponentProName;
	ComponentProName.ComponentName = "Mesh";
	PhysicsComponent->ComponentName1 = ComponentProName;
	PhysicsComponent->SetupAttachment(Mesh);


	Tags.Add("pushable");

}

// Called when the game starts or when spawned
void APushableActor::BeginPlay()
{
	Super::BeginPlay();

	BindingTimeLine();
}

// Called every frame
void APushableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MoveTimeLine.TickTimeline(DeltaTime);
}

void APushableActor::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);


	Character = Cast<AChronoQuestCharacter>(Other);

	if(!Character)
	{
		return;
	}

	const FVector ActorForwardVector = Other->GetActorForwardVector();

	FVector RoundedForwardVector(round(ActorForwardVector.X), round(ActorForwardVector.Y),round(ActorForwardVector.Z));


	FString msg = FString::Printf(TEXT("X: %f Y: %f Z: %f"), RoundedForwardVector.X, RoundedForwardVector.Y, RoundedForwardVector.Z);

	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, *msg);
	if(CanPush())
	{
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);

		Character->GetCharacterMovement()->DisableMovement();
		Character->AttachToActor(this, AttachmentRules);
		Play();
	}
}

void APushableActor::CallBackFunc(float value)
{

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("%f"), value ));
}

void APushableActor::Play()
{
	MoveTimeLine.PlayFromStart();
}

bool APushableActor::CanPush()
{

	if (!Character)
	{
		return false;
	}

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;

	QueryParams.AddIgnoredActor(Character);

	FVector Start = Character->GetActorLocation();
	FVector End = Character->GetActorForwardVector() * 100.0f + Start;

	if(ActorLineTraceSingle(HitResult, Start, End, ECC_Visibility, QueryParams))
	{

		if(HitResult.GetActor() == this)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "Actor Can Push");
			return true;
		}
	}

	return false;
}

void APushableActor::BindingTimeLine()
{
	if(CurveFloatAmim)
	{
		FOnTimelineFloatStatic FloatTimeLimeCallBack;
		FloatTimeLimeCallBack.BindUFunction(this, "CallBackFunc");
		MoveTimeLine.AddInterpFloat(CurveFloatAmim, FloatTimeLimeCallBack);
	}
}
