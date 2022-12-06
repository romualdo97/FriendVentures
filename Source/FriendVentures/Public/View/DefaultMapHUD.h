// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DefaultHUDWidget.h"
#include "GameFramework/HUD.h"
#include "DefaultMapHUD.generated.h"

class UUserWidget;

/**
 * The default HeadsUp-Display for the game.
 * An instance of an AHUD is assigned to an owner and offers a variety of function helpers. It can be used as
 * a logical container of widgets with info to display to player. For the sake of the example, we only use it
 * as the creator of the only widget to display to the player: HUDWidget.
 */
UCLASS(Abstract, meta = (DisableNativeTick))
class FRIENDVENTURES_API ADefaultMapHUD final : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override; // Used to instantiate, hold and properly initialize the HUD widget

private:
	/**
	* Holds the class to use in order to instantiate the main widget to show as part of the HUD.
	* Its default value can be set in the editor (avoiding hard-coding it in code and recompilations on changes).
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> HUDWidgetClass;

	/**
	 * Holds the instance of the only widget to show as part of the HUD. 
	 */
	UPROPERTY()
	UUserWidget* HUDWidget;;
};
