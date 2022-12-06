#pragma once
#include "Model/Services/OnlineServicesSubsystem.h"

/**
 * The base service interface
 */
class FRIENDVENTURES_API IBaseService
{
	/**
	 * Make friend so the initialization of this services is handled by the
	 * subsystem container.
	 */
	friend UOnlineServicesSubsystem;
	
public:
	virtual ~IBaseService() = default;
	
protected:
	/**
	 * Put initialization logic here, is protected because it will
	 * be called by the OnlineServicesSubsystem
	 *
	 * @param InSubsystemOwner The OnlineSubsystem that owns this service
	 */
	virtual void Initialize(UOnlineServicesSubsystem* InSubsystemOwner)
	{
		// Store a weak pointer to UObject that "owns" this service
		SubsystemOwner = InSubsystemOwner;
	}

	/**
	 * Gets the config object casted as a derived config type
	 */
	template<class ConfigT>
	ConfigT* GetConfig()
	{
		static_assert(TIsDerivedFrom<ConfigT, UOnlineServicesSubsystemConfig>::IsDerived,
			"ConfigT is not derived from the UOnlineServicesSubsystemConfig class");

		if (!SubsystemOwner.IsValid())
		{
			return nullptr;
		}

		return Cast<ConfigT>(SubsystemOwner->GetConfig());
	}

	// Shared pointers are not compatible with Unreal objects (UObject classes)!
	// There is a TWeakObjPtr you can use to store a weak uobject reference
	// https://forums.unrealengine.com/t/is-it-safe-to-use-sharedptr-to-act-as-uobject-hard-reference/119248/2
	// https://docs.unrealengine.com/4.27/en-US/ProgrammingAndScripting/ProgrammingWithCPP/UnrealArchitecture/SmartPointerLibrary/
	TWeakObjectPtr<UOnlineServicesSubsystem> SubsystemOwner;
};
