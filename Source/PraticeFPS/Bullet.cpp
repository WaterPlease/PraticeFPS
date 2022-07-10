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

	// Use this component to drive this projectile's movement.
	ProjectileMovementComponent->InitialSpeed = 300.0f;
	ProjectileMovementComponent->MaxSpeed = 300;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.3f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	BulletHead->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnBeginOverlap);

	BulletHead->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BulletHead->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	BulletHead->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BulletHead->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	BulletHead->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	BulletHead->SetSimulatePhysics(true);
	BulletHead->SetEnableGravity(false);
	BulletHead->bHiddenInGame = false;
	BulletHead->SetSphereRadius(10.f);

}

void ABullet::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;
	//if (OtherActor == (AActor*)OwnerPlayer) return;
	if (OtherComp->GetCollisionObjectType() == ECollisionChannel::ECC_WorldStatic)
	{
		UE_LOG(LogTemp, Warning, TEXT("WorldStatic collided"));
	}
	else if (OtherComp->GetCollisionObjectType() == ECollisionChannel::ECC_Pawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pawn collided"));
	}
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

