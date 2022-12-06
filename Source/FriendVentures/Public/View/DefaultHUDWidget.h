// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DefaultHUDWidget.generated.h"

class UFriendsViewModel;
class UFriend;

/**
 * Default UMG widget to include in the HeadsUp Display.
 */
UCLASS(Abstract)
class FRIENDVENTURES_API UDefaultHUDWidget final : public UUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnLoadingFriendsList();

	UFUNCTION(BlueprintImplementableEvent)
	void OnFriendsListLoaded(bool bWasSuccessful);

	UFUNCTION(BlueprintImplementableEvent)
	void OnLoadingPresenceList();

	UFUNCTION(BlueprintImplementableEvent)
	void OnPresenceLoaded(bool bWasSuccessful);

	UFUNCTION(BlueprintImplementableEvent)
	void OnSinglePresenceChanged(UFriend* ChangedFriend);

	UFUNCTION(BlueprintCallable)
	const TArray<UFriend*>& GetFriendsList() const;
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	/**
	* @brief	Holds the class to use in order to instantiate the view-model that will connect the view with the model.
	* @remark	Its default value can be set in the editor (avoiding hard-coding it in code and recompilations on changes).
	 */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UFriendsViewModel> FriendsViewModelClass;

	UPROPERTY()
	UFriendsViewModel* FriendsViewModel;
};
