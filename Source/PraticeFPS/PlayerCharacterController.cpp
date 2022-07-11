// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterController.h"
#include "Blueprint/UserWidget.h"


void APlayerCharacterController::BeginPlay()
{
	Super::BeginPlay();

	// Instanctiate Widgets
	if (HUDInGameAsset)
	{
		HUDInGame = CreateWidget<UUserWidget>(this, HUDInGameAsset);
		if (HUDInGame)
		{
			HUDInGame->AddToViewport();
			HUDInGame->SetVisibility(ESlateVisibility::Visible);
		}
	}
}
void APlayerCharacterController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}