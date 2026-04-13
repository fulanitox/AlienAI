// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/ActorComponent.h"
#include "Perception/AISense_Hearing.h"
#include "AC_SoundEmitter.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UAC_SoundEmitter : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAC_SoundEmitter();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	float fSpeedToSound = 500.0f;

	UPROPERTY(EditAnywhere)
	float fLandedNoise = 1.0f;

	UPROPERTY(EditAnywhere)
	float fRunNoise = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bMakeNoise = true;

	//Funcion para recibir el impacto del aterrizaje con el suelo
	UFUNCTION()
	void OnOwnerLanded(const FHitResult& Hit);

	//Funcion que realizara el custom event
	UFUNCTION()
	void PlayFootstep();

	//Manejador del timer necesario para cerarlo y pararlo
	FTimerHandle TimerHandle_Footstep;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
