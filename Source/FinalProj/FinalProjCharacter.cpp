// Copyright Epic Games, Inc. All Rights Reserved.

#include "FinalProjCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BrushComponent.h" 
#include "Components/ModelComponent.h"
#include "Engine/Brush.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "FinalProj.h"
#define DELAY(time, block)\
{\
FTimerHandle TimerHandle;\
GetWorld()->GetTimerManager().SetTimer(TimerHandle, [=]()block, time, false);\
}


AFinalProjCharacter::AFinalProjCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
}

void AFinalProjCharacter::BeginPlay() {
	Super::BeginPlay();
	if (ISMReference)
	{
		ISMReference->NumCustomDataFloats = 4;
	}
	/*
	TArray<AActor*> BrushActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABrush::StaticClass(), BrushActors);
	for (AActor* A : BrushActors)
	{
		TArray<UBrushComponent*> Comps;
		A->GetComponents<UBrushComponent>(Comps);
		for (UBrushComponent* C : Comps)
		{
			C->SetHiddenInGame(true, true);
			C->SetVisibility(false, true);
			C->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		}
	}
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);
	for (AActor* A : AllActors)
	{
		TArray<UModelComponent*> Models;
		A->GetComponents<UModelComponent>(Models);
		for (UModelComponent* MC : Models)
		{
			MC->SetHiddenInGame(true, true);
			MC->SetVisibility(false, true);
			MC->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

	}
	*/
}

void AFinalProjCharacter::AddPoints() {
	const bool bIsTimerValid = GetWorldTimerManager().IsTimerActive(ScanTimer);
	float ElapsedTime = 0.0f;
	if (bIsTimerValid) {
		ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ScanTimer);
	}
	float ClampedTime = FMath::Clamp(ElapsedTime, 0.02f, ScanDuration);
	if (bIsTimerValid) {
		InstanceArray.Add(ClampedTime);
	}
	else {
		InstanceArray.Add(ScanDuration);
	}
}
/*
void AFinalProjCharacter::TimerComplete() {
	const float Elapsed = GetWorldTimerManager().GetTimerElapsed(ScanTimer);
	if ((InstanceArray[0] - Elapsed) <= 0.021f) {
		for (int32 k = 0; k < PointsPerFrame; ++k) {
			const int32 Count = ISMReference->GetInstanceCount();
			if (Count <= 0) {
				break;
			}
			ISMReference->RemoveInstance(0);
		}
		if (InstanceArray.Num() > 0) {
			InstanceArray.RemoveAt(0);
		}
	}
	else {
		InstanceArray[0] = InstanceArray[0] - Elapsed;
	}
	if (InstanceArray.Num() == 0) {
		GetWorldTimerManager().ClearTimer(ScanTimer);
		return;
	}
	GetWorldTimerManager().SetTimer(ScanTimer,this,&AFinalProjCharacter::TimerComplete,InstanceArray[0],true);
}
*/

void AFinalProjCharacter::TimerComplete() {
	UWorld* World = GetWorld();
	if (!World || !ISMReference) {
		GetWorldTimerManager().ClearTimer(ScanTimer);
		return;
	}
	if (ScanDuration <= 0.f) {
		GetWorldTimerManager().ClearTimer(ScanTimer);
		return;
	}
	const float Dt = DecayTickInterval > 0.f ? DecayTickInterval : 0.05f;
	for (int32 i = InstanceLifetimes.Num() - 1; i >= 0; --i) {
		InstanceLifetimes[i] -= Dt;
		if (InstanceLifetimes[i] <= 0.f) {
			if (ISMReference->GetInstanceCount() > i) {
				ISMReference->RemoveInstance(i);
			}
			InstanceLifetimes.RemoveAt(i);
		}
	}
	if (InstanceLifetimes.Num() == 0 || ISMReference->GetInstanceCount() == 0) {
		GetWorldTimerManager().ClearTimer(ScanTimer);
	}
}

void AFinalProjCharacter::MakeStartEnd(FVector& OutStart, FVector& OutEnd) const {
	FVector CameraLocation;
	FRotator CameraRotation;
	if (const UCameraComponent* Cam = FindComponentByClass<UCameraComponent>()) {
		CameraLocation = Cam->GetComponentLocation();
		CameraRotation = Cam->GetComponentRotation();
	}
	const FVector Forward = CameraRotation.Vector();
	const FVector Right = CameraRotation.RotateVector(FVector::RightVector);
	const FVector Up = CameraRotation.RotateVector(FVector::UpVector);
	//use chatgpt to help determine a value(0.4) to clamp down the render width
	const float XYRange = FMath::Sin(FMath::DegreesToRadians(ScannerAngle)) * ScannerRange * 0.4;
	const float RandX = FMath::FRandRange(-1.0, 1.0);
	const float RandY = FMath::FRandRange(-1.0, 1.0);
	OutStart = CameraLocation + Right * (RandX * XYRange) + Up * (RandY * XYRange);
	OutEnd = CameraLocation + Forward * ScannerRange + Right * (RandX * XYRange) + Up * (RandY * XYRange);
}

