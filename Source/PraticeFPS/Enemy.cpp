// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "AIController.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerChar.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnemyBodypart.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackSphere"));
	AttackSphere->SetupAttachment(GetRootComponent());

	AttackCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackCollision"));
	AttackCollision->SetupAttachment(GetMesh(),FName("AttackSocket"));

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;


	// Bodypart initiation
	Head = CreateDefaultSubobject<UEnemyBodypart>(TEXT("Head"));
	Head->SetupAttachment(GetMesh(), "HeadSocket");

	Spine = CreateDefaultSubobject<UEnemyBodypart>(TEXT("Spine"));
	Spine->SetupAttachment(GetMesh(), "SpineSocket");

	RightArm = CreateDefaultSubobject<UEnemyBodypart>(TEXT("RightArm"));
	RightArm->SetupAttachment(GetMesh(), "RightArmSocket");

	RightForeArm = CreateDefaultSubobject<UEnemyBodypart>(TEXT("RightForeArm"));
	RightForeArm->SetupAttachment(GetMesh(), "RightForeArmSocket");

	LeftArm = CreateDefaultSubobject<UEnemyBodypart>(TEXT("LeftArm"));
	LeftArm->SetupAttachment(GetMesh(), "LeftArmSocket");

	LeftForeArm = CreateDefaultSubobject<UEnemyBodypart>(TEXT("LeftForeArm"));
	LeftForeArm->SetupAttachment(GetMesh(), "LeftForeArmSocket");

	RightUpLeg = CreateDefaultSubobject<UEnemyBodypart>(TEXT("RightUpLeg"));
	RightUpLeg->SetupAttachment(GetMesh(), "RightUpLegSocket");

	RightLeg = CreateDefaultSubobject<UEnemyBodypart>(TEXT("RightLeg"));
	RightLeg->SetupAttachment(GetMesh(), "RightLegSocket");

	LeftUpLeg = CreateDefaultSubobject<UEnemyBodypart>(TEXT("LeftUpLeg"));
	LeftUpLeg->SetupAttachment(GetMesh(), "LeftUpLegSocket");

	LeftLeg = CreateDefaultSubobject<UEnemyBodypart>(TEXT("LeftLeg"));
	LeftLeg->SetupAttachment(GetMesh(), "LeftLegSocket");

	EnemyState = EEnemyState::EES_Idle;
	bCanAttackPlayer = false;

	MaxHealth = 100.f;
	Health = 28.f;

	DeathDelay = 3.f;

	BaseDamage = 10.f;

	StandupDelay = 5.f;
	bFalling = false;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());
	if(!AIController) UE_LOG(LogTemp, Warning, TEXT("Failed to cast into AAIController"));
	Player = Cast<APlayerChar>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!Player) UE_LOG(LogTemp, Warning, TEXT("Failed to find player character"));

	// Bodypart init
	Head->BodypartName = "Head";
	Head->Enemy = this;

	Spine->BodypartName = "Spine";
	Spine->Enemy = this;

	RightArm->BodypartName = "RightArm";
	RightArm->Enemy = this;

	RightForeArm->BodypartName = "RightForeArm";
	RightForeArm->Enemy = this;

	LeftArm->BodypartName = "LeftArm";
	LeftArm->Enemy = this;

	LeftForeArm->BodypartName = "LeftForeArm";
	LeftForeArm->Enemy = this;

	RightUpLeg->BodypartName = "RightUpLeg";
	RightUpLeg->Enemy = this;

	RightLeg->BodypartName = "RightLeg";
	RightLeg->Enemy = this;

	LeftUpLeg->BodypartName = "LeftUpLeg";
	LeftUpLeg->Enemy = this;

	LeftLeg->BodypartName = "LeftLeg";
	LeftLeg->Enemy = this;

	AttackSphere->OnComponentBeginOverlap.AddDynamic(this,&AEnemy::OnAttackSphereOverlapBegin);
	AttackSphere->OnComponentEndOverlap.AddDynamic(this,&AEnemy::OnAttackSphereOverlapEnd);

	AttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AttackCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	AttackCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnAttackCollisionOverlapBegin);

	GetCharacterMovement()->bUseRVOAvoidance = true;

	DefaultMeshCollisionPresetName = GetMesh()->GetCollisionProfileName();

	MeshRelativeTransform = GetMesh()->GetRelativeTransform();

	StandupAnimSectionIndex = EnemyAnimMontage->GetSectionIndex(FName("StandupOnBack"));
}

void AEnemy::ChasePlayer()
{
	if (!AIController) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Player);
	MoveRequest.SetAcceptanceRadius(AttackSphere->GetScaledSphereRadius());

	FNavPathSharedPtr NavPath;
	AIController->MoveTo(MoveRequest, &NavPath);

	EnemyState = EEnemyState::EES_Chase;
}

void AEnemy::AttackPlayer()
{
	if (!EnemyAnimMontage) return;
	
	EnemyState = EEnemyState::EES_Attack;
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance)
	{
		AnimInstance->Montage_Play(EnemyAnimMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Attack"), EnemyAnimMontage);
	}
}

void AEnemy::UpdateHealth(float DeltaHealth)
{
	Health += DeltaHealth;
	if (Health < 0.f)
	{
		Die();
		return;
	}
}

