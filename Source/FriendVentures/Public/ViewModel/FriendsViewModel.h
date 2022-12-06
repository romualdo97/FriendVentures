// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ViewModelInterface.h"
#include "Data/Friend.h"
#include "UObject/Object.h"
#include "FriendsViewModel.generated.h"

class UOnlineServicesSubsystem;

/**
 * Intermediary between the widgets and the data of friends.
 * Note that this class does not depends on any view class, this is an
 * important concept on the model-view-viewmodel pattern, in general dependency
 * can be explained as view -> ViewModel -> Model. Think of model as an isolated
 * component/plugin/package, ViewModel should use the model (but no the views)
 * and Views can use ViewModel (which may returns some objects from the model),
 * ViewModel is the model for the View.
 * A really good explanation and implementation of this is here: https://blog.grijjy.com/2018/01/22/mvvm-starter-kit-part-1-of-3/
 * NOTE: Since a viewmodel knows nothing about the view, UI and ViewModel development is independent 
 */
UCLASS(Blueprintable, Abstract, meta = (DisableNativeTick))
class FRIENDVENTURES_API UFriendsViewModel final : public UObject, public IViewModel
{
	GENERATED_BODY()

	// EVENTS:
	// 1- On Loading Friends List
	// 2- On Friends List Loaded
	// 3- On Loading Presence List
	// 4- On Presence Loaded
	// 5- On Single Presence Changed

	DECLARE_EVENT(UFriendsViewModel, FOnLoadingFriendsList);
	DECLARE_EVENT_OneParam(UFriendsViewModel, FOnFriendsListLoaded, bool /* bWasSuccessful */);
	DECLARE_EVENT(UFriendsViewModel, FOnLoadingPresenceList);
	DECLARE_EVENT_OneParam(UFriendsViewModel, FOnPresenceLoaded, bool /* bWasSuccessful */);
	DECLARE_EVENT_OneParam(UFriendsViewModel, FOnSinglePresenceChanged, UFriend* /* ChangedFriend */);

public:
	FOnLoadingFriendsList& OnLoadingFriendsList() const { return OnLoadingFriendsListEvent; }
	FOnFriendsListLoaded& OnFriendsListLoaded() const { return OnFriendsListLoadedEvent; }
	FOnLoadingPresenceList& OnLoadingPresenceList() const { return OnLoadingPresenceListEvent; }
	FOnPresenceLoaded& OnPresenceLoaded() const { return OnPresenceLoadedEvent; }
	FOnSinglePresenceChanged& OnSinglePresenceChanged() const { return OnSinglePresenceChangedEvent; }
	const TArray<UFriend*>& GetFriendsList() const { return Friends; }

protected:
	virtual bool Initialize_Implementation(const UWorld* InWorld) override;
	virtual void Invalidate_Implementation() override;
	virtual void BeginDestroy() override;
	
private:
	void HandleFriendsListFetched(bool bWasSuccessful);

	void HandlePresenceDataFetched(bool bWasSuccessful);

	void HandleSinglePresenceChanged(const FGuid& UserId, const TSharedRef<FOnlineUserPresence>& Presence);

	mutable FOnLoadingFriendsList OnLoadingFriendsListEvent;
	mutable FOnFriendsListLoaded OnFriendsListLoadedEvent;
	mutable FOnLoadingPresenceList OnLoadingPresenceListEvent;
	mutable FOnPresenceLoaded OnPresenceLoadedEvent;
	mutable FOnSinglePresenceChanged OnSinglePresenceChangedEvent;
	
	TSoftObjectPtr<UOnlineServicesSubsystem> OnlineServices;
	TMap<FGuid, uint32> GuidToFriendsIndex;
	
	UPROPERTY()
	TArray<UFriend*> Friends;
};
