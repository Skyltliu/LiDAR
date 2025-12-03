// Copyright Epic Games, Inc. All Rights Reserved.


#include "FinalProjGameMode.h"
#include "Engine/World.h"
#include "EngineUtils.h"                 // TActorIterator
#include "Kismet/GameplayStatics.h"
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


void AFinalProjGameMode::HideVegetation() {
	UWorld* World = GetWorld();
	if (!World) return;
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsWithTag(World, FName("Vegetation"), Actors);
	for (AActor* A : Actors) {
		if (!A) continue;
		A->SetActorHiddenInGame(true);
		A->SetActorEnableCollision(true);
	}
}

void AFinalProjGameMode::HideText() {
	UWorld* World = GetWorld();
	if (!World) return;
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsWithTag(World, FName("Text"), Actors);
	for (AActor* A : Actors) {
		if (!A) continue;
		A->SetActorHiddenInGame(true);
		A->SetActorEnableCollision(true);
	}
}

