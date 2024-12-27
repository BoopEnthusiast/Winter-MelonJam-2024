// Copyright Epic Games, Inc. All Rights Reserved.

#include "Winter_MelonJamGameMode.h"
#include "Winter_MelonJamCharacter.h"
#include "UObject/ConstructorHelpers.h"

AWinter_MelonJamGameMode::AWinter_MelonJamGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
