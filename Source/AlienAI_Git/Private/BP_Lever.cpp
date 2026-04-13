// Fill out your copyright notice in the Description page of Project Settings.


#include "BP_Lever.h"

// Sets default values
ABP_Lever::ABP_Lever()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TurningOffSpeed = 0.1f;
	TurningOnSpeed = 0.1f;
	LeverLevel = 0.0f;
	FullOpen = 1.0f;
	HalfOpen = 0.5f;


}

// Called when the game starts or when spawned
void ABP_Lever::BeginPlay()
{
	Super::BeginPlay();
	
}

float ABP_Lever::TurningOnLever()
{
	return GetWorld()->GetDeltaSeconds() * TurningOnSpeed;
}

float ABP_Lever::TurningOffLever()
{
	if (State50 && LeverLevel <= HalfOpen)
	{
		return 0;
	}
	return GetWorld()->GetDeltaSeconds() * TurningOffSpeed;
}

void ABP_Lever::UpPalanca()
{
	if (State100) return;

	LeverLevel += TurningOnLever();

	if (LeverLevel >= FullOpen)
	{
		State100 = true;
	}
	else if (LeverLevel >= HalfOpen)
	{
		State50 = true;
	}

	LeverLevel = FMath::Clamp(LeverLevel, 0.0f, FullOpen);
}

void ABP_Lever::LowPalanca()
{
	if (!State100)
	{
		LeverLevel -= TurningOffLever();
		LeverLevel = FMath::Clamp(LeverLevel, 0.0f, FullOpen);
	}
}



// Called every frame
void ABP_Lever::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

