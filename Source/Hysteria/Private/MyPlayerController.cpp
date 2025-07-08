// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "HUDWidget.h"
#include "Blueprint/UserWidget.h"

void AMyPlayerController::BeginPlay()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		PC->bShowMouseCursor = true;
		PC->bEnableClickEvents = true;
		PC->bEnableMouseOverEvents = true;
	}
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay"));
	if (IsLocalPlayerController())
	{
		// It's a real player's controller, safe to create HUD widgets
		UE_LOG(LogTemp, Warning, TEXT("IsLocalPlayerController"));
	}
	if (GetLocalPlayer())
	{
		// Also safe!
		UE_LOG(LogTemp, Warning, TEXT("GetLocalPlayer"));
	}

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UHUDWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();
		}
	}
}
