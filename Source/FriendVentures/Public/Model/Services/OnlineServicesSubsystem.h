// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "OnlineServicesSubsystem.generated.h"

class UOnlineServicesSubsystemConfig;
class IOnlineFriends;
class IOnlinePresence;

/**
 * Static service locator which retrieves access to distinct game online services
 */
UCLASS(meta = (DisableNativeTick))
class FRIENDVENTURES_API UOnlineServicesSubsystem final : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UOnlineServicesSubsystem();
	
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem
	
	/**
	 * Return the instance of the OnlineServicesSubsystem, this is simply a shortcut
	 * which requests the Subsystem from the GameInstance.
	 *
	 * @param World	- the World object
	 * @return The instance of the OnlineServicesSubsystem
	 */
	static UOnlineServicesSubsystem* Get(const UWorld* World);

	/**
	 * Service used to fetch presence data about a player i.e.: (online, offline, playing, etc...)
	 *
	 * @return A pointer to the service
	 */
	TSharedPtr<IOnlinePresence> GetPresenceService() const
	{
		return PresenceService;
	}

	/**
	 * Service used to fetch user data about friends
	 *
	 * @return A pointer to the service
	 */
	TSharedPtr<IOnlineFriends> GetFriendsService() const
	{
		return FriendsService;
	}

	UOnlineServicesSubsystemConfig* GetConfig() const
	{
		return RemoteSubsystemConfig;
	}
	
private:
	TSharedPtr<IOnlineFriends> FriendsService{ nullptr };
	TSharedPtr<IOnlinePresence> PresenceService{ nullptr };

	/**
	 * Holds the configuration values for the subsystem.
	 * @remark Since the subsystem is non-blueprintable it cannot be set on the editor and must be fetched at runtime.
	 */
	UPROPERTY()
	UOnlineServicesSubsystemConfig* RemoteSubsystemConfig;
};
