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
#include "Engine/DecalActor.h"


USMGComponent::USMGComponent()
{
	//SK_SMG11 = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/SK_SMG11_X.SK_SMG11_X")));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SMG11Finder(TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/SK_SMG11_X.SK_SMG11_X"));
	SK_SMG11 = SK_SMG11Finder.Object;

	//SMGAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), nullptr, TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/Anim/SMGAnimMontage.SMGAnimMontage")));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> SMGAnimMontageFinder(TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/Anim/SMGAnimMontage.SMGAnimMontage"));
	SMGAnimMontage = SMGAnimMontageFinder.Object;

	//SMGCue = Cast<USoundCue>(StaticLoadObject(USoundCue::StaticClass(), nullptr, TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/Sound/9mm_Cue.9mm_Cue")));
	static ConstructorHelpers::FObjectFinder<USoundCue> SMGCueFinder(TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/Sound/9mm_Cue.9mm_Cue"));
	SMGCue = SMGCueFinder.Object;

	//SMGSoundConcurrency = Cast<USoundConcurrency>(StaticLoadObject(USoundConcurrency::StaticClass(), nullptr, TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/Sound/9mm_SoundConcurrency.9mm_SoundConcurrency")));
	static ConstructorHelpers::FObjectFinder<USoundConcurrency> SMGSoundConcurrencyFinder(TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/Sound/9mm_SoundConcurrency.9mm_SoundConcurrency"));
	SMGSoundConcurrency = SMGSoundConcurrencyFinder.Object;

	//DecalMaterial = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), nullptr, TEXT("/Game/Texture/BulletHoleMaterial.BulletHoleMaterial")));
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialFinder(TEXT("/Game/Texture/BulletHoleMaterial.BulletHoleMaterial"));
	DecalMaterial = DecalMaterialFinder.Object;
	
	static ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimBPFinder(TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/Anim/SMGAnim_BP.SMGAnim_BP"));
	SMGAnim_BP = AnimBPFinder.Object;
}

void USMGComponent::Fire()
{
	if (!bTryFire || bReloading)
		return;
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


	SpawnBullet();

	// Fire delay timer
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
void USMGComponent::SpawnBullet()
{
	// Spawn bullet
	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = PlayerCharacter;
	ABullet* Bullet = GetWorld()->SpawnActor<ABullet>(ABullet::StaticClass(), PlayerCharacter->Camera->GetComponentLocation(), PlayerCharacter->Camera->GetComponentRotation(), SpawnParam);
	const FRotator CamRotation = PlayerCharacter->Camera->GetComponentRotation();
	Bullet->PlayerChar = PlayerCharacter;
	Bullet->SetInitSpeed(10000.f);
	Bullet->SetMaxSpeed(10000.f);
	Bullet->SetBulletHeadSize(1.f);
	Bullet->Launch(FRotationMatrix(CamRotation).GetUnitAxis(EAxis::X));
	Bullet->DecalMaterial = WeaponDecalMaterial;
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
	RemainBullets = RoundCapacity;

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

	if (SK_SMG11)
	{
		PlayerCharacter->GetMesh()->SetSkeletalMesh(SK_SMG11);

		PlayerCharacter->GetMesh()->CastShadow = false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to a load asset : %s"), TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/SK_SMG11_X.SK_SMG11_X"))
	}


	PlayerCharacter->GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	//UAnimBlueprint* SMGAnim_BP = Cast<UAnimBlueprint>(StaticLoadObject(UAnimBlueprint::StaticClass(), nullptr, TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/Anim/SMGAnim_BP.SMGAnim_BP_C")));

	if (SMGAnim_BP)
	{
		PlayerCharacter->GetMesh()->SetAnimInstanceClass(SMGAnim_BP->GeneratedClass);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to a load asset : %s"), TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/Anim/SMGAnim_BP.SMGAnim_BP"));
	}

	SetMesh(PlayerCharacter->GetMesh());


	if (SMGAnimMontage)
	{
		SetAnimMontage(SMGAnimMontage);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to a load asset : %s"), TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/Anim/SMGAnimMontage.SMGAnimMontage"));
	}


	if (SMGCue)
	{
		WeaponFireSound = SMGCue;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to a load asset : %s"), TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/Sound/9mm_Cue.9mm_Cue"));
	}


	if (SMGSoundConcurrency)
	{
		WeaponFireSoundConcurrency = SMGSoundConcurrency;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to a load asset : %s"), TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/Sound/9mm_SoundConcurrency.9mm_SoundConcurrency"));
	}


	if (DecalMaterial)
	{
		WeaponDecalMaterial = DecalMaterial;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to a load asset : %s"), TEXT("/Game/Texture/BulletHoleMaterial.BulletHoleMaterial"));
	}
}
