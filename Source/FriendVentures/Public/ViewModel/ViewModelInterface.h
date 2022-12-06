// Copyright Globant, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FriendVentures/FriendVentures.h"

class UGameInstance;

/**
* @brief	Contract that view-models must comply with.
* @remarks	In case the interface must be available for Blueprints, use UE's UInterface approach instead.
* @see		https://docs.unrealengine.com/4.27/en-US/ProgrammingAndScripting/GameplayArchitecture/Interfaces/
*/
class FRIENDVENTURES_API IViewModel
{
	// Here we could declare events, for instance, to notify when the service is initialized or invalidated.
	
protected:
	/**
	 * @brief	Default constructor for the contract.
	 * @remark	We could be tempted to do some initialization here, but given we have a GC that discretionary recollects
	 *			memory, it's better to handle it in an independent function and just use C++11 way to declare a default one.
	 */
	IViewModel() = default;

	/**
	 * @brief	Destructor for the contract.
	 * @remark	There is no guarantee for the destructor to be executed in PIE.
	 */
	virtual ~IViewModel()
	{
		// Mark the view-model as not initialized.
		
		bIsInitialized = false;
		UE_LOG(LogFriendVentures, Warning, TEXT("ViewModel destructed."));
	}

	/**
	 * @brief					Called by the Initialize function.
	 * @param					InWorld The world object 
	 * @return					Returns TRUE if initialization succeeds; otherwise, FALSE.
	 * @remark					Pure virtual functions force a class to be abstract and therefore cannot be marked as UClass.
	 */
	virtual bool Initialize_Implementation(const UWorld* InWorld) = 0;
	
	/**
	 * @brief	Called by the Invalidate function.
	 * @remark	Pure virtual functions force a class to be abstract and therefore cannot be marked as UClass.
	 */
	virtual void Invalidate_Implementation() = 0;
	
public:
	/**
	 * @brief						Initializes the service.
	 * @param WorldContextObject	A valid world-context object.
	 * @remark						Make sure this function is called at due time.
	 */
	virtual void Initialize(const UObject& WorldContextObject)
	{		
		if(bIsInitialized)
		{
			UE_LOG(LogFriendVentures, Warning, TEXT("ViewModel has been already initialized."));
			
#if WITH_EDITOR
			UE_LOG(LogFriendVentures, Warning, TEXT("Reinitializing ViewModel in editor ..."));
#else
			return; // BeginDestroy (or even any destructor) is called when PIE ends. It depends on the GC. So we allow continuation here.
#endif
		}

		if(Initialize_Implementation(WorldContextObject.GetWorld()) == false)
		{
			UE_LOG(LogFriendVentures, Warning, TEXT("ViewModel failed to initialize."));
			return;
		}
		
		bIsInitialized = true;
	}

	/**
	 * @brief	Invalidates the view-model.
	 * @remark	Make sure this function is called at due time, where applicable.
	 */
	void Invalidate()
	{
		// 1. Call the implementation function.
		// 2. Mark the view-model as not initialized.
		
		UE_LOG(LogFriendVentures, Warning, TEXT("Invalidating ViewModel ..."));
		Invalidate_Implementation();
		bIsInitialized = false;
	}
	
	/**
	 * @brief	Indicates whether the view-model has been initialized or not.
	 * @return	Returns TRUE if initialized; otherwise, FALSE.
	 */
	bool IsInitialized() const
	{
		return bIsInitialized;
	}

private:
	/**
	 * @brief	Indicates whether the service has been initialized or not.
	 */
	bool bIsInitialized = false;
};
