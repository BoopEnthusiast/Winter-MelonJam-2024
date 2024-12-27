// Fill out your copyright notice in the Description page of Project Settings.


#include "PolarityBlock.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Winter_MelonJam/Winter_MelonJamCharacter.h"

// Sets default values
APolarityBlock::APolarityBlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));
	BlockMesh->Mobility = EComponentMobility::Stationary;
	RootComponent = BlockMesh;

	PullingArea = CreateDefaultSubobject<USphereComponent>(TEXT("PullingArea"));
	PullingArea->Mobility = EComponentMobility::Stationary;
	PullingArea->bAutoActivate = true;
	PullingArea->SetupAttachment(BlockMesh);
	PullingArea->SetSphereRadius(MaxPullDistance);
	PullingArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PullingArea->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	PullingArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PullingArea->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	UE_LOG(LogTemp, Warning, TEXT("Setting up listeners"));
	PullingArea->OnComponentBeginOverlap.AddDynamic(this, &APolarityBlock::OnOverlapBegin);
	PullingArea->OnComponentEndOverlap.AddDynamic(this, &APolarityBlock::OnOverlapEnd);
	
	UpdatePolarity();
}

// Called when the game starts or when spawned
void APolarityBlock::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Setting up listeners"));
	PullingArea->OnComponentBeginOverlap.AddDynamic(this, &APolarityBlock::OnOverlapBegin);
	PullingArea->OnComponentEndOverlap.AddDynamic(this, &APolarityBlock::OnOverlapEnd);

	UpdatePolarity();
}

// Called every frame
void APolarityBlock::Tick(float const DeltaTime)
{
	Super::Tick(DeltaTime);

	ApplyPolarityForce(DeltaTime);
}

void APolarityBlock::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
		AffectedPlayers.AddUnique(Character);
}

void APolarityBlock::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
		AffectedPlayers.Remove(Character);
}

void APolarityBlock::ApplyPolarityForce(float DeltaTime)
{
	for (ACharacter* Character : AffectedPlayers)
	{
		AWinter_MelonJamCharacter* PolarityCharacter = Cast<AWinter_MelonJamCharacter>(Character);
		if (!PolarityCharacter)
			continue;

		if (PolarityCharacter->Polarity != Polarity || Polarity == EPolarity::Neutral)
			continue;
		
		FVector Direction = GetActorLocation() - Character->GetActorLocation();
		float Distance = Direction.Size();
		float Radius = PullingArea->GetScaledSphereRadius();

		Direction.Normalize();
		float ForceMagnitude = FMath::Lerp(PullStrength, 0.0f, Distance / Radius);

		
		if (UCharacterMovementComponent* MovementComponent = PolarityCharacter->GetCharacterMovement())
		{
			MovementComponent->AddForce(Direction * ForceMagnitude * DeltaTime * 10000.0);
		}
	}
}


void APolarityBlock::UpdatePolarity()
{
	if (!BlockMesh) return;
	switch (Polarity)
	{
	case EPolarity::Positive:
		BlockMesh->SetMaterial(0, BlueMaterial);
		break;
	case EPolarity::Negative:
		BlockMesh->SetMaterial(0, RedMaterial);
		break;
	case EPolarity::Neutral:
		BlockMesh->SetMaterial(0, GreyMaterial);
		break;
	}
}
