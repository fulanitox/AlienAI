// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "BP_Lever.generated.h"

UCLASS()
class ALIENAI_GIT_API ABP_Lever : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABP_Lever();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TurningOnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TurningOffSpeed;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float LeverLevel;

	UFUNCTION(BlueprintCallable)
	float TurningOnLever();

	UFUNCTION(BlueprintCallable)
	float TurningOffLever();

	UFUNCTION(BlueprintCallable)
	void UpPalanca();

	UFUNCTION(BlueprintCallable)
	void LowPalanca();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool State50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool State100;
	float FullOpen;
	float HalfOpen;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
