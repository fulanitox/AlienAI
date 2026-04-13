// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_SoundEmitter.h"

// Sets default values for this component's properties
UAC_SoundEmitter::UAC_SoundEmitter()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UAC_SoundEmitter::BeginPlay()
{
	Super::BeginPlay();

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		//Nos suscribimos la evento
		OwnerCharacter->LandedDelegate.AddDynamic(this, &UAC_SoundEmitter::OnOwnerLanded);
	}

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle_Footstep,
		this,
		&UAC_SoundEmitter::PlayFootstep,
		0.3f,
		true
	);
}

void UAC_SoundEmitter::OnOwnerLanded(const FHitResult& Hit)
{
	//Hacemos el make noise
	//Parametros: Loudness, Instigator, Location
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());

	if (bMakeNoise && GetOwner())
	{
		// Añadimos 0.0f (Rango por defecto) y el FName("Moderate")
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), Hit.ImpactPoint, fLandedNoise, OwnerCharacter, 0.0f, FName("Moderate"));
	}
}

void UAC_SoundEmitter::PlayFootstep()
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (bMakeNoise && OwnerCharacter && OwnerCharacter->GetCharacterMovement())
	{
		bool bOnGround = OwnerCharacter->GetCharacterMovement()->IsMovingOnGround();
		bool bMoving = OwnerCharacter->GetVelocity().Size() > fSpeedToSound;

		if (bOnGround && bMoving)
		{
			// Lo mismo aquí: ReportNoiseEvent en lugar de MakeNoise
			UAISense_Hearing::ReportNoiseEvent(GetWorld(), OwnerCharacter->GetActorLocation(), fRunNoise, OwnerCharacter, 0.0f, FName("Moderate"));
		}
	}
}


// Called every frame
void UAC_SoundEmitter::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

