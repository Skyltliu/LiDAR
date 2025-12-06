// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "InputActionValue.h" 
#include "FinalProjCharacter.generated.h"
#define DEFAULT_RANGE 1475.0f;
#define DEFAULT_ANGLE 60.0f;
#define DEFAULT_DOTS 1750;
#define DEFAULT_SHORT_RANGE 1000.0f;
#define DEFAULT_SHORT_ANGLE 15.0f;
#define DEFAULT_SHORT_DOTS 500;
#define DEFAULT_LONG_RANGE 2000.0f;
#define DEFAULT_LONG_ANGLE 120.0f;
#define DEFAULT_LONG_DOTS 3000;
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;
class UInstancedStaticMeshComponent;
class UNiagaraSystem;
class UNiagaraComponent;
/**
 *  A basic first person character
 */
UCLASS()
class FINALPROJ_API AFinalProjCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* MouseLookAction;

public:
	AFinalProjCharacter();
	UFUNCTION(BlueprintCallable)
	int32 GetCount() const { return Count; }
	TArray<float> InstanceLifetimes;

	/** How often TimerComplete runs */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scan")
	float DecayTickInterval = 0.5f; // 20 Hz
	FTimerHandle ScanTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lidar")
	int32 PointsPerFrame = 1750;//2200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lidar")
	float ScannerRange = 1475.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lidar")
	float ScannerAngle = 60.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lidar")
	float ScanDuration = 30.0;

	//UPROPERTY(BlueprintReadWrite, Category = "Lidar")
	//FTimerHandle ScanTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lidar")
	UInstancedStaticMeshComponent* ISMReference = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lidar")
	TArray<float> InstanceArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lidar")
	TArray<int32> InstanceCounts;


	UFUNCTION(BlueprintCallable, Category = "Lidar")
	void ScanArea();

protected:
	virtual void BeginPlay() override;

	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoJumpEnd();

protected:

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Count = 0;
	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "LiDAR")
	bool bLiDAREnabled = true;


	UFUNCTION(BlueprintCallable, Category = "LiDAR")
	void SetLiDAREnabled(bool bEnabled);


	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "LiDAR")
	bool IsLiDAREnabled() const { return bLiDAREnabled; }


	UFUNCTION(BlueprintCallable, Category = "LiDAR")
	void SetAngle(float angle);
	UFUNCTION(BlueprintCallable, Category = "LiDAR")
	void SetRange(float dist);
	UFUNCTION(BlueprintCallable, Category = "LiDAR")
	void SetDotNumber(int32 number);

	UFUNCTION(BlueprintCallable, Category = "LiDAR")
	void reset_properties();
private:
	UFUNCTION(BlueprintCallable, Category = "Lidar")
	void AddPoints();

	UFUNCTION(BlueprintCallable, Category = "Lidar")
	void TimerComplete();

	void MakeStartEnd(FVector& OutStart, FVector& OutEnd) const;
	bool bBSPInitialized = false;
	void HideAllBSPAndModels();
	
	/*
	void OnScanTimerComplete();
	*/
};