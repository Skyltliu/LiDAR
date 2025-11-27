// Copyright Epic Games, Inc. All Rights Reserved.


#include "FinalProjGameMode.h"
#include "Engine/World.h"
#include "EngineUtils.h"                 // TActorIterator
#include "Engine/DirectionalLight.h"     // ADirectionalLight
#include "Engine/SkyLight.h"             // ASkyLight
#include "Components/StaticMeshComponent.h"

AFinalProjGameMode::AFinalProjGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PawnBP(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	if (PawnBP.Succeeded())
	{
		DefaultPawnClass = PawnBP.Class;
	}
}

