// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "Enemy.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	if (Pawn) return;

	Pawn = TryGetPawnOwner();
	if (!Pawn) return;

	Enemy = Cast<AEnemy>(Pawn);
}

void UEnemyAnimInstance::UpdateProperties()
{
	if (!Pawn)
	{
		UEnemyAnimInstance::NativeInitializeAnimation();
	}

	if (Pawn && Enemy)
	{
		// Update speed
		const FVector Velocity = Pawn->GetVelocity();
		const FVector LateralVelocity = FVector(Velocity.X, Velocity.Y, 0.f);
		Speed = LateralVelocity.Size();

		// Update EnemyState
		EnemyState = Enemy->EnemyState;
	}
}
