// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Model/Services/Interfaces/OnlineFriendsInterface.h"
#include "Model/Services/Interfaces/OnlinePresenceInterface.h"
#include "UObject/Object.h"
#include "Friend.generated.h"

/**
 * Wraps model data about a friend, so the data integrates nicely with an UListView
 */
UCLASS(BlueprintType, meta = (DisableNativeTick))
class FRIENDVENTURES_API UFriend final : public UObject
{
	GENERATED_BODY()

public:
	TSharedPtr<FOnlineUser> UserInfo;
	TSharedPtr<FOnlineUserPresence> PresenceInfo;

	UFUNCTION(BlueprintCallable)
	FString GetNickname() const;

	UFUNCTION(BlueprintCallable)
	FString GetRealName() const;

	UFUNCTION(BlueprintCallable)
	uint8 GetLevel() const;

	UFUNCTION(BlueprintCallable)
	bool GetIsOnline() const;

	UFUNCTION(BlueprintCallable)
	bool GetIsPlaying() const;

	UFUNCTION(BlueprintCallable)
	bool GetIsPlayingThisGame() const;

	UFUNCTION(BlueprintCallable)
	bool GetIsJoinable() const;

	UFUNCTION(BlueprintCallable)
	bool GetHasVoiceSupport() const;

	UFUNCTION(BlueprintCallable)
	FDateTime GetLastOnline() const;

	virtual void BeginDestroy() override;
};
