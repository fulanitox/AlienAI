// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_VisionsSystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UAC_VisionsSystem::UAC_VisionsSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UAC_VisionsSystem::BeginPlay()
{
	Super::BeginPlay();
	PlayerRef = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}


// Called every frame
void UAC_VisionsSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 1. SEGURIDAD TOTAL: Si no hay settings o no hay jugador, abortamos misión
	if (!VisionSettings || !PlayerRef || !GetWorld()) return;

	AActor* Alien = GetOwner();
	if (!Alien) return;

	FVector AlienLoc = Alien->GetActorLocation();
	FVector PlayerLoc = PlayerRef->GetActorLocation();

	FVector DirToPlayer = PlayerLoc - AlienLoc;
	float Distance = DirToPlayer.Size();
	DirToPlayer.Normalize();

	float Dot = FVector::DotProduct(Alien->GetActorForwardVector(), DirToPlayer);
	float Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));

	FVisionConeStruct* BestCone = nullptr;
	auto& Map = VisionSettings->VisionConfigMap;

	// 2. Selección de cono (Asegurándonos de que el Map tiene la clave antes de leer)
	if (Map.Contains(EVisionConeType::Purple) && Distance <= Map[EVisionConeType::Purple].MaxRange)
	{
		BestCone = &Map[EVisionConeType::Purple];
		CurrentCone = FColor::Purple;
		ActiveDetectionColor = FColor::Purple;
	}
	else if (Map.Contains(EVisionConeType::Red) && Angle <= Map[EVisionConeType::Red].HalfAngle && Distance <= Map[EVisionConeType::Red].MaxRange)
	{
		BestCone = &Map[EVisionConeType::Red];
		CurrentCone = FColor::Red;
		ActiveDetectionColor = FColor::Red;
	}
	else if (Map.Contains(EVisionConeType::Blue) && Angle <= Map[EVisionConeType::Blue].HalfAngle && Distance <= Map[EVisionConeType::Blue].MaxRange)
	{
		BestCone = &Map[EVisionConeType::Blue];
		CurrentCone = FColor::Blue;
		ActiveDetectionColor = FColor::Blue;
	}
	else if (Map.Contains(EVisionConeType::Green) && Angle <= Map[EVisionConeType::Green].HalfAngle && Distance <= Map[EVisionConeType::Green].MaxRange)
	{
		BestCone = &Map[EVisionConeType::Green];
		CurrentCone = FColor::Green;
		ActiveDetectionColor = FColor::Green;
	}
		

	// 3. Lógica de visibilidad
	if (BestCone)
	{
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Alien);

		if (GetWorld()->LineTraceSingleByChannel(Hit, AlienLoc, PlayerLoc, ECC_Visibility, Params) && Hit.GetActor() == PlayerRef)
		{
			float VisFactor = 1.0f;
			WatchingMe = true;
			ACharacter* P = Cast<ACharacter>(PlayerRef);

			// Comprobación de seguridad para el Cast
			if (P && P->GetCharacterMovement())
			{
				if (P->GetCharacterMovement()->IsCrouching()) VisFactor = 0.5f;
				else if (P->GetVelocity().Size() > 400.0f) VisFactor = 1.5f;
			}

			// FÓRMULA CORREGIDA (Suma, no multiplicación)
			float Gain = (BestCone->Multiplier * VisFactor) / FMath::Max(Distance / 100.0f, 1.0f);
			CurrentDetection += Gain * DeltaTime * 50.0f * VisionSettings->GlobalDetectionSensitivity;
		}
		else { CurrentDetection -= VisionSettings->DetectionDecreaseRate * DeltaTime; }
	}
	else 
	{ 
		CurrentDetection -= VisionSettings->DetectionDecreaseRate * DeltaTime; 
		ActiveDetectionColor = FColor::White;
		WatchingMe = false;
	}

	CurrentDetection = FMath::Clamp(CurrentDetection, 0.0f, 100.0f);

	if (bDrawDebugCones)
	{
		DrawVisionDebug();
	}
}

void UAC_VisionsSystem::DrawVisionDebug()
{
	if (!bDrawDebugCones || !VisionSettings || !GetOwner()) return;

	AActor* Alien = GetOwner();
	FVector Origin = Alien->GetActorLocation();
	FVector Forward = Alien->GetActorForwardVector();
	FVector Up = Alien->GetActorUpVector();

	for (auto& Elem : VisionSettings->VisionConfigMap)
	{
		FVisionConeStruct& Settings = Elem.Value;

		// Asignar color según el tipo
		FColor Color;
		switch (Elem.Key) {
		case EVisionConeType::Purple: Color = FColor::Purple; break;
		case EVisionConeType::Red:    Color = FColor::Red; break;
		case EVisionConeType::Blue:   Color = FColor::Blue; break;
		case EVisionConeType::Green:  Color = FColor::Green; break;
		default:                      Color = FColor::White; break;
		}

		if (bUse2DDebug)
		{
			// MODO CUÑA 2D (Dos líneas laterales)
			// Calculamos los vectores de los límites izquierdo y derecho
			FVector LeftLimit = Forward.RotateAngleAxis(-Settings.HalfAngle, Up);
			FVector RightLimit = Forward.RotateAngleAxis(Settings.HalfAngle, Up);

			DrawDebugLine(GetWorld(), Origin, Origin + (LeftLimit * Settings.MaxRange), Color, false, -1.f, 0, 2.0f);
			DrawDebugLine(GetWorld(), Origin, Origin + (RightLimit * Settings.MaxRange), Color, false, -1.f, 0, 2.0f);

			// Opcional: Una línea curva o arco para cerrar la cuña
			// DrawDebugCircle(...) podría usarse aquí, pero las dos líneas suelen bastar.
		}
		else
		{
			// MODO 3D (El anterior)
			DrawDebugCone(GetWorld(), Origin, Forward, Settings.MaxRange,
				FMath::DegreesToRadians(Settings.HalfAngle), FMath::DegreesToRadians(Settings.HalfAngle),
				12, Color, false, -1.f, 0, 1.0f);
		}
	}

	// EL NÚMERO FLOTANTE (Con el color del cono activo)
	FString DebugText = FString::Printf(TEXT("DETECCION: %.1f %%"), CurrentDetection);

	DrawDebugString(GetWorld(), FVector(0, 0, 160), DebugText, Alien, ActiveDetectionColor, 0.03f, false, 2.0f);
}