// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/MovableActor.h"

AMovableActor::AMovableActor()
{
	PrimaryActorTick.bCanEverTick = true;

	if (HasAuthority())
	{
		bReplicates = true;
		AActor::SetReplicateMovement(true);

		rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
		SetRootComponent(rootComp);

		Point1 = CreateDefaultSubobject<UArrowComponent>(TEXT("Point 1"));
		Point1->SetupAttachment(rootComp);
		Point1->SetRelativeLocation(FVector::Zero());

		Point2 = CreateDefaultSubobject<UArrowComponent>(TEXT("Point 2"));
		Point2->SetupAttachment(rootComp);
		Point2->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));

		mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
		mesh->SetupAttachment(rootComp);
		mesh->SetIsReplicated(true);

		//actor component does not have a transform
		transportComponent = CreateDefaultSubobject<UCPP_Transporter>(TEXT("Trasport Component"));
	}

}

void AMovableActor::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority())
	{
		// Get the actor's transform
		const FTransform ActorTransform = GetActorTransform();

		// Transform the relative locations of the points to world space
		const FVector Start = ActorTransform.TransformPosition(Point1->GetRelativeLocation());
		const FVector End = ActorTransform.TransformPosition(Point2->GetRelativeLocation());

		transportComponent->SetPoints(Start, End);
		transportComponent->SetIsReplicated(true);
	}
}

void AMovableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

