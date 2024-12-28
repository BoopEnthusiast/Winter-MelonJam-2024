// Copyright Epic Games, Inc. All Rights Reserved.

#include "Winter_MelonJamCharacter.h"
#include "Winter_MelonJamProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AWinter_MelonJamCharacter

AWinter_MelonJamCharacter::AWinter_MelonJamCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	UpdatePolarity();
}

//////////////////////////////////////////////////////////////////////////// Input

void AWinter_MelonJamCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* const PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}


void AWinter_MelonJamCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AWinter_MelonJamCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AWinter_MelonJamCharacter::Look);

		// Changing Polarity
		EnhancedInputComponent->BindAction(BluePolarityAction, ETriggerEvent::Started, this, &AWinter_MelonJamCharacter::BluePolarity);
		EnhancedInputComponent->BindAction(RedPolarityAction, ETriggerEvent::Started, this, &AWinter_MelonJamCharacter::RedPolarity);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AWinter_MelonJamCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D const MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}


void AWinter_MelonJamCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D const LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}


void AWinter_MelonJamCharacter::BluePolarity(const FInputActionValue& Value)
{
	if (Polarity == EPolarity::Positive)
		Polarity = EPolarity::Neutral;
	else
	{
		Polarity = EPolarity::Positive;
	}
	UpdatePolarity();
}


void AWinter_MelonJamCharacter::RedPolarity(const FInputActionValue& Value)
{
	if (Polarity == EPolarity::Negative)
		Polarity = EPolarity::Neutral;
	else
	{
		Polarity = EPolarity::Negative;
	}
	UpdatePolarity();
}


void AWinter_MelonJamCharacter::UpdatePolarity()
{
	if (!Mesh1P) return;
	switch (Polarity)
	{
	case EPolarity::Positive:
		Mesh1P->SetMaterial(0, BlueMaterial);
		break;
	case EPolarity::Negative:
		Mesh1P->SetMaterial(0, RedMaterial);
		break;
	case EPolarity::Neutral:
		Mesh1P->SetMaterial(0, GreyMaterial);
		break;
	}
	UpdateCrosshairPolarity();
}


void AWinter_MelonJamCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	if (PrevMovementMode == MOVE_Walking)
	{
		if (GetWorldTimerManager().IsTimerActive(RespawnSetting))
		{
			GetWorldTimerManager().ClearTimer(RespawnSetting);
			UE_LOG(LogTemp, Warning, TEXT("Timer Stopped"));
		}	
	}
	else if (PrevMovementMode == MOVE_Falling)
	{
		GetWorldTimerManager().SetTimer(
			RespawnSetting,
			this,
			&AWinter_MelonJamCharacter::RespawnSettingStopped,
			1.0f,
			false
		);
	}
}


void AWinter_MelonJamCharacter::RespawnSettingStopped()
{
	RespawnTransform = GetTransform();
}


void AWinter_MelonJamCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	RespawnTransform = GetTransform();
}



void AWinter_MelonJamCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (GetTransform().GetLocation().Z < -300)
	{
		Respawn();
	}
}


void AWinter_MelonJamCharacter::Respawn()
{
	SetActorTransform(RespawnTransform);
	GetCharacterMovement()->Velocity = FVector::ZeroVector;
}