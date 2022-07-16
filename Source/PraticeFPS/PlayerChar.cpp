// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerChar.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "WeaponComponent.h"
#include "SMGComponent.h"
#include "GameFramework/Actor.h"

#define EPS (1e-3)

// Sets default values
APlayerChar::APlayerChar()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetRootComponent());
	Camera->bUsePawnControlRotation = true;
	GetMesh()->SetupAttachment(Camera);
	WeaponComponent = CreateDefaultSubobject<USMGComponent>(TEXT("Weapon"));
	Velocity = 650.f;
	JumpVelocity = 650.f;
	AirControl = .5f;
	RunningFactor = 1.4f;

	bCrouch = false;

	bSliding = false;

	SlidingAccelFactor = 1.7f;
	SlidingTime = 1.f;
	CrouchTime = .5f;

	DashAccelFactor = 2.0f;
	DashDuration = 0.5f;
	bDash = false;
	DashMaxCount = 2;
	CurrentDashCount = 0;
	JumpMaxCount = 2;

	MaxHealth = 100.f;
	Health = 70.f;
	HealthRate = 1.f;
	HealthRegenTime = 10.f;
	HealthStatus = EHealthStatus::EHS_Idle;
}

// Called when the game starts or when spawned
void APlayerChar::BeginPlay()
{
	Super::BeginPlay();

	Camera->AddLocalOffset(FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight()- GetCapsuleComponent()->GetScaledCapsuleRadius()) + CameraOffset);

	auto CharacterMovementComponent = GetCharacterMovement();
	CharacterMovementComponent->JumpZVelocity = JumpVelocity;
	CharacterMovementComponent->AirControl = AirControl;
	CharacterMovementComponent->MaxWalkSpeed = Velocity;
	CharacterMovementComponent->MaxAcceleration = Velocity * 10.f;
	CharacterMovementComponent->GetNavAgentPropertiesRef().bCanCrouch = true;

	StandHeight = BaseEyeHeight;

	USMGComponent* SmgComponent = Cast<USMGComponent>(WeaponComponent);
	SmgComponent->InitWeapon(this);

	MeshOffset = GetMesh()->GetRelativeLocation();// -Camera->GetComponentLocation();
	UE_LOG(LogTemp, Warning, TEXT("%f, %f, %f"), MeshOffset.X, MeshOffset.Y, MeshOffset.Z);
}

// Called every frame
void APlayerChar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!GetCharacterMovement()->IsFalling())
		JumpCount = 0;

	//GetMesh()->SetRelativeLocation(MeshOffset);
	if (bRunning 
		&&FVector2D(GetInputAxisValue("Horizontal"), GetInputAxisValue("Vertical")).IsNearlyZero())
	ToggleRun();
	if (bSliding)
	{
		float ZVelocity = GetCharacterMovement()->Velocity.Z;
		GetCharacterMovement()->Velocity = SlidingDirection * (FMath::Lerp(SlidingAccelFactor,1.f,SlidedTime/SlidingTime) * Velocity);
		GetCharacterMovement()->Velocity.Z = ZVelocity;
		SlidedTime += DeltaTime;
	}
	if (CrouchingTime < CrouchTime)
	{
		LerpedHeight = FMath::Lerp(LerpedHeight, TargetHeight, CrouchingTime / CrouchTime);
		Camera->SetRelativeLocation(FVector(0.f, 0.f, LerpedHeight - TargetHeight) + CameraOffset);
		//GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, LerpedHeight - TargetHeight) + CameraOffset + MeshOffset);
		GetMesh()->SetRelativeLocation(MeshOffset);
		CrouchingTime += DeltaTime;
	}
	else
	{
		GetMesh()->SetRelativeLocation(MeshOffset);
	}

	if (bDash)
	{
		GetCharacterMovement()->Velocity = DashDirection * (FMath::Lerp(DashAccelFactor, 1.f, DashingTime / DashDuration) * Velocity);
		DashingTime += DeltaTime;
	}
	else
	{
		if (!GetCharacterMovement()->IsFalling()) { CurrentDashCount = 0; }
	}

	// Health Regen
	if (HealthStatus == EHealthStatus::EHS_Idle &&
		Health < MaxHealth)
	{
		Health = FMath::Min(Health + DeltaTime * HealthRate, MaxHealth);
	}
}

// Called to bind functionality to input
void APlayerChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerChar::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APlayerChar::EndFire);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APlayerChar::InputReload);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerChar::InputJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APlayerChar::InputCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &APlayerChar::InputUnCrouch);
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &APlayerChar::ToggleRun);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &APlayerChar::Dash);

	PlayerInputComponent->BindAxis("Yaw", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Pitch", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Horizontal", this, &APlayerChar::InputHorizontal);
	PlayerInputComponent->BindAxis("Vertical", this, &APlayerChar::InputVertical);
}

