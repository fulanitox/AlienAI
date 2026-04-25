// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DA_AlienVisonBalance.h"	// El Data Asset que creamos
#include "DrawDebugHelpers.h"		//Para el debugger
#include "AC_VisionsSystem.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALIENAI_GIT_API UAC_VisionsSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAC_VisionsSystem();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// El "Libro de Recetas" que asignaremos en el Editor
	UPROPERTY(EditAnywhere, Category = "Alien | Vision")
	UDA_AlienVisonBalance* VisionSettings;

	// El valor de 0 a 100 que el Evaluator leerá
	UPROPERTY(BlueprintReadOnly, Category = "Alien | Vision")
	float CurrentDetection = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Alien | Vision")
	FColor CurrentCone = FColor::White;

	UPROPERTY(BlueprintReadOnly, Category = "Alien | Vision")
	bool WatchingMe = true;

	// Interruptor para ver los conos en el juego
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Alien | Vision | Debug")
	bool bDrawDebugCones = false;	

	// Referencia al jugador para no buscarlo cada frame
	UPROPERTY()
	AActor* PlayerRef;

	// ¿Dibujar cuña 2D o cono 3D?
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Alien | Vision | Debug")
	bool bUse2DDebug = true;

	// Color que usará el texto
	FColor ActiveDetectionColor = FColor::White;

	void DrawVisionDebug();

	UPROPERTY(EditAnywhere, Category = "Alien | Vision")
	float DetectionThreshold = 80.0f;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Función para que el Evaluator pueda consultar la detección
	UFUNCTION(BlueprintCallable, Category = "Alien | Vision")
	float GetDetectionValue() const { return CurrentDetection; }


};
