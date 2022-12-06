// Fill out your copyright notice in the Description page of Project Settings.


#include "ViewModel/FriendsViewModel.h"

#include "Algo/ForEach.h"
#include "FriendVentures/FriendVentures.h"
#include "Model/Services/OnlineServicesSubsystem.h"
#include "Model/Services/Interfaces/OnlineFriendsInterface.h"

bool UFriendsViewModel::Initialize_Implementation(const UWorld* InWorld)
{
	if (const UOnlineServicesSubsystem* RemoteOnlineServices = UOnlineServicesSubsystem::Get(InWorld))
	{
		OnlineServices = RemoteOnlineServices;

		// Starts async task to get the friends list
		if (const TSharedPtr<IOnlineFriends> FriendsService = OnlineServices->GetFriendsService())
		{
			// Broadcast that friends started loading
			OnLoadingFriendsListEvent.Broadcast();

			// Start request
			IOnlineFriends::FOnReadFriendsListComplete OnceCompleted;
			OnceCompleted.BindUObject(this, &ThisClass::HandleFriendsListFetched);
			FriendsService->ReadFriendsList(OnceCompleted);
			return true;
		}
	}

	UE_LOG(LogFriendVentures, Error, TEXT("UFriendsViewModel failed getting the UOnlineServicesSubsystem object."))
	return false;
}

void UFriendsViewModel::Invalidate_Implementation()
{
	// Clear all handlers for all events.

	// You may wonder why "this" is being checked here and not before, say, at the beginning of the method. Well, this
	// is due to the garbage collector when exiting PIE and the editor. Sometimes, when executing a function that clears
	// the local handlers of a delegate or an event, the object gets collected and an error occurs. The interesting part,
	// is that, in case of this example, it does not happen -at least, on my tests- when any of the arrays are emptied.
	// Cases like this could potentially happen even with functions like "IsValid(MyInstanceOfAnUObject)" where is valid
	// may cause an error when the object been checked gets somehow corrupted in memory. So, please be aware and do not
	// panic when it happens (workarounds may exist).
	if (this && OnLoadingFriendsListEvent.IsBound())
	{
		OnLoadingFriendsListEvent.Clear();		
	}

	if (this && OnFriendsListLoadedEvent.IsBound())
	{
		OnFriendsListLoadedEvent.Clear();
	}

	if (this && OnLoadingPresenceListEvent.IsBound())
	{
		OnLoadingPresenceListEvent.Clear();
	}
	
	if (this && OnPresenceLoadedEvent.IsBound())
	{
		OnPresenceLoadedEvent.Clear();
	}
	
	if (this && OnSinglePresenceChangedEvent.IsBound())
	{
		OnSinglePresenceChangedEvent.Clear();
	}
}

void UFriendsViewModel::BeginDestroy()
{
	Friends.Reset();

	if (OnlineServices)
	{
		if (const TSharedPtr<IOnlinePresence> Presence = OnlineServices->GetPresenceService())
		{
			Presence->OnPresenceReceived().RemoveAll(this);
		}
	}
	
	UObject::BeginDestroy();
}

