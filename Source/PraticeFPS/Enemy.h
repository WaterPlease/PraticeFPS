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
	EES_Falling UMETA(DisplayName = "Falling"),
	EES_Standup UMETA(DisplayName = "Standup"),
	EES_Dead UMETA(DisplayName = "Dead"),

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	class UBoxComponent* AttackCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	EEnemyState EnemyState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	bool bCanAttackPlayer;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	float StandupDelay;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool bFalling;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Stats")
	float MaxHealth;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Stats")
	float Health;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Stats")
	float DeathDelay;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Stats")
	float BaseDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Stats")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Animation")
	class UAnimMontage* EnemyAnimMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Animation")
	class UAnimationAsset* StandupAnimation;

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

	FTimerHandle DeathTimerHandle;
	FTimerHandle StandupTimerHandle;
private:
	int32 StandupAnimSectionIndex;
	float StandupTimeWhole;
	float StandupTimeTaken;

	FName DefaultMeshCollisionPresetName;
	FTransform MeshRelativeTransform;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	void ChasePlayer();
	void AttackPlayer();

	// Damage Logic
	void UpdateHealth(float DeltaHealth);
	void Die();
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnAttackSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnAttackSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void Ragdoll();
	void StandUp();
	UFUNCTION(BlueprintCallable)
	void StandupDone();
	void UpdateMeshPosition(bool bUpdateMeshPosition);

	UFUNCTION(BlueprintCallable)
	void DeathEnd();
	void DestroyEnemy();


	UFUNCTION(BlueprintCallable)
	void ActivateAttackCollision();
	UFUNCTION(BlueprintCallable)
	void DeactivateAttackCollision();
	UFUNCTION()
	void OnAttackCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
