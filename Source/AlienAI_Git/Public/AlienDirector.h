// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"

#include "DirectorInteractInterface.h"

#include "AlienDirector.generated.h"

//EventDispacher que sirve para avisar al alien para dar un respiro al jugador y alejarse
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDirectorStateChanged, bool, bShouldRetreat, FVector, RetreatLocation);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDirectorHintGiven, FVector, HintLocation);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDirectorHintExpired);


UCLASS()
class ALIENAI_GIT_API AAlienDirector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAlienDirector();

	UPROPERTY(BlueprintAssignable, Category = "AI Director | Events")
	FOnDirectorStateChanged OnDirectorStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "AI Director | Events")
	FOnDirectorHintGiven OnDirectorHintGiven;

	UPROPERTY(BlueprintAssignable, Category = "Director | Events")
	FOnDirectorHintExpired OnDirectorHintExpired;

	// Función para limpiar el timer si el Alien llega antes de tiempo
	UFUNCTION(BlueprintCallable, Category = "Director | Hints")
	void ClearHintTimer();

	UFUNCTION(BlueprintCallable, Category = "Director | Hints")
	float GetTension() const { return fCurrentTension; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//VARIBLES DE ESTADO------------------------------------------
	
	//Valor entre 0-100
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Director | Stats") 
	float fCurrentTension;	

	//Nivel en el que el alien se retira
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI Director | Settings") 
	float fMaxTension;		

	//VARIABLES DE CONFIGURACIÓN------------------------------------------

	//Cuanto sube con alien cerca
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI Director | Settings") 
	float fTensionIncreaseRate;	

	//Cuanto baja con alien lejos
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI Director | Settings") 
	float fTensionDecreaseRate;	

	//Intervalo entre pistas
	UPROPERTY(EditAnywhere, Category = "AI Director | Settings")
	float HintInterval;

	//Radio de error para dar la pista;
	UPROPERTY(EditAnywhere, Category = "AI Director | Settings")
	float HintRadius;

	UPROPERTY(EditAnywhere, Category = "AI Director | Settings")
	float HintBannedRadius;

	//Tiempo que tarda en comenzar el alien desde el inicio;
	UPROPERTY(EditAnywhere, Category = "AI Director | Settings")
	float HintStartDelay;

	UPROPERTY(EditAnywhere, Category = "AI Director | Settings")
	float FlashlightNoise;

	//REFERENCIAS------------------------------------------

	UPROPERTY(BlueprintReadWrite, Category = "AI Director | References")
	AActor* PlayerRef;

	UPROPERTY(BlueprintReadWrite, Category = "AI Director | References")
	AActor* AlienRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI Director | Settings")
	bool bIsCoolingDown; //Alien da un respiro

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI Director | Settings")
	float fCooldownDuration; //Cuento tiempo esta lejos el alien

	UPROPERTY(EditAnywhere, Category = "AI Director | Settings")
	float FlashlightNoiseInterval;

	// Tiempo máximo que el Alien tiene para llegar a la pista
	UPROPERTY(EditAnywhere, Category = "Director | Config | Hints")
	float fHintTimeoutDuration = 15.0f;

	float LastFlashlightNoiseTime;


	void StartCooldown();
	void EndCooldown();
	void CheckLinternaAlien(float DeltaTime);
	void ExpireHint();

	//TIMERS------------------------------------------

	FTimerHandle CooldownTimerHandle;
	FTimerHandle HintTimeoutTimerHandle;
	FTimerHandle HintTimerHandle;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FVector GetBestRetreatLocation();
	void SendHintToAlien();

	bool VisionDirecta;
};
