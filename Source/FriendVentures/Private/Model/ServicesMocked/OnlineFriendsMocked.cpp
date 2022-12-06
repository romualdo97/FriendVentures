// Fill out your copyright notice in the Description page of Project Settings.


#include "Model/ServicesMocked/OnlineFriendsMocked.h"

#include "Engine/DataTable.h"
#include "FriendVentures/FriendVentures.h"
#include "Model/ServicesMocked/Data/FriendDataTableRow.h"
#include "Model/ServicesMocked/ServiceMockedConfig.h"

bool FOnlineFriendsMocked::ReadFriendsList(const FOnReadFriendsListComplete& Delegate)
{
	// To allow "Delegate" execution we need to store a copy on "this" object, otherwise we would
	// get an access violation exception when executing the delegate because the delegate object was created on the stack and will be
	// executed after the stack of the caller and callee function were freed-up
	OnReadFriendsListCompleteCallback = Delegate;
	
	// ORIGINAL PEDRO NOTE:
	// When you cannot (or do not) create a "weak" lambda expression, it's a common practice to use a weak object pointer
	// to "this", instead, given that the expression may be executed and evaluated on a different thread for the async
	// operation, and even when it gets back to the main thread, "this" may not exist. Since this is a simple exercise
	// you can go ahead an replace "WeakThis" for "this" and see how it goes (possibly, with no memory hassles).
	// TWeakObjectPtr<URemoteSubsystem> WeakThis = this;

	// ROMUALDO: Capturing "this" directly because this is not an UObject, also as this object
	// is handled manually we can't use the TWeakObjectPtr nor the TWeakPtr as noted by Tim Sylvester on the
	// following thread:
	// > Your note about threads is worrisome. I'm pretty sure you have a race condition
	// > that can lead to a crash. If a thread deletes an object and zeros the pointer,
	// > another thread could make a call through that pointer between those two operations,
	// > leading to this being non-null and also not valid, resulting in a crash. Similarly,
	// > if a thread calls a method while another thread is in the middle of creating
	// > the object, you may also get a crash.
	//
	// > Short answer, you really need to use a mutex or something to synchonize access
	// > to this variable. You need to ensure that this is never null or you're going
	// > to have problems.
	// https://stackoverflow.com/questions/1844005/checking-if-this-is-null

	// NOTE: The following code is potentially not thread-safe, must be improved, continue with precaution
	// however for the sake of this exercise we can assume that this is the only thread modifying data
	// so is kind-of safe
	const TFuture<void> Future = Async(
		EAsyncExecution::TaskGraph,
		[this]()
		{
			constexpr float DelayInSeconds = 1.0f;
			FPlatformProcess::Sleep(DelayInSeconds); // Simulate some latency
			this->FetchMockedData();
		},
		[this]()
		{
			// Since the callback is executed in the same thread than the async operation, we switch to the main one.
			AsyncTask(
				ENamedThreads::GameThread,
				[this]()
				{
					this->OnReadFriendsListCompleteCallback.ExecuteIfBound(this->bFetchSucceed);
					this->OnReadFriendsListCompleteCallback.Unbind();
				}
			);
		}
	);
	
	UE_LOG(LogFriendVentures, Log, TEXT("Async ReadFriendsList started..."));

	// Request was started successfully
	return true;
}

bool FOnlineFriendsMocked::GetFriendsList(TArray<TSharedRef<FOnlineUser>>& OutFriends)
{
	if (!bFetchSucceed)
	{
		UE_LOG(LogFriendVentures, Error, TEXT("Friends data was not retrieved correctly"));
		return false;
	}

	// Assign copy operator
	OutFriends = FriendsList;
	return true;
}

void FOnlineFriendsMocked::FetchMockedData()
{
	FriendsList.Reset(0);
	bFetchSucceed = true;

	const UServiceMockedConfig* Config = GetConfig<UServiceMockedConfig>();
	if (!Config)
	{
		UE_LOG(LogFriendVentures, Error, TEXT("Failed fetching data because config is not available"));
		bFetchSucceed = false;
		return;
	}
	
	if (!IsValid(Config->GetFriendsTable()))
	{
		UE_LOG(LogFriendVentures, Error, TEXT("Friends table is invalid"));
		bFetchSucceed = false;
		return;
	}
	
	UE_LOG(LogFriendVentures, Log, TEXT("Fetching initial data..."));
	
	const TMap<FName, uint8*> RowMaps = Config->GetFriendsTable()->GetRowMap();
	for (const auto [RowKeyName, RowValuePtr] : RowMaps) // We can use "structured bindings" (since C++17)
	{
		// We already have the data associated to the raw, so all we need to do is reinterpret it correctly (which will be
		// faster than using "FindRow", instead).
		const FFriendDataTableRow* FriendsRow = reinterpret_cast<FFriendDataTableRow*>(RowValuePtr);
		if (FriendsRow == nullptr)
		{
			continue;
		}

		// Load the data fetched from "database"
		FriendsList.Add
		(
			TSharedRef<FOnlineUser>{ 
				new FOnlineUser{
					FriendsRow->Nickname,
					FriendsRow->RealName,
					FriendsRow->Level
				}
			}
		);
	}

	UE_LOG(LogFriendVentures, Log, TEXT("Initial data fetched..."));
}

void FOnlineFriendsMocked::Initialize(UOnlineServicesSubsystem* InSubsystemOwner)
{
	IOnlineFriends::Initialize(InSubsystemOwner);
	UE_LOG(LogFriendVentures, Log, TEXT("Initialized FOnlineFriendsMocked..."));
}
