// Fill out your copyright notice in the Description page of Project Settings.


#include "ARotatingDumbDumb.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "FinalProjCharacter.h"
// Sets default values
AARotatingDumbDumb::AARotatingDumbDumb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TorusMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TorusMesh"));
	RootComponent = TorusMesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> TorusAsset(TEXT("/Game/StarterContent/Shapes/Shape_Torus.Shape_Torus"));
	TorusMesh->SetStaticMesh(TorusAsset.Object);

	TorusMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TorusMesh->SetCollisionObjectType(ECC_WorldDynamic);
	TorusMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	TorusMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TorusMesh->SetGenerateOverlapEvents(true);

	// Bind overlap callback
	TorusMesh->OnComponentBeginOverlap.AddDynamic(this, &AARotatingDumbDumb::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AARotatingDumbDumb::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AARotatingDumbDumb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FRotator DeltaRot(0.f, 0.f, RotationSpeed * DeltaTime);
	AddActorLocalRotation(DeltaRot);
}

void AARotatingDumbDumb::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (!OtherActor || OtherActor == this) {
		return;
	}
	if (AFinalProjCharacter* Player = Cast<AFinalProjCharacter>(OtherActor)) {
		Player->Count += 1;
		if (Sound) {
			UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation());
		}
		Destroy();
	}
}