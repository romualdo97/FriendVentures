// Fill out your copyright notice in the Description page of Project Settings.


#include "ViewModel/Data/Friend.h"

FString UFriend::GetNickname() const
{
	if (!UserInfo)
	{
		return "";
	}
		
	return UserInfo->GetDisplayName();
}

FString UFriend::GetRealName() const
{
	if (!UserInfo)
	{
		return "";
	}

	return UserInfo->GetRealName();
}

uint8 UFriend::GetLevel() const
{
	if (!UserInfo)
	{
		return 0;
	}

	return UserInfo->GetLevel();
}

bool UFriend::GetIsOnline() const
{
	if (!PresenceInfo)
	{
		return false;
	}
	return PresenceInfo->bIsOnline;
}

bool UFriend::GetIsPlaying() const
{
	if (!PresenceInfo)
	{
		return false;
	}
	return PresenceInfo->bIsPlaying;
}

bool UFriend::GetIsPlayingThisGame() const
{
	if (!PresenceInfo)
	{
		return false;
	}
	return PresenceInfo->bIsPlayingThisGame;
}

bool UFriend::GetIsJoinable() const
{
	if (!PresenceInfo)
	{
		return false;
	}
	return PresenceInfo->bIsJoinable;
}

bool UFriend::GetHasVoiceSupport() const
{
	if (!PresenceInfo)
	{
		return false;
	}
	return PresenceInfo->bHasVoiceSupport;
}

FDateTime UFriend::GetLastOnline() const
{
	if (!PresenceInfo)
	{
		return FDateTime::MinValue();
	}
	return PresenceInfo->LastOnline;
}

void UFriend::BeginDestroy()
{
	UserInfo = nullptr;
	PresenceInfo = nullptr;

	Super::BeginDestroy();
}
