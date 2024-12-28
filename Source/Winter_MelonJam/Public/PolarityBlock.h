// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "EPolarity.h"
#include "Components/WidgetComponent.h"
#include "PolarityBlock.generated.h"


UCLASS()
class WINTER_MELONJAM_API APolarityBlock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APolarityBlock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* BlockMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* PullingArea;

	// Pull strength
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pull Settings")
	float PullStrength = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pull Settings")
	float MaxPullDistance = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pull Settings")
	EPolarity Polarity = EPolarity::Neutral;

	// Materials
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pull Settings")
	UMaterialInterface* BlueMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pull Settings")
	UMaterialInterface* RedMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pull Settings")
	UMaterialInterface* GreyMaterial;

	// Update Polarity
	UFUNCTION(BlueprintCallable, Category = "Pull Settings")
	void UpdatePolarity();

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, 
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void ApplyPolarityForce(float DeltaTime);
    
	TArray<ACharacter*> AffectedPlayers;
};