void AFinalProjCharacter::HideAllBSPAndModels()
{
	UWorld* World = GetWorld();
	if (!World) return;
	{
		TArray<AActor*> BrushActors;
		UGameplayStatics::GetAllActorsOfClass(World, ABrush::StaticClass(), BrushActors);

		for (AActor* A : BrushActors)
		{
			if (!IsValid(A)) continue;

			TArray<UBrushComponent*> BrushComps;
			A->GetComponents<UBrushComponent>(BrushComps);

			for (UBrushComponent* BC : BrushComps)
			{
				if (!IsValid(BC)) continue;
				BC->SetHiddenInGame(true, true);
				BC->SetVisibility(false, true);
			}
			A->SetActorHiddenInGame(true);
		}
	}

	const TArray<ULevel*>& Levels = World->GetLevels();
	for (ULevel* Lvl : Levels)
	{
		if (!IsValid(Lvl)) continue;

		for (UModelComponent* MC : Lvl->ModelComponents)
		{
			if (!IsValid(MC)) continue;
			MC->SetHiddenInGame(true, true);
			MC->SetVisibility(false, true);
		}
	}

}


void AFinalProjCharacter::ScanArea() {
	if (!ISMReference) return;
	if (!bBSPInitialized) {
		// HideAllBSPAndModels();
		bBSPInitialized = true;
	}
	for (int32 i = 0; i < PointsPerFrame; ++i) {
		FVector Start, End;
		MakeStartEnd(Start, End);
		FHitResult Hit;
		FCollisionQueryParams Params(SCENE_QUERY_STAT(ScanArea), true, this);
		Params.AddIgnoredActor(this);
		bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
		bool bHitEnemy = false;
		if (UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(Hit.GetComponent())) {
			Params.AddIgnoredComponent(Capsule);
			FHitResult Hit2;
			bool bHit2 = GetWorld()->LineTraceSingleByChannel(
				Hit2, Start, End, ECC_Visibility, Params
			);

			if (bHit2 && Hit2.bBlockingHit) {
				Hit = Hit2;
				bHit = true;
			}
			else {
				bHit = false;
			}
		}
		//if (AActor* HitActor = Hit.GetActor()) bHitEnemy = true;
		if (bHit && Hit.bBlockingHit) {
			UPrimitiveComponent* HitComp = Hit.GetComponent();
			AActor* HitActor = Hit.GetActor();
			if (USkeletalMeshComponent* Skeleton = Cast<USkeletalMeshComponent>(HitComp)) {
				bHitEnemy = true;
			}
			bool bLongDots = false;
			bool isBlue = false;
			bool isText = false;
			if (HitActor && HitActor->ActorHasTag(FName("LongDots")))
			{
				bLongDots = true;
			}
			if (HitActor && HitActor->ActorHasTag(FName("Blue")))
			{
				isBlue = true;
			}
			if (HitActor && HitActor->ActorHasTag(FName("Text")))
			{
				isText = true;
			}
			FTransform T;
			T.SetLocation(Hit.ImpactPoint);
			int32 NewIndex = ISMReference->AddInstanceWorldSpace(T);
			if (NewIndex >= 0) {
				FVector Color = FVector(1.f, 1.f, 1.f);
				if (InstanceLifetimes.Num() <= NewIndex) {
					InstanceLifetimes.SetNum(NewIndex + 1);
				}
				if (!bHitEnemy) {
					InstanceLifetimes[NewIndex] = ScanDuration;

				}
				else {
					if (bLongDots) {
						InstanceLifetimes[NewIndex] = ScanDuration;
					}
					else {
						InstanceLifetimes[NewIndex] = 3;
					}
					Color = FVector(1.f, 0.f, 0.f);
				}
				if (isBlue) {
					Color = FVector(0.f, 0.f, 1.f);
				}
				if (isText) {
					Color = FVector(1.f, 0.f, 0.f);
				}
				ISMReference->SetCustomDataValue(NewIndex, 0, Color.X, true);
				ISMReference->SetCustomDataValue(NewIndex, 1, Color.Y, true);
				ISMReference->SetCustomDataValue(NewIndex, 2, Color.Z, true);
				float flag = isText ? 1.0f : 0.0f;
				ISMReference->SetCustomDataValue(NewIndex, 3, flag, true);
			}
		}
	}
	if (!GetWorldTimerManager().IsTimerActive(ScanTimer)) {
		const float Interval = DecayTickInterval > 0.f ? DecayTickInterval : 0.05f;
		GetWorldTimerManager().SetTimer(ScanTimer, this, &AFinalProjCharacter::TimerComplete, Interval, true);
	}
}

void AFinalProjCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AFinalProjCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AFinalProjCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFinalProjCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFinalProjCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AFinalProjCharacter::LookInput);
	}
	else
	{
		UE_LOG(LogFinalProj, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AFinalProjCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void AFinalProjCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void AFinalProjCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AFinalProjCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void AFinalProjCharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void AFinalProjCharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}