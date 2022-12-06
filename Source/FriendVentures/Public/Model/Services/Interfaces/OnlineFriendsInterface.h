#pragma once
#include "BaseServiceInterface.h"

/**
 * Base for all online user info
 */
class FOnlineUser
{
public:
	/**
	 * Constructor
	 */
	FOnlineUser(const FString& InNickname, const FString& InRealName, const uint8 InLevel)
	: Nickname(InNickname), RealName(InRealName), Level(InLevel)
	{
		Id = FGuid::NewGuid();
	}
		
	/** 
	 * @return Id associated with the user account provided by the online service during registration 
	 */
	const FGuid& GetUserId() const { return Id; }
	
	/**
	 * @return the real name for the user if known
	 */
	const FString& GetRealName() const { return RealName; }
	
	/**
	 * @return the nickname of the user if known
	 */
	const FString& GetDisplayName() const { return Nickname; }
	
	/**
	 * @return the level of the player
	 */
	uint8 GetLevel() const { return Level; }

private:
	FGuid Id;
	FString Nickname{};
	FString RealName{};
	uint8 Level{};
};

/**
 * Online Friends service retrieves user info of to current player friends list.
 */
class FRIENDVENTURES_API IOnlineFriends : public IBaseService
{	
public:
	/**
	 * Delegate used when the friends read request has completed
	 *
	 * @param bWasSuccessful True if the read operation succeed.
	 */
	DECLARE_DELEGATE_OneParam(FOnReadFriendsListComplete, bool /*bWasSuccessful*/);
		
	/**
	 * Starts an async task that reads the named friends list for the current player 
	 *
	 * @param Delegate Called when the list has been fetched
	 *
	 * @return true if the read request was started successfully, false otherwise
	 */
	virtual bool ReadFriendsList(const FOnReadFriendsListComplete& Delegate = FOnReadFriendsListComplete()) = 0;

	/**
	 * Copies the list of friends for the player previously retrieved from the online service
	 * 
	 * @param OutFriends [out] array that receives the copied data
	 * @return true if friends list was found
	 */
	virtual bool GetFriendsList(TArray< TSharedRef<FOnlineUser> >& OutFriends) = 0;
};
