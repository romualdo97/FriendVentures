// Fill out your copyright notice in the Description page of Project Settings.


#include "View/DefaultMapHUD.h"

#include "Blueprint/UserWidget.h"
#include "FriendVentures/FriendVentures.h"

void ADefaultMapHUD::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(HUDWidgetClass))
	{
		UE_LOG(LogFriendVentures, Error, TEXT("HudWidgetClass is invalid."));
		return;
	}

	if (APlayerController* PlayerController = GetOwningPlayerController())
	{
		HUDWidget = CreateWidget<UUserWidget>(PlayerController, HUDWidgetClass);

		if (IsValid(HUDWidget))
		{
			constexpr int ZOrder = 0;
			HUDWidget->AddToPlayerScreen(ZOrder); // In this case, we could have used AddToViewport instead since we're not dealing with a split screen
			PlayerController->SetShowMouseCursor(true);
			HUDWidget->SetFocus();			
		}
		else
		{
			UE_LOG(LogFriendVentures, Error, TEXT("HudWidget is invalid."));		
		}
	}
	else
	{
		UE_LOG(LogFriendVentures, Error, TEXT("PlayerController is null."));	
	}
}
