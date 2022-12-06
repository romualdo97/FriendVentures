// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OnlineServicesSubsystemConfig.generated.h"

class IOnlineFriends;
class IOnlinePresence;

/**
 * Config file that provides services implementations to the OnlineServicesSubsystem
 */
UCLASS(Abstract)
class FRIENDVENTURES_API UOnlineServicesSubsystemConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	virtual TSharedPtr<IOnlineFriends> NewFriendsService() const
	{
		return nullptr;
	}

	virtual TSharedPtr<IOnlinePresence> NewPresenceService() const
	{
		return nullptr;
	}
};
