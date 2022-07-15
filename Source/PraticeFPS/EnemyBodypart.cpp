// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBodypart.h"

UEnemyBodypart::UEnemyBodypart()
{
	BodypartName = "Bodypart";
	DamageFactor = 1.0f;

	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
}
