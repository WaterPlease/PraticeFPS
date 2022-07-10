// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "PlayerChar.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

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
	ProjectileMovementComponent->Bounciness = 0.3f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

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
	BulletHead->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

}

void ABullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherComp->GetCollisionObjectType() == ECollisionChannel::ECC_WorldStatic)
	{
		// Spawn decal
	}
}

inline void ABullet::SetInitSpeed(float Speed) { ProjectileMovementComponent->InitialSpeed = Speed; }

inline void ABullet::SetMaxSpeed(float Speed) { ProjectileMovementComponent->MaxSpeed = Speed; }

inline void ABullet::SetBulletHeadSize(float Size) { BulletHead->SetSphereRadius(Size); }

void ABullet::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetUniqueID() != OwnerID)
	{
		// enemy hit
	}
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

