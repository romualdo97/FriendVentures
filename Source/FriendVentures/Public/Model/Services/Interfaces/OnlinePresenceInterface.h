#pragma once

#include "BaseServiceInterface.h"

struct FGuid;

/**
 * Presence info for an online user returned via IOnlinePresence interface.
 */
class FOnlineUserPresence
{
public:
	uint32 bIsOnline:1;
	uint32 bIsPlaying:1;
	uint32 bIsPlayingThisGame:1;
	uint32 bIsJoinable:1;
	uint32 bHasVoiceSupport:1;
	FDateTime LastOnline;

	bool operator==(const FOnlineUserPresence& Rhs) const
	{
		return FMemory::Memcmp(this, &Rhs, sizeof(this)) == 0;
	}

	bool operator!=(const FOnlineUserPresence& Rhs) const
	{
		return !(*this == Rhs);
	}
};

/**
 * Presence services retrieves info about the online status of a user.
 */
class FRIENDVENTURES_API IOnlinePresence : public IBaseService
{
	/**
	 * Event executed when new presence data is available for a user.
	 *
	 * @param UserId The unique id of the user whose presence was received.
	 * @param Presence The received presence
	 */
	DECLARE_EVENT_TwoParams(IOnlinePresence, FOnPresenceReceived, const FGuid& /*UserId*/, const TSharedRef<FOnlineUserPresence>& /*Presence*/);
	
public:
	/** Virtual destructor to allow proper cleanup on implementors */
	virtual ~IOnlinePresence() override
	{
		if (OnPresenceReceivedEvent.IsBound())
		{
			OnPresenceReceivedEvent.Clear();
		}
	}

	/**
	 * Delegate executed when setting or querying presence for a user has completed.
	 *
	 * @param bWasSuccessful true if the async action completed without error, false if there was an error.
	 */
	DECLARE_DELEGATE_OneParam(FOnPresenceTaskCompleteDelegate, const bool /*bWasSuccessful*/);

	/**
	 * Update the presence info of a specific user, this is an async workload
	 * 
	 * @param User The unique id of the user whose presence is being set.
	 * @param NewPresence The collection of state and key/value pairs to set as the user's presence data.
	 * @param Delegate The delegate to be executed when the potentially asynchronous set operation completes.
	 */
	virtual void SetPresence(FGuid User,
		const FOnlineUserPresence& NewPresence,
		const FOnPresenceTaskCompleteDelegate& Delegate = FOnPresenceTaskCompleteDelegate()) = 0;
	
	/**
	 * Starts an async operation that will update the cache with presence data from all users in the Users array.
	 *
	 * @param UserIds The list of unique ids of the users to query for presence information.
	 * @param Delegate The delegate to be executed when the potentially asynchronous query operation completes.
	 */
	virtual void QueryPresence(const TArray<TSharedRef<FGuid>>& UserIds,
	    const FOnPresenceTaskCompleteDelegate& Delegate = FOnPresenceTaskCompleteDelegate()) = 0;
	
	/**
	 * Gets the cached presence information for a user.
	 * @param UserId Unique id of the user to query for presence information.
	 * @param OutPresence If found, a shared pointer to the cached presence data for User will be stored here.
	 */
	virtual bool GetCachedPresence(const FGuid& UserId, TSharedPtr<FOnlineUserPresence>& OutPresence) = 0;
	
	/**
	 * Event executed when new presence data is available for a user.
	 *
	 * @return A reference to the event object that will be executed.
	 */
	FOnPresenceReceived& OnPresenceReceived() const
	{
		return OnPresenceReceivedEvent;
	}
		
protected:	
	void BroadcastOnPresenceReceivedEvent(const FGuid& UserId, const TSharedRef<FOnlineUserPresence>& Presence) const
	{
		OnPresenceReceivedEvent.Broadcast(UserId, Presence);
	}
	
private:
	mutable FOnPresenceReceived OnPresenceReceivedEvent;
};
