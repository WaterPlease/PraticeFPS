// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "PlayerChar.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "DrawDebugHelpers.h"
#include "EnemyBodypart.h"

#define EPS (1e-3)

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BulletHead = CreateDefaultSubobject<USphereComponent>(TEXT("BulletHead"));
	SetRootComponent(BulletHead);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(BulletHead);

	ProjectileMovementComponent->InitialSpeed = 300.f;
	ProjectileMovementComponent->MaxSpeed = 300.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 1.0f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	BounceCount = 0;
	MaxBounceCount = 1;

	BulletHead->SetHiddenInGame(false);
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	BulletHead->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnBeginOverlap);
	BulletHead->OnComponentHit.AddDynamic(this, &ABullet::OnHit);

	BulletHead->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BulletHead->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BulletHead->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BulletHead->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	//BulletHead->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	BulletHead->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);

	GetWorldTimerManager().SetTimer(BulletDestroyTimerHandle, this, &ABullet::DestroyBullet, 5.f);
}

void ABullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherComp->GetCollisionObjectType() == ECollisionChannel::ECC_WorldStatic)
	{
		// Spawn decal
		SpawnDecal(Hit.Location - Hit.ImpactNormal * 5.f, Hit.ImpactNormal.Rotation());
		BounceCount++;

		if (BounceCount == MaxBounceCount)
		{
			Destroy();
		}
		//DrawDebugLine(GetWorld(), Hit.Location, Hit.Location + Hit.ImpactNormal * 40.f, FColor::Red,false,20.f,0U,10.f);
	}
}

inline void ABullet::SetInitSpeed(float Speed) { ProjectileMovementComponent->InitialSpeed = Speed; }

inline void ABullet::SetMaxSpeed(float Speed) { ProjectileMovementComponent->MaxSpeed = Speed; }

inline void ABullet::SetBulletHeadSize(float Size) { BulletHead->SetSphereRadius(Size); }

void ABullet::DestroyBullet()
{
	Destroy();
}

void ABullet::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("overlap"));
	if (OtherActor->GetUniqueID() == OwnerID)
	{
		return;
	}

	UEnemyBodypart* Bodypart = Cast<UEnemyBodypart>(OtherComp);
	if (!Bodypart) return;
	
	// Enemy hit
	UE_LOG(LogTemp, Warning, TEXT("Bodypart hit : %s => %f"), *(Bodypart->BodypartName),Bodypart->DamageFactor);
	DestroyBullet();
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABullet::Launch(const FVector& direction) {
	ProjectileMovementComponent->Velocity = ProjectileMovementComponent->InitialSpeed * direction;
	BulletHead->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

ADecalActor* ABullet::SpawnDecal(FVector Location, FRotator Rotator)
{
	if (!DecalMaterial) return nullptr;
	Rotator.Pitch += 90.f; // Front of DecalActor look down initially.
	ADecalActor* DecalActor = GetWorld()->SpawnActor<ADecalActor>(Location, Rotator);
	if (!IsValid(DecalActor)) return nullptr;
	DecalActor->SetDecalMaterial(DecalMaterial);
	DecalActor->SetLifeSpan(5.f);
	DecalActor->GetDecal()->DecalSize = FVector(10.f,4.f,4.f);
	//const FVector endPoint = Location + DecalActor->GetTransform().GetUnitAxis(EAxis::X)*10.f;
	//DrawDebugLine(GetWorld(), Location, endPoint, FColor::Red, false, 20.f, 0U, 10.f);
	return DecalActor;
}

