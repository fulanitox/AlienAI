#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_AlienVisonBalance.generated.h" // Este siempre debe ser el último include

// 1. Definimos el Enum aquí en C++. Ahora AMBOS (C++ y BP) podrán usarlo.
UENUM(BlueprintType)
enum class EVisionConeType : uint8
{
	Purple      UMETA(DisplayName = "Purple (Contact)"),
	Red         UMETA(DisplayName = "Red (Focus)"),
	Blue        UMETA(DisplayName = "Blue (Mid)"),
	Green       UMETA(DisplayName = "Green (Wide)")
};

// 2. Definimos la estructura de los datos del cono
USTRUCT(BlueprintType)
struct FVisionConeStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vision")
	float HalfAngle = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vision")
	float Multiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vision")
	float MaxRange = 1500.0f;
};

UCLASS(BlueprintType)
class ALIENAI_GIT_API UDA_AlienVisonBalance : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Ahora el Map usa el Enum que acabamos de definir arriba
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vision Balance")
	TMap<EVisionConeType, FVisionConeStruct> VisionConfigMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Detection")
	float DetectionDecreaseRate = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Detection")
	float GlobalDetectionSensitivity = 1.0f;
};