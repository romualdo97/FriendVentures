// Fill out your copyright notice in the Description page of Project Settings.


#include "Model/Services/OnlineServicesSubsystem.h"
#include "Model/Services/Interfaces/OnlinePresenceInterface.h"
#include "Model/Services/OnlineServicesSubsystemConfig.h"
#include "FriendVentures/FriendVentures.h"
#include "Model/Services/Interfaces/OnlineFriendsInterface.h"

UOnlineServicesSubsystem::UOnlineServicesSubsystem() : UGameInstanceSubsystem()
{
	UE_LOG(LogFriendVentures, Log, TEXT("Fetching settings for Remote Subsystem ..."));

	const TCHAR* PathToConfigFile = TEXT("/Game/Services/OnlineServicesSubsystemConfig");
	static ConstructorHelpers::FObjectFinder<UOnlineServicesSubsystemConfig> ConfigProxy(PathToConfigFile);
	
	if (ConfigProxy.Succeeded())
	{
		RemoteSubsystemConfig = ConfigProxy.Object;
		UE_LOG(LogFriendVentures, Log, TEXT("Settings for Remote Subsystem fetched!"));

		// Take ownership of the service instances
		FriendsService = RemoteSubsystemConfig->NewFriendsService();
		if (FriendsService == nullptr)
		{
			UE_LOG(LogFriendVentures, Warning, TEXT("IOnlineFriends service not loaded"));
		}

		PresenceService = RemoteSubsystemConfig->NewPresenceService();
		if (PresenceService == nullptr)
		{
			UE_LOG(LogFriendVentures, Warning, TEXT("IOnlinePresence service not loaded"));
		}
		
		return;
	}

	UE_LOG(LogFriendVentures, Warning, TEXT("Failed to fetch settings for Remote Subsystem."));
}

void UOnlineServicesSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (FriendsService != nullptr)
	{
		FriendsService->Initialize(this);
	}

	if (PresenceService != nullptr)
	{
		PresenceService->Initialize(this);
	}
	
	check(GetGameInstance() != nullptr);
}

void UOnlineServicesSubsystem::Deinitialize()
{	
	Super::Deinitialize();
}

UOnlineServicesSubsystem* UOnlineServicesSubsystem::Get(const UWorld* World)
{
	if (World)
	{
		if (const UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<UOnlineServicesSubsystem>();
		}
	}
	return nullptr;
}
