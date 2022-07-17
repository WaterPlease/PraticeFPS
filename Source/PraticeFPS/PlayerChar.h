// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerChar.generated.h"

enum class EHealthStatus : uint8
{
	EHS_Idle UMETA(DisplayName="Idle"),
	EHS_Hurt UMETA(DisplayName = "Hurt"),
	EHS_Die UMETA(DisplayName = "Die"),

	EHS_MAX UMETA(DisplayName = "DefaultMax"),
};

UCLASS()
class PRATICEFPS_API APlayerChar : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerChar();
	class UCameraComponent* Camera;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS | Camera")
	FVector CameraOffset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS | Mesh")
	FVector MeshOffset;

	// Fileds for movement
	/** Walk velocity */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS | Movement")
	float Velocity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS | Movement")
	float CrouchVelocity;
	/** Jump velocity in z-axis */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS | Movement")
	float JumpVelocity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS | Movement")
	class USoundCue* JumpInAirSound;
	/** In-Air Control factor*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS | Movement")
	float AirControl;
	/** Running speed factor */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS | Movement")
	float RunningFactor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS | Movement")
	bool bCrouch;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS | Movement")
	bool bRunning;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS | Movement")
	float CrouchTime;
	/** Offset of camera when crouching 
	*   Only valid in the middle of crouching or standing on.
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPS | Movement")
	float LerpedHeight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPS | Movement")
	float TargetHeight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS | Movement")
	bool bSliding;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS | Movement")
	float SlidingTime;
	/** Speed factor when starting sliding */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS | Movement")
	float SlidingAccelFactor;
	/** Speed factor when starting Dashing  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS | Movement")
	float DashAccelFactor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS | Movement")
	float DashDuration;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS | Movement")
	bool bDash;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS | Movement")
	USoundCue* DashSound;
	/** Not used yet*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS | Movement")
	uint8 DashMaxCount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS | Movement")
	uint8 CurrentDashCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS | Weapon")
	class UWeaponComponent* WeaponComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Stats")
	float MaxHealth;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Stats")
	float Health;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Stats")
	float HealthRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Stats")
	float HealthRegenTime;
	EHealthStatus HealthStatus;


	FTimerHandle SlidingTimerHandle;
	FTimerHandle DashTimerHandle;
	FTimerHandle HealthTimerHandle;
private:
	float StandHeight;
	float SlidedTime;
	float CrouchingTime;
	float DashingTime;
	uint8 JumpCount;
	FVector SlidingDirection;
	FVector DashDirection;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void InputJump();
	void InputHorizontal(float Value);
	void InputVertical(float Value);
	void InputCrouch();
	void InputUnCrouch();
	void ToggleRun();
	void Run();
	void Walk();
	void EndSliding();
	void Dash();
	void EndDash();
	void StartFire();
	void EndFire();
	void InputReload();

	// Damage logic
	void UpdateHealth(float DeltaHealth, bool bOverheal);
	void RegenHealth();
	void Die();
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;



	// Player character's Data
	UFUNCTION(BlueprintCallable)
	int32 GetRemainBullets();
	UFUNCTION(BlueprintCallable)
	int32 GetRoundCapacity();
	UFUNCTION(BlueprintCallable)
	float GetRemainBulletsNormalized();

};
