// Fill out your copyright notice in the Description page of Project Settings.


#include "AlienDirector.h"

// Sets default values
AAlienDirector::AAlienDirector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	fCurrentTension = 0.0f;
	fMaxTension = 100.0f;
	fTensionIncreaseRate = 1.5f;

	fTensionDecreaseRate = 2.0f;
	bIsCoolingDown = false;
	fCooldownDuration = 30.0f;

	//Varaibles para configurar Hints
	HintInterval = 25.0f;
	HintRadius = 1000.0f;
	HintBannedRadius = 200.0f;
	HintStartDelay = 20.0f;

	FlashlightNoiseInterval = 0.5f; // Enviar pista cada medio segundo es suficiente
	LastFlashlightNoiseTime = 0.0f;
	FlashlightNoise = 1.0f;

}

// Called when the game starts or when spawned
void AAlienDirector::BeginPlay()
{
	Super::BeginPlay();
	
	//Buscamos la referencia del PLAYER
	PlayerRef = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (PlayerRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("Director: Jugador encontrado con exito."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Director: ¡NO se pudo encontrar al Jugador!"));
	}

	//Buscamos la referencia del ALIEN
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Alien"), FoundActors);

	if (!FoundActors.IsEmpty())
	{
		AlienRef = FoundActors[0];
		UE_LOG(LogTemp, Warning, TEXT("Director: Alien encontrado con exito."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Director: ¡NO se pudo encontrar al Alien! Asegúrate de que tenga el Tag 'Alien'."));
	}

	//Iniciamos timer de Hints (si metes un float al final se le puede meter un start delay)
	GetWorldTimerManager().SetTimer(HintTimerHandle, this, &AAlienDirector::SendHintToAlien, HintInterval, true, HintStartDelay);
}

// Called every frame
void AAlienDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Volvemos a verificar que tenemos las referencias para evitar crasheos
	if (!AlienRef || !PlayerRef) return;

	//Distancia entre ambos
	float fDistance = FVector::Dist(PlayerRef->GetActorLocation(), AlienRef->GetActorLocation());

	float TensionMultiplier = 1.0f;
	VisionDirecta = false;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(AlienRef);
	Params.AddIgnoredActor(this); // Ignoramos al director si tuviera colisión

	// Ajustamos el rayo para que vaya de "pecho a pecho" y no de "pies a pies"
	FVector StartLoc = AlienRef->GetActorLocation() + FVector(0, 0, 50.f);
	FVector EndLoc = PlayerRef->GetActorLocation() + FVector(0, 0, 50.f);

	bool bHitSomething = GetWorld()->LineTraceSingleByChannel(
		Hit,
		StartLoc,
		EndLoc,
		ECC_Visibility,
		Params
	);
	

	FColor DebugColor = FColor::Red;

	if (bHitSomething)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			GEngine->AddOnScreenDebugMessage(2,0.1f,FColor::Cyan, FString::Printf(TEXT("Chocando con: %s"), *HitActor->GetName()));
			
			if (HitActor == PlayerRef || HitActor->GetOwner() == PlayerRef)
			{
				TensionMultiplier = 4.0f;
				DebugColor = FColor::Green;
				VisionDirecta = true;
			}
		}
	}

	//DrawDebugLine(GetWorld(), StartLoc, EndLoc, DebugColor, false, -1.0f, 0, 2.0f);

	if (fDistance < 2000.0f)
	{
		//Cuanto mas cerca mas rapido sube la tension
		float ProximityFactor = 1.0f - (fDistance / 2000.0f);
		fCurrentTension += (fTensionIncreaseRate * TensionMultiplier * (ProximityFactor + 0.5f)) * DeltaTime;
	}
	else
	{
		//Si esta lejos baja poco a poco
		fCurrentTension -= fTensionDecreaseRate* DeltaTime;
	}

	fCurrentTension = FMath::Clamp(fCurrentTension, 0.0f, fMaxTension);

	if (fCurrentTension >= fMaxTension && !bIsCoolingDown)
	{
		StartCooldown();
	}

	// DEBUG //
	GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Red, FString::Printf(TEXT("Tensión Actual: %.2f"), fCurrentTension));
	CheckLinternaAlien(DeltaTime);

}

FVector AAlienDirector::GetBestRetreatLocation()
{
	TArray<AActor*> PuntosHuida;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("RetreatPoint"), PuntosHuida);

	TPair<AActor*, float> ParejaPunto_Distancia(nullptr, 0.0f);

	for (AActor* Puntos : PuntosHuida)
	{
		float distancia = FVector::Dist(Puntos->GetActorLocation(), PlayerRef->GetActorLocation());
		if (ParejaPunto_Distancia.Value < distancia)
		{
			ParejaPunto_Distancia.Key = Puntos;
			ParejaPunto_Distancia.Value = distancia;
		}
	}

	// Esto es un return con condicion, si existe algun punto de huida envia el mejor, sino, encia un vector 0,0,0;
	return ParejaPunto_Distancia.Key ? ParejaPunto_Distancia.Key->GetActorLocation() : FVector::ZeroVector;
}

void AAlienDirector::StartCooldown() 
{
	bIsCoolingDown = true;

	//Lanzamos el aviso a quien este escuhando que el alien debe alejarse

	OnDirectorStateChanged.Broadcast(true, GetBestRetreatLocation());

	//Log para saber que funciona
	UE_LOG(LogTemp, Warning, TEXT("Director: ¡Tensión máxima! El Alien debe retirarse."));

	//Programamos el "despertar" del Alien tras X segundos
	GetWorldTimerManager().SetTimer(CooldownTimerHandle, this, &AAlienDirector::EndCooldown, fCooldownDuration, false);
}

void AAlienDirector::EndCooldown()
{
	bIsCoolingDown = false;
	fCurrentTension = 0.0f;

	// Lanzamos el evento: "¡Ya pueden volver a la normalidad!"
	OnDirectorStateChanged.Broadcast(false, FVector::ZeroVector);
	SendHintToAlien(); //Envia una pista nada mas terminar de ir al descanso ara que vuelva a acechar por la zona.
}

void AAlienDirector::SendHintToAlien()
{
	if (!PlayerRef || bIsCoolingDown) return; //Si esta en cooldown no se dan pistas

	FVector PlayerLocation = PlayerRef->GetActorLocation();
	FVector RandomOffset = FMath::VRand();	//Genero numero aleatorio
	RandomOffset.Z = 0;						//Quito la altura
	RandomOffset.Normalize();				//Normalizado a 1

	float RandomDist = FMath::FRandRange(HintBannedRadius, HintRadius);	//Calcula el putno alrededor dejando el donut
	FVector RawLocation = PlayerLocation + (RandomOffset * RandomDist);	//Calculamos el putndo en una esfera nuestra

	FVector FinalHintLocation;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (NavSys)
	{
		FNavLocation ProjectedLocation;	//Se utiliza para biscar el punto mas cercano en el NavMesh
		if (NavSys->ProjectPointToNavigation(RawLocation, ProjectedLocation, FVector(500.f, 500.f, 500.f)))
		{
			FinalHintLocation = ProjectedLocation.Location;

			OnDirectorHintGiven.Broadcast(FinalHintLocation);
			// Si ya había un timer anterior, SetTimer lo reiniciará automáticamente
			GetWorldTimerManager().SetTimer(
				HintTimeoutTimerHandle,
				this,
				&AAlienDirector::ExpireHint,
				fHintTimeoutDuration,
				false
			);

			DrawDebugSphere(GetWorld(), FinalHintLocation, 100.0f, 12, FColor::Magenta, false, 8.0f);

			UE_LOG(LogTemp, Warning, TEXT("Director: Pista enviada a la posición %s"), *FinalHintLocation.ToString());
		}
		//Sino hay posicoin valida no se envia nada el alien no se pone a buscar nuevas posiciones de pistas
	}
}

void AAlienDirector::CheckLinternaAlien(float DeltaTime)
{
	bool bIsFlashlightOn = false;
	if (PlayerRef->GetClass()->ImplementsInterface(UDirectorInteractInterface::StaticClass()))
	{
		// Usamos Execute_ seguido del nombre de la función para llamar a Blueprints desde C++
		bIsFlashlightOn = IDirectorInteractInterface::Execute_IsFlashLightActive(PlayerRef);
	}
	if (bIsFlashlightOn && VisionDirecta)
	{
		//Calculo para saber si apuntamos al alien
		FVector PlayerForward = PlayerRef->GetActorForwardVector();
		FVector DirectionToAlien = AlienRef->GetActorLocation() - PlayerRef->GetActorLocation();
		DirectionToAlien.Normalize();

		//DotProduct: 1 mirando directo; 0 perpendicular; -1 de espaladas
		float DotToAlien = FVector::DotProduct(PlayerForward, DirectionToAlien);
		float fDistance = FVector::Dist(PlayerRef->GetActorLocation(), AlienRef->GetActorLocation());

		if (DotToAlien > 0.96f && fDistance < 1500.0f)
		{
			fCurrentTension += 20.0f * DeltaTime;

			float CurrentTime = GetWorld()->GetTimeSeconds();

			if (CurrentTime - LastFlashlightNoiseTime >= FlashlightNoiseInterval)
			{
				LastFlashlightNoiseTime = CurrentTime;
				PlayerRef->MakeNoise(FlashlightNoise, Cast<APawn>(GetOwner()), PlayerRef->GetActorLocation());


				GEngine->AddOnScreenDebugMessage(3, 0.1f, FColor::Yellow, TEXT("DIRECTOR: Alumbrando al Alien!"));
				//DrawDebugSphere(GetWorld(), PlayerRef->GetActorLocation(), 120.0f, 12, FColor::Purple, false, 2.0f);

				// Dibujamos un rayo de luz debug para estar seguros
				DrawDebugLine(GetWorld(), PlayerRef->GetActorLocation(), AlienRef->GetActorLocation(), FColor::Yellow, false, -1.f, 0, 5.f);
			}
		}
	}
}

void AAlienDirector::ExpireHint()
{
	// Avisamos a los Blueprints: olvida la pista, ha pasado demasiado tiempo
	OnDirectorHintExpired.Broadcast();

	UE_LOG(LogTemp, Log, TEXT("Director: La pista ha caducado por tiempo."));
}

void AAlienDirector::ClearHintTimer()
{
	if (GetWorldTimerManager().IsTimerActive(HintTimeoutTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(HintTimeoutTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("Director: Pista completada. Timer cancelado."));
	}
}