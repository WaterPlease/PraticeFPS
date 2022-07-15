// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "EnemyBodypart.generated.h"

/**
 * 
 */
UCLASS()
class PRATICEFPS_API UEnemyBodypart : public UBoxComponent
{
	GENERATED_BODY()
public:

	UEnemyBodypart();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bodypart")
	FString BodypartName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bodypart")
	float DamageFactor;

	class AEnemy* Enemy;
	
};
