// Fill out your copyright notice in the Description page of Project Settings.


#include "View/DefaultHUDWidget.h"

#include "FriendVentures/FriendVentures.h"
#include "ViewModel/FriendsViewModel.h"
#include "ViewModel/Data/Friend.h"

void UDefaultHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if(!IsValid(FriendsViewModelClass))
	{
		UE_LOG(LogFriendVentures, Error, TEXT("FriendViewModel is invalid."));
		return;
	}

	FriendsViewModel = NewObject<UFriendsViewModel>(this, FriendsViewModelClass);
	FriendsViewModel->Initialize(*this);

	if(!IsValid(FriendsViewModel))
	{
		UE_LOG(LogFriendVentures, Error, TEXT("FriendViewModel is invalid."));
		return;
	}

	// Register listeners
	FriendsViewModel->OnLoadingFriendsList().AddUObject(this, &ThisClass::OnLoadingFriendsList);
	FriendsViewModel->OnFriendsListLoaded().AddUObject(this, &ThisClass::OnFriendsListLoaded);
	FriendsViewModel->OnLoadingPresenceList().AddUObject(this, &ThisClass::OnLoadingPresenceList);
	FriendsViewModel->OnPresenceLoaded().AddUObject(this, &ThisClass::OnPresenceLoaded);
	FriendsViewModel->OnSinglePresenceChanged().AddUObject(this, &ThisClass::OnSinglePresenceChanged);
	
	UE_LOG(LogFriendVentures, Log, TEXT("DefaultHudWidget initialized..."));
}

void UDefaultHUDWidget::NativeDestruct()
{
	Super::NativeDestruct();
}


const TArray<UFriend*>& UDefaultHUDWidget::GetFriendsList() const
{
	return FriendsViewModel->GetFriendsList();
}
