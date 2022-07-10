// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class PRATICEFPS_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bullet")
	class USphereComponent* BulletHead;
	
	UPROPERTY(VisibleAnywhere, Category = "Bullet")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	uint32 OwnerID;

	/**
	* Implement This later...
	UPROPERTY(EditdefaultsOnly, BlueprintReadOnly, Cateogry = "Bullet")
	class USphereComponent* BulletCase;
	*/

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Launch(const FVector& direction);
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	FORCEINLINE void SetInitSpeed(float Speed);
	FORCEINLINE void SetMaxSpeed(float Speed);
	FORCEINLINE void SetBulletHeadSize(float Size);
};
