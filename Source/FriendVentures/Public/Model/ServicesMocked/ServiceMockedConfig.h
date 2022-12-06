#pragma once
#include "Model/Services/OnlineServicesSubsystemConfig.h"
#include "OnlineFriendsMocked.h"
#include "OnlinePresenceMocked.h"
#include "ServiceMockedConfig.generated.h"

/**
 * Provides config and initialization for the mocked implementation.
 */
UCLASS()
class FRIENDVENTURES_API UServiceMockedConfig final : public UOnlineServicesSubsystemConfig
{
	GENERATED_BODY()
public:
	virtual TSharedPtr<IOnlineFriends> NewFriendsService() const override
	{
		return TSharedPtr<IOnlineFriends>{ new FOnlineFriendsMocked };
	}

	virtual TSharedPtr<IOnlinePresence> NewPresenceService() const override
	{
		return TSharedPtr<IOnlinePresence>{ new FOnlinePresenceMocked };
	}

	UDataTable* GetFriendsTable() const
	{
		return FriendsTable;
	}

	uint8 GetAverageSecondsToChangeOnlineState() const
	{
		return AverageSecondsToChangeOnlineState;
	}
	
private:
	/**
	 * Holds the table with the information of all friends of the player simulating a remote backend,
	 * this mimics a database source.
	 */
	UPROPERTY(EditDefaultsOnly)
	UDataTable* FriendsTable;

	/**
	 * Indicates the amount of seconds to wait in average before a friend changes his/her connected status.
	 * Its default value can be set in the editor (avoiding hard-coding it in code and recompilations on changes).
	 */
	UPROPERTY(EditDefaultsOnly)
	uint8 AverageSecondsToChangeOnlineState = 5;
};
