// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PRATICEFPS_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | Internal")
	class APlayerChar* PlayerCharacter;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | Internal")
	class USkeletalMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | Internal")
	class UAnimMontage* WeaponAnimMontage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | Internal")
	class USoundCue* WeaponFireSound;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | Internal")
	class USoundConcurrency* WeaponFireSoundConcurrency;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | Internal")
	class USoundCue* WeaponReloadSound;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | Internal")
	class UMaterial* WeaponDecalMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | Stat")
	float Delay;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | Stat")
	float FireRateFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | Stat")
	int32 RoundCapacity;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | Stat")
	int32 RemainBullets;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | Mechanism")
	bool bReloading;

	bool bTryFire;

public:

protected:
	float InvFireRateFactor;
	float InvDelay;
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void Fire() {};
	virtual void Reload() {};
	virtual void SpawnBullet() {};

	FORCEINLINE void SetMesh(USkeletalMeshComponent* Target) { Mesh = Target; }
	FORCEINLINE USkeletalMeshComponent* GetMesh() { return Mesh; }
	FORCEINLINE void SetAnimMontage(UAnimMontage* Target) { WeaponAnimMontage = Target; }
	FORCEINLINE UAnimMontage* GetAnimMontage() { return WeaponAnimMontage; }

	virtual void InitWeapon(APlayerChar* PlayerContext);

	FORCEINLINE int32 GetRoundCapacity() { return RoundCapacity; }
	FORCEINLINE int32 GetRemainBullets() { return RemainBullets; }
	FORCEINLINE float GetRemainBulletsNormalized() { return RemainBullets/(float)RoundCapacity; }
		
};