void APlayerChar::InputJump()
{
	if (bSliding)
	{
		bSliding = false;
		bCrouch = false;
		ACharacter::UnCrouch();
	}
	if (GetCharacterMovement()->IsFalling())
	{
		if (JumpCount < JumpMaxCount) JumpCount++;
		else return;

		JumpCount = FMath::Max<uint8>(2,JumpCount);
	}
	else
	{
		JumpCount = 1;
	}
	GetCharacterMovement()->Velocity.Z = JumpVelocity;
	GetCharacterMovement()->SetMovementMode(MOVE_Falling);
}

void APlayerChar::InputHorizontal(float Value)
{
	if (Controller != nullptr &&
		FMath::Abs(Value) > EPS)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation = FRotator(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void APlayerChar::InputVertical(float Value)
{
	if (Controller != nullptr &&
		FMath::Abs(Value) > EPS)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation = FRotator(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APlayerChar::InputCrouch()
{
	CrouchingTime = 0.f;
	LerpedHeight = StandHeight;
	TargetHeight = CrouchedEyeHeight;
	bCrouch = true;
	ACharacter::Crouch();
	if (bRunning && !GetCharacterMovement()->IsFalling() && !bSliding)
	{
		bSliding = true;
		ToggleRun();
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation = FRotator(0.f, Rotation.Yaw, 0.f);

		SlidingDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		GetWorldTimerManager().SetTimer(SlidingTimerHandle, this, &APlayerChar::EndSliding, SlidingTime, false);
		SlidedTime = 0.f;
	}
	else
	{
		Walk();
	}
}

void APlayerChar::InputUnCrouch()
{
	if (bSliding || !bCrouch) return;
	bCrouch = false;
	CrouchingTime = 0.f;
	LerpedHeight = CrouchedEyeHeight;
	TargetHeight = StandHeight;
	ACharacter::UnCrouch();
}

void APlayerChar::ToggleRun()
{
	if (GetMovementComponent()->IsFalling()) return;
	bRunning = !bRunning;
	GetCharacterMovement()->MaxWalkSpeed = (bRunning) ? Velocity * RunningFactor : Velocity;
}

void APlayerChar::Run()
{
	if (bRunning) return;
	bRunning = true;

	GetCharacterMovement()->MaxWalkSpeed = Velocity * RunningFactor;
}

void APlayerChar::Walk()
{
	if (!bRunning) return;
	bRunning = false;
	GetCharacterMovement()->MaxWalkSpeed = Velocity;
}

void APlayerChar::EndSliding()
{
	bSliding = false;
	InputUnCrouch();
}

void APlayerChar::Dash()
{
	if (CurrentDashCount >= DashMaxCount) return;
	CurrentDashCount++;
	bDash = true;
	if (bSliding) EndSliding();
	else if (bCrouch) InputUnCrouch();
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation = FRotator(0.f, Rotation.Yaw, 0.f);

	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) * GetInputAxisValue("Horizontal");
	const FVector FrontDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) * GetInputAxisValue("Vertical");
	DashDirection = (RightDirection + FrontDirection).GetSafeNormal();
	if (DashDirection.IsNearlyZero()) DashDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	DashingTime = 0.f;
	GetWorldTimerManager().SetTimer(DashTimerHandle, this, &APlayerChar::EndDash, DashDuration,false);
}
void APlayerChar::EndDash()
{
	bDash = false;
}

void APlayerChar::StartFire()
{
	WeaponComponent->bTryFire = true;
	WeaponComponent->Fire();
}

void APlayerChar::EndFire()
{
	WeaponComponent->bTryFire = false;
}

void APlayerChar::InputReload()
{
	WeaponComponent->Reload();
}

void APlayerChar::UpdateHealth(float DeltaHealth, bool bOverheal=false)
{
	Health += DeltaHealth;
	if (DeltaHealth < 0.f)
	{
		HealthStatus = EHealthStatus::EHS_Hurt;
		GetWorldTimerManager().ClearTimer(HealthTimerHandle);
		GetWorldTimerManager().SetTimer(HealthTimerHandle, this,
			&APlayerChar::RegenHealth, HealthRegenTime, false);
		if (Health < 0.f)
		{
			Die();
			return;
		}
	}
	if (Health > MaxHealth && !bOverheal) Health = MaxHealth;
}

void APlayerChar::RegenHealth()
{
	HealthStatus = EHealthStatus::EHS_Idle;
}

void APlayerChar::Die()
{
	HealthStatus = EHealthStatus::EHS_Die;
	Health = 0.f;
}

float APlayerChar::TakeDamage(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	float EffectiveDamage = -DamageAmount;
	
	// Modify EffectiveDamage here
	// ...

	UpdateHealth(EffectiveDamage);
	return -EffectiveDamage;
}

int32 APlayerChar::GetRemainBullets()
{
	return WeaponComponent->GetRemainBullets();
}

int32 APlayerChar::GetRoundCapacity()
{
	return WeaponComponent->GetRoundCapacity();
}

float APlayerChar::GetRemainBulletsNormalized()
{
	return WeaponComponent->GetRemainBulletsNormalized();
}
