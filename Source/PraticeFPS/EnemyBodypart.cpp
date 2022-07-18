// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBodypart.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Bullet.h"
#include "Particles/ParticleSystem.h"
#include "DrawDebugHelpers.h"

UEnemyBodypart::UEnemyBodypart()
{
	BodypartName = "Bodypart";
	DamageFactor = 1.0f;

	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

}

void UEnemyBodypart::BeginPlay()
{
	OnComponentBeginOverlap.AddDynamic(this, &UEnemyBodypart::OnBodypartAttacked);
}

void UEnemyBodypart::OnBodypartAttacked(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABullet* Bullet = Cast<ABullet>(OtherActor);
	if (!Bullet) return;
	FRotator ParticleRotator= UKismetMathLibrary::FindLookAtRotation(SweepResult.ImpactPoint, SweepResult.ImpactPoint - Bullet->GetVelocity().GetSafeNormal());
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodParticleSystem,
		FVector(SweepResult.ImpactPoint),
		ParticleRotator,FVector(0.3f));
}