void UFriendsViewModel::HandleFriendsListFetched(const bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogFriendVentures, Error, TEXT("UFriendsViewModel can't fetch friends list data because operation didn't completed correctly"));
		
		// Broadcast that friends failed loading
		OnFriendsListLoadedEvent.Broadcast(false);
		return;
	}

	// Check if friends service is available
	const TSharedPtr<IOnlineFriends> FriendsService = OnlineServices->GetFriendsService();
	if (FriendsService == nullptr)
	{
		UE_LOG(LogFriendVentures, Warning, TEXT("UFriendsViewModel can't fetch friends list data because the service is not registered"));

		// Broadcast that friends failed loading
		OnFriendsListLoadedEvent.Broadcast(false);
		return;
	}

	// Reset the cache
	Friends.Reset(); // TODO: Avoid re-instancing, re-use if available

	// An owned list of friend ids
	TArray<TSharedRef<FGuid>> FriendIds;
	
	// Get Friends data and wrap it into an UObject
	if (TArray<TSharedRef<FOnlineUser>> FetchedFriendsData; FriendsService->GetFriendsList(FetchedFriendsData))
	{
		for (const TSharedRef<FOnlineUser>& UserData : FetchedFriendsData)
		{
			if (!UserData->GetUserId().IsValid())
			{
				continue;
			}

			// Creating copy of friend guids on free-store to query presence data
			FriendIds.Add(TSharedRef<FGuid>
			{
				new FGuid{ UserData->GetUserId() }
			});
			
			// Create an UFriend which wraps the model data
			UFriend* FriendWrapper = NewObject<UFriend>(this);
			FriendWrapper->UserInfo = UserData;
			FriendWrapper->PresenceInfo = nullptr;
			Friends.Add(FriendWrapper);
		}
	}
	
	UE_LOG(LogFriendVentures, Log, TEXT("UFriendsViewModel fetched friends data"));

	// Broadcast that friends loaded correctly
	OnFriendsListLoadedEvent.Broadcast(true);

	// Broadcast that presence started loading
	OnLoadingPresenceListEvent.Broadcast();
	
	// Check if presence service is available
	const TSharedPtr<IOnlinePresence> PresenceService = OnlineServices->GetPresenceService();
	if (PresenceService == nullptr)
	{
		UE_LOG(LogFriendVentures, Warning, TEXT("UFriendsViewModel can't fetch friends presence data because the service is not registered"));

		// Broadcast that presence failed loading
		OnPresenceLoadedEvent.Broadcast(false);
		return;
	}
	
	// Register callback for when Presence info of friends gets fetched
	IOnlinePresence::FOnPresenceTaskCompleteDelegate OnceCompleted;
	OnceCompleted.BindUObject(this, &ThisClass::HandlePresenceDataFetched);
	
	// Query the presence info of the friends
	PresenceService->QueryPresence(FriendIds, OnceCompleted);
}

void UFriendsViewModel::HandlePresenceDataFetched(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogFriendVentures, Error, TEXT("UFriendsViewModel can't fetch friends presence data because operation didn't completed correctly"));

		// Broadcast that presence failed loading
		OnPresenceLoadedEvent.Broadcast(false);
		return;
	}
	
	// Check if presence service is available
	const TSharedPtr<IOnlinePresence> PresenceService = OnlineServices->GetPresenceService();
	if (PresenceService == nullptr)
	{
		UE_LOG(LogFriendVentures, Warning, TEXT("UFriendsViewModel can't fetch friends presence data because the service is not registered"));

		// Broadcast that presence failed loading
		OnPresenceLoadedEvent.Broadcast(false);
		return;
	}
	
	// Get the cached presence for each friend using id
	Algo::ForEach(Friends, [&PresenceService](UFriend* Friend)
	{
		// Get the presence info
		TSharedPtr<FOnlineUserPresence> OutPresence;
		PresenceService->GetCachedPresence
		(
			Friend->UserInfo->GetUserId(),
			OutPresence
		);
	
		// Populate the presence info
		Friend->PresenceInfo = OutPresence;
	});

	// Broadcast that presence loaded correctly
	OnPresenceLoadedEvent.Broadcast(true);
	
	// Presence service will keep updating
	PresenceService->OnPresenceReceived().AddUObject(this, &ThisClass::HandleSinglePresenceChanged);
	
	UE_LOG(LogFriendVentures, Log, TEXT("UFriendsViewModel fetched friends presence data"));
}

void UFriendsViewModel::HandleSinglePresenceChanged(const FGuid& UserId,
	const TSharedRef<FOnlineUserPresence>& Presence)
{
	using UFriendPtr = UFriend*;
	
	// Ptr to the UObject* associated to the user
	const UFriendPtr* ChangedFriend = Friends.FindByPredicate([&UserId](const UFriend* Friend)
	{
		return Friend->UserInfo->GetUserId() == UserId;
	});

	// Inform which friend changed presence
	OnSinglePresenceChangedEvent.Broadcast(*ChangedFriend);
	
	UE_LOG(LogFriendVentures, Log, TEXT("ViewModel New Presence received"));
}
