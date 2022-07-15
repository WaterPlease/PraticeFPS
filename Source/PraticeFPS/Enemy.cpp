// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "AIController.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerChar.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnemyBodypart.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackSphere"));
	AttackSphere->SetupAttachment(GetRootComponent());

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

	ChasePlayer();
}

void AEnemy::ChasePlayer()
{
	if (!AIController) return;
	UE_LOG(LogTemp, Warning, TEXT("Chasing Start"));

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Player);
	MoveRequest.SetAcceptanceRadius(AttackSphere->GetScaledSphereRadius());

	FNavPathSharedPtr NavPath;
	if (AIController->GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		UE_LOG(LogTemp, Warning, TEXT("Focus actor check succ 0"));
	}
	AIController->MoveTo(MoveRequest, &NavPath);
	if (AIController->GetMoveStatus() != EPathFollowingStatus::Idle)
	{
		UE_LOG(LogTemp, Warning, TEXT("Focus actor check succ 1"));
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

