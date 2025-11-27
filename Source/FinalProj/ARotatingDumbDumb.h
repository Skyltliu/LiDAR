// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ARotatingDumbDumb.generated.h"
class USphereComponent;
UCLASS()
class FINALPROJ_API AARotatingDumbDumb : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AARotatingDumbDumb();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Torus", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TorusMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Torus")
	float RotationSpeed = 45.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Torus", meta = (AllowPrivateAccess = "true"))
	USphereComponent* Collision;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* Sound;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Countl = 0;
};
