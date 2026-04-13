#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DirectorInteractInterface.generated.h"

UINTERFACE(MinimalAPI)
class UDirectorInteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interfaz para que el Director interactúe con el Personaje
 */
class ALIENAI_GIT_API IDirectorInteractInterface
{
	GENERATED_BODY()

public:
	// IMPORTANTE: Los paréntesis () al final la convierten en función
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Director")
	bool IsFlashLightActive();
};