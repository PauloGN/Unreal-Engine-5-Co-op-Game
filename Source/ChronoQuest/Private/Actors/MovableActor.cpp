// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/MovableActor.h"

AMovableActor::AMovableActor()
{
	PrimaryActorTick.bCanEverTick = true;

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

void AMovableActor::BeginPlay()
{
	Super::BeginPlay();

	const FVector start = Point1->GetRelativeLocation() + GetActorLocation();
	const FVector end = Point2->GetRelativeLocation() + GetActorLocation();

	transportComponent->SetPoints(start, end);
}

void AMovableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

