// Copyright Epic Games, Inc. All Rights Reserved.

#include "Winter_MelonJamPickUpComponent.h"

UWinter_MelonJamPickUpComponent::UWinter_MelonJamPickUpComponent()
{
	// Setup the Sphere Collision
	SphereRadius = 32.f;
}

void UWinter_MelonJamPickUpComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register our Overlap Event
	OnComponentBeginOverlap.AddDynamic(this, &UWinter_MelonJamPickUpComponent::OnSphereBeginOverlap);
}

void UWinter_MelonJamPickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Checking if it is a First Person Character overlapping
	AWinter_MelonJamCharacter* Character = Cast<AWinter_MelonJamCharacter>(OtherActor);
	if(Character != nullptr)
	{
		// Notify that the actor is being picked up
		OnPickUp.Broadcast(Character);

		// Unregister from the Overlap Event so it is no longer triggered
		OnComponentBeginOverlap.RemoveAll(this);
	}
}
