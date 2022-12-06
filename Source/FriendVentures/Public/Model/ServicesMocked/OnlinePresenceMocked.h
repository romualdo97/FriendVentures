// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Model/Services/Interfaces/OnlinePresenceInterface.h"

class UDataTable;

/**
 * Get/Post data from/to the backend related to the presence status of a user, i.e. online, offline, playing, etc...
 */
class FRIENDVENTURES_API FOnlinePresenceMocked final : public IOnlinePresence
{
public:
	virtual void SetPresence(FGuid User, const FOnlineUserPresence& NewPresence, const FOnPresenceTaskCompleteDelegate& Delegate) override;

	virtual void QueryPresence(const TArray<TSharedRef<FGuid>>& UserIds, const FOnPresenceTaskCompleteDelegate& Delegate) override;
	
	virtual bool GetCachedPresence(const FGuid& UserId, TSharedPtr<FOnlineUserPresence>& OutPresence) override;
	
protected:
	virtual void Initialize(UOnlineServicesSubsystem* InSubsystemOwner) override;

private:
	void FetchMockedData(const TArray<TSharedRef<FGuid>>& UsersId);

	void RegisterTimer();

	void PickFriendToChangeStatus();
	
	// Simulates a simple cache
	TMap<FGuid, TSharedRef<FOnlineUserPresence>> CachedPresence;

	FOnPresenceTaskCompleteDelegate OnPresenceTaskCompletedCallback;
};
