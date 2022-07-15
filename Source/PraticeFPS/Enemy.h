// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

/**
* State of enemy
*/
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Idle UMETA(DisplayName="Idle"),
	EES_Chase UMETA(DisplayName = "Chase"),
	EES_Attack UMETA(DisplayName = "Attack"),

	EES_MAX UMETA(DisplayName = "DefaultMax"),
};

/**
* Basic enemy character class.
*/
UCLASS()
class PRATICEFPS_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AAIController* AIController;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	class USphereComponent* AttackSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	EEnemyState EnemyState;

	class APlayerChar* Player;

	// Bodyparts Component
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bodypart")
	class UEnemyBodypart* Head;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bodypart")
	UEnemyBodypart* Spine;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bodypart")
	UEnemyBodypart* RightArm;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bodypart")
	UEnemyBodypart* RightForeArm;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bodypart")
	UEnemyBodypart* LeftArm;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bodypart")
	UEnemyBodypart* LeftForeArm;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bodypart")
	UEnemyBodypart* RightUpLeg;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bodypart")
	UEnemyBodypart* RightLeg;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bodypart")
	UEnemyBodypart* LeftUpLeg;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bodypart")
	UEnemyBodypart* LeftLeg;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	void ChasePlayer();



	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
