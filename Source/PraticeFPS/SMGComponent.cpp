// Fill out your copyright notice in the Description page of Project Settings.


#include "SMGComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "PlayerChar.h"
#include "FPSAnimInstance.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Bullet.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
USMGComponent::USMGComponent()
{

}

void USMGComponent::Fire()
{
	if (!bTryFire || bReloading)
		return;
	UE_LOG(LogTemp, Warning, TEXT("Remain bullets : %d->%d"), RemainBullets,FMath::Max(RemainBullets-1,0));
	if (RemainBullets == 0)
	{
		// play empty round sound
		bTryFire = false;
		return;
	}
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && GetAnimMontage())
	{
		AnimInstance->Montage_Play(WeaponAnimMontage, InvDelay * FireRateFactor);
		AnimInstance->Montage_JumpToSection("Fire", WeaponAnimMontage);
	}
	UGameplayStatics::PlaySound2D(PlayerCharacter, WeaponFireSound,1.f,1.f,0.f,WeaponFireSoundConcurrency);
	RemainBullets--;

	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = PlayerCharacter;
	ABullet* Bullet = GetWorld()->SpawnActor<ABullet>(ABullet::StaticClass(),PlayerCharacter->Camera->GetComponentLocation(), PlayerCharacter->Camera->GetComponentRotation(), SpawnParam);
	Bullet->ProjectileMovementComponent->Velocity = Bullet->ProjectileMovementComponent->Velocity.GetSafeNormal() * 300.f;
	Bullet->ProjectileMovementComponent->UpdateComponentVelocity();
	PlayerCharacter->GetWorldTimerManager().SetTimer(FireTimerHandle, this, &USMGComponent::Fire, Delay*InvFireRateFactor, false);
}

void USMGComponent::Reload()
{
	if (RemainBullets == RoundCapacity) return;
	bReloading = true;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && GetAnimMontage())
	{
		AnimInstance->Montage_Play(WeaponAnimMontage, InvReloadTime * ReloadTimeFactor);
		AnimInstance->Montage_JumpToSection("Reload", WeaponAnimMontage);
	}
	PlayerCharacter->GetWorldTimerManager().SetTimer(FireTimerHandle, this, &USMGComponent::ReloadDone, ReloadTime * InvReloadTimeFactor, false);
}
void USMGComponent::ReloadDone()
{
	bReloading = false;
	RemainBullets = RoundCapacity;
}

void USMGComponent::BeginPlay()
{
	Super::BeginPlay();
	bTryFire = false;
	Delay = 0.075f;
	InvFireRateFactor = 1.f / FireRateFactor;
	InvDelay = 1.f / Delay;

	RoundCapacity = 30;
	RemainBullets = 1;//RoundCapacity;

	ReloadTime = 1.5f;
	InvReloadTime = 1.f / ReloadTime;
	ReloadTimeFactor = 1.f;
	InvReloadTimeFactor = 1.f / ReloadTimeFactor;
}

void USMGComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USMGComponent::InitWeapon(APlayerChar* PlayerContext)
{
	Super::InitWeapon(PlayerContext);

	USkeletalMesh* SK_SMG11 = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/SK_SMG11_X.SK_SMG11_X")));
	if (SK_SMG11)
	{
		PlayerCharacter->GetMesh()->SetSkeletalMesh(SK_SMG11);
	}
	else
	{
		UE_LOG(LogTemp,Warning, TEXT("Failed to a load asset : %s"), TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/SK_SMG11_X.SK_SMG11_X"))
	}


	PlayerCharacter->GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	UAnimBlueprint* SMGAnim_BP = Cast<UAnimBlueprint>(StaticLoadObject(UAnimBlueprint::StaticClass(), nullptr, TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/Anim/SMGAnim_BP.SMGAnim_BP")));
	if (SMGAnim_BP)
	{
		PlayerCharacter->GetMesh()->SetAnimInstanceClass(SMGAnim_BP->GeneratedClass);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to a load asset : %s"), TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/Anim/SMGAnim_BP.SMGAnim_BP"));
	}

	SetMesh(PlayerCharacter->GetMesh());

	UAnimMontage* SMGAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), nullptr, TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/Anim/SMGAnimMontage.SMGAnimMontage")));
	if (SMGAnimMontage)
	{
		SetAnimMontage(SMGAnimMontage);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to a load asset : %s"), TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/Anim/SMGAnimMontage.SMGAnimMontage"));
	}

	USoundCue* SMGCue = Cast<USoundCue>(StaticLoadObject(USoundCue::StaticClass(), nullptr, TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/Sound/9mm_Cue.9mm_Cue")));
	if (SMGCue)
	{
		WeaponFireSound = SMGCue;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to a load asset : %s"), TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/Sound/9mm_Cue.9mm_Cue"));
	}
	
	USoundConcurrency* SMGSoundConcurrency = Cast<USoundConcurrency>(StaticLoadObject(USoundConcurrency::StaticClass(), nullptr, TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/Sound/9mm_SoundConcurrency.9mm_SoundConcurrency")));
	if (SMGSoundConcurrency)
	{
		WeaponFireSoundConcurrency = SMGSoundConcurrency;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to a load asset : %s"), TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/Sound/9mm_SoundConcurrency.9mm_SoundConcurrency"));
	}
}
