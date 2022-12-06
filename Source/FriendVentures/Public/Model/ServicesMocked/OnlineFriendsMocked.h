// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Model/Services/Interfaces/OnlineFriendsInterface.h"

class UDataTable;

/**
 * Get/Post data from/to the backend related to the friends list
 */
class FRIENDVENTURES_API FOnlineFriendsMocked final : public IOnlineFriends
{
public:
	virtual bool ReadFriendsList(const FOnReadFriendsListComplete& Delegate) override;

	virtual bool GetFriendsList(TArray<TSharedRef<FOnlineUser>>& OutFriends) override;

protected:
	virtual void Initialize(UOnlineServicesSubsystem* InSubsystemOwner) override;

private:
	void FetchMockedData();
	
	/**
	 * True if fetching succeed
	 */
	bool bFetchSucceed{true};
	
	/**
	 * Loaded friends list
	 */
	TArray<TSharedRef<FOnlineUser>> FriendsList;

	/**
	 * A callback to inform when the friends list was fetched
	 */
	FOnReadFriendsListComplete OnReadFriendsListCompleteCallback;
};
