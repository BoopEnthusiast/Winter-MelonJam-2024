#pragma once

#pragma once

#include "CoreMinimal.h"
#include "EPolarity.generated.h"

UENUM(BlueprintType) // Exposes the enum to Blueprints
enum class EPolarity : uint8
{
	Neutral UMETA(DisplayName = "Neutral"),
	Positive UMETA(DisplayName = "Positive"),
	Negative UMETA(DisplayName = "Negative")
};
