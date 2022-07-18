// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "EnemyBodypart.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PRATICEFPS_API UEnemyBodypart : public UBoxComponent
{
	GENERATED_BODY()
public:

	UEnemyBodypart();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bodypart")
	FString BodypartName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bodypart")
	float DamageFactor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bodypart")
	UParticleSystem* BloodParticleSystem;

	class AEnemy* Enemy;

public:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBodypartAttacked(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
