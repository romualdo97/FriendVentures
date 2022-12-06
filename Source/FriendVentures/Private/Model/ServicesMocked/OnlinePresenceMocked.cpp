// Fill out your copyright notice in the Description page of Project Settings.


#include "Model/ServicesMocked/OnlinePresenceMocked.h"

#include "CoreTypes.h"
#include "FriendVentures/FriendVentures.h"
#include "Model/ServicesMocked/ServiceMockedConfig.h"


void FOnlinePresenceMocked::SetPresence(FGuid User, const FOnlineUserPresence& NewPresence,
                                        const FOnPresenceTaskCompleteDelegate& Delegate)
{
	// Does nothing on the mocked service
}

void FOnlinePresenceMocked::QueryPresence(const TArray<TSharedRef<FGuid>>& UserIds,
	const FOnPresenceTaskCompleteDelegate& Delegate)
{
	// Check OnlineFriendsMocked.cpp for a detailed discussion on this
	OnPresenceTaskCompletedCallback = Delegate;
	
	const TFuture<void> Future = Async(
		EAsyncExecution::TaskGraph,
		[this, UserIds]()
		{
			// Capture userIds by value to protect against access violation
			// in case the caller created a temporal array on the stack when
			// started this operation
			constexpr float DelayInSeconds = 1.0f;
			FPlatformProcess::Sleep(DelayInSeconds); // Simulate some latency
			this->FetchMockedData(UserIds);
		},
		[this]()
		{
			AsyncTask(
				ENamedThreads::GameThread,
				[this]()
				{
					this->OnPresenceTaskCompletedCallback.ExecuteIfBound(true);
					this->OnPresenceTaskCompletedCallback.Unbind();
					this->RegisterTimer();
				}				
			);
		}
	);
}

bool FOnlinePresenceMocked::GetCachedPresence(const FGuid& UserId,
                                              TSharedPtr<FOnlineUserPresence>& OutPresence)
{
	if (CachedPresence.Contains(UserId))
	{
		OutPresence = CachedPresence[UserId];
		return true;
	}

	return false;	
}

void FOnlinePresenceMocked::FetchMockedData(const TArray<TSharedRef<FGuid>>& UsersId)
{
	CachedPresence.Reset();
	
	for (const TSharedRef<FGuid>& UserId : UsersId)
	{
		FOnlineUserPresence* PresenceData = new FOnlineUserPresence;
		PresenceData->bIsOnline = FMath::RandBool();
		PresenceData->bIsPlaying = FMath::RandBool();
		PresenceData->bIsPlayingThisGame = FMath::RandBool();
		PresenceData->bIsJoinable = FMath::RandBool();
		PresenceData->bHasVoiceSupport = FMath::RandBool();
		PresenceData->LastOnline = FDateTime::UtcNow();

		// Mimic last connection
		if (!PresenceData->bIsOnline)
		{
			PresenceData->LastOnline -= FTimespan::FromDays(FMath::RandRange(1, 360));
		}		

		CachedPresence.Add(*UserId, TSharedRef<FOnlineUserPresence>{ PresenceData });
	}

	UE_LOG(LogFriendVentures, Log, TEXT("Fetched online presence data for given array..."));
}

void FOnlinePresenceMocked::RegisterTimer()
{
	// If the global time manager is not valid, return.
	// If valid, pick a random time for the next selection.
	// Set the timer for the next selection.
	
	const UServiceMockedConfig* RemoteSubsystemConfig = GetConfig<UServiceMockedConfig>();
	if(RemoteSubsystemConfig == nullptr)
	{
		UE_LOG(LogFriendVentures, Error, TEXT("RemoteSubsystemConfig is null"));
		return;
	}

	if(!IsValid(SubsystemOwner->GetGameInstance()))
	{
		UE_LOG(LogFriendVentures, Error, TEXT("GameInstance is null"));
		return;
	}
	
	static constexpr float SecondsToOffset = 2.0F;
	const uint8 AverageSecondsToChangeOnlineState = RemoteSubsystemConfig->GetAverageSecondsToChangeOnlineState();
	const float SecondsToWait = FMath::RandRange(AverageSecondsToChangeOnlineState - SecondsToOffset, AverageSecondsToChangeOnlineState + SecondsToOffset);
	
	FTimerHandle TimerHandle;
	FTimerDelegate OnceCompleted;
	OnceCompleted.BindRaw(this, &FOnlinePresenceMocked::PickFriendToChangeStatus);
	SubsystemOwner->GetGameInstance()->GetTimerManager().SetTimer(TimerHandle, OnceCompleted, SecondsToWait, false);
}

void FOnlinePresenceMocked::PickFriendToChangeStatus()
{
	
	if(!SubsystemOwner.IsValid() && !IsValid(SubsystemOwner->GetGameInstance()))
	{
		UE_LOG(LogFriendVentures, Error, TEXT("GameInstance is null"));
		return;
	}

	SubsystemOwner->GetGameInstance()->GetTimerManager().ClearAllTimersForObject(this);

	if(const int32 Count = CachedPresence.Num(); Count > 0)
	{
		// Mimic new presence data returned by client-server connection
		const TSharedRef<FOnlineUserPresence> NewPresenceData{ new FOnlineUserPresence };
		NewPresenceData->bIsOnline = FMath::RandBool();
		NewPresenceData->bIsPlaying = FMath::RandBool();
		NewPresenceData->bIsPlayingThisGame = FMath::RandBool();
		NewPresenceData->bIsJoinable = FMath::RandBool();
		NewPresenceData->bHasVoiceSupport = FMath::RandBool();
		NewPresenceData->LastOnline = FDateTime::UtcNow();

		// Pick a random entry from the cache
		int32 CurrentIndex = 0;
		const int32 SelectedPresenceIndex = FMath::RandRange(0,  Count - 1);
		TTuple<FGuid, TSharedPtr<FOnlineUserPresence>> SelectedCacheEntry;
		for (const auto& CacheTuple : CachedPresence)
		{
			if (CurrentIndex == SelectedPresenceIndex)
			{
				SelectedCacheEntry = CacheTuple;
			}
			++CurrentIndex;
		}

		// Inform that new presence is available
		if (SelectedCacheEntry.Value.IsValid())
		{
			if (*SelectedCacheEntry.Value != *NewPresenceData)
			{
				if (SelectedCacheEntry.Value->bIsOnline != NewPresenceData->bIsOnline)
				{
					NewPresenceData->LastOnline = FDateTime::UtcNow();
				}
				*SelectedCacheEntry.Value = *NewPresenceData;
				
				BroadcastOnPresenceReceivedEvent(SelectedCacheEntry.Key, SelectedCacheEntry.Value.ToSharedRef());
			}
		}
	}
	
	RegisterTimer();
}

void FOnlinePresenceMocked::Initialize(UOnlineServicesSubsystem* InSubsystemOwner)
{
	IOnlinePresence::Initialize(InSubsystemOwner);
	UE_LOG(LogFriendVentures, Log, TEXT("Initialized FOnlinePresenceMocked..."));
}

