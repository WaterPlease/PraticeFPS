// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponComponent.h"
#include "SMGComponent.generated.h"

/**
 * 
 */
UCLASS()
class PRATICEFPS_API USMGComponent : public UWeaponComponent
{
	GENERATED_BODY()
public:
	USMGComponent();
	virtual void Fire() override;
	virtual void Reload() override;
	virtual void SpawnBullet() override;

	UFUNCTION()
	virtual void ReloadDone();

	float ReloadTime;
	float ReloadTimeFactor;
	float InvReloadTimeFactor;

	FTimerHandle FireTimerHandle;
	FTimerHandle ReloadTimerHandle;
private:
	float InvReloadTime;

	USkeletalMesh* SK_SMG11;
	UAnimMontage* SMGAnimMontage;
	USoundCue* SMGCue;
	USoundConcurrency* SMGSoundConcurrency;
	UMaterial* DecalMaterial;
	UAnimBlueprint* SMGAnim_BP;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void InitWeapon(APlayerChar* PlayerContext);
};