void AEnemy::Die()
{
	if (EnemyState == EEnemyState::EES_Dead) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	GetWorldTimerManager().ClearTimer(StandupTimerHandle);
	if (!AnimInstance)
	{
		Destroy();
		return;
	}

	AnimInstance->Montage_Play(EnemyAnimMontage, 1.0f);
	AnimInstance->Montage_JumpToSection(FName("Death"), EnemyAnimMontage);

	EnemyState = EEnemyState::EES_Dead;

	AIController->StopMovement();

	GetCapsuleComponent()->SetCollisionResponseToAllChannels(
		ECollisionResponse::ECR_Ignore
	);
	GetCapsuleComponent()->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_WorldStatic,
		ECollisionResponse::ECR_Block
	);
}

float AEnemy::TakeDamage(
	float DamageAmount, 
	FDamageEvent const& DamageEvent, 
	AController* EventInstigator, 
	AActor* DamageCauser)
{
	float EffectiveDamage = -DamageAmount;
	// Modify EffectiveDamage here
	// ...
	UpdateHealth(EffectiveDamage);
	return -EffectiveDamage;
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetCharacterMovement()->IsFalling() &&
		GetMesh()->GetPhysicsAsset())
	{
		Ragdoll();
		AIController->StopMovement();
		GetWorldTimerManager().SetTimer(StandupTimerHandle, this, &AEnemy::StandUp, StandupDelay);
	}

	switch (EnemyState)
	{
	case EEnemyState::EES_Idle:
		if (bCanAttackPlayer)
		{
			AttackPlayer();
		}
		else
		{
			ChasePlayer();
		}
		break;
	case EEnemyState::EES_Chase:
		if (bCanAttackPlayer)
		{
			AttackPlayer();
		}
		else
		{
			if (AIController->GetMoveStatus()
				== EPathFollowingStatus::Idle)
			{
				ChasePlayer();
			}
		}
		break;
	case EEnemyState::EES_Falling:
		break;
	case EEnemyState::EES_Standup:
		//GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName("Hips"), 1.f - StandupTimeTaken / StandupTimeWhole);
		//StandupTimeTaken += DeltaTime;
		if (StandupTimeTaken < 1e-6)
		{
			auto AnimInstance = GetMesh()->GetAnimInstance();
			AnimInstance->Montage_Play(EnemyAnimMontage, 1.f);
			AnimInstance->Montage_JumpToSection(FName("StandupOnBack"), EnemyAnimMontage);
			StandupTimeWhole = EnemyAnimMontage->GetSectionLength(StandupAnimSectionIndex);
			StandupTimeTaken = 0.f;
		}
		StandupTimeTaken += DeltaTime;
		break;
	default:
		break;
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::OnAttackSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != (AActor*)Player) return;
	
	bCanAttackPlayer = true;
}

void AEnemy::OnAttackSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != (AActor*)Player) return;
	
	bCanAttackPlayer = false;
}

void AEnemy::Ragdoll()
{
	EnemyState = EEnemyState::EES_Falling;
	bFalling = true;

	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
	//GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("Hips"), true);
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName("Hips"), 1.f);
}

void AEnemy::StandUp()
{	
	EnemyState = EEnemyState::EES_Standup;

	GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("Hips"), false);
	GetMesh()->SetCollisionProfileName(DefaultMeshCollisionPresetName);

	UpdateMeshPosition(false);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;
	//GetMesh()->PlayAnimation(StandupAnimation,false);
	StandupTimeTaken = 0.f;
}

void AEnemy::StandupDone()
{
	EnemyState = EEnemyState::EES_Idle;
	bFalling = false;
	UpdateMeshPosition(true);
}

void AEnemy::UpdateMeshPosition(bool bUpdateMeshPosition)
{
	FVector CenterLocation = GetMesh()->GetBoneLocation(FName("Hips"));
	FRotator CenterRotation = GetMesh()->GetSocketRotation("StomachSocket");

	GetCapsuleComponent()->SetWorldLocation(CenterLocation);
	FVector FrontVector = FRotationMatrix(CenterRotation).GetUnitAxis(EAxis::Y);
	FrontVector.Z = 0.f;
	const FRotator FrontRotation = FrontVector.Rotation();
	GetCapsuleComponent()->SetWorldRotation(FrontRotation);

	GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	//GetMesh()->SetRelativeTransform(MeshRelativeTransform);
	GetMesh()->SetRelativeLocation(FVector(0.f,0.f,MeshRelativeTransform.GetLocation().Z));
	if (bUpdateMeshPosition)
		GetMesh()->SetRelativeLocation(MeshRelativeTransform.GetLocation());
	GetMesh()->SetRelativeRotation(MeshRelativeTransform.GetRotation());
}

void AEnemy::DeathEnd()
{
	GetWorldTimerManager().SetTimer(DeathTimerHandle, this, &AEnemy::DestroyEnemy, DeathDelay, false);

	GetMesh()->bPauseAnims = true;
}

void AEnemy::DestroyEnemy()
{
	Destroy();
}

void AEnemy::ActivateAttackCollision()
{
	AttackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateAttackCollision()
{
	AttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::OnAttackCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != (AActor*)Player) return;

	UGameplayStatics::ApplyDamage(Player, BaseDamage, GetController(), this, DamageType);
}

