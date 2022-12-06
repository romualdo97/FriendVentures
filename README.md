# Friend Ventures
Instance of the Model-View-ViewModel pattern using UMG + C++

## Showcase
Open [Showcase Video](https://youtu.be/LwNxAgiPbFM)

![Showcase preview](https://github.com/romualdo97/FriendVentures/blob/master/Showcase/FriendVenturesShowcase.gif?raw=true)

## Model

The model is inspired on some interfaces exposed by the [Unreal Online Subsystems](https://docs.unrealengine.com/4.27/en-US/ProgrammingAndScripting/Online/), here we developed a simple incomplete clone of the [Friends Interface](https://docs.unrealengine.com/4.27/en-US/ProgrammingAndScripting/Online/FriendsInterface/) and [Presence Interface](https://docs.unrealengine.com/4.27/en-US/ProgrammingAndScripting/Online/PresenceInterface/) just to start studying EOS internals and to provide a complete Model for the exercise.

![Showcase preview](https://github.com/romualdo97/FriendVentures/blob/master/Showcase/Model.png?raw=true)

Registering services implementatations is done using a config object `UOnlineServicesSubsystemConfig` which acts as an abstract factory for service implementations and is used by the `OnlineServicesSubsystem` subsystem that takes ownership of the received objects and exposes a `TSharedPtr` for usage on the client code.

![Showcase preview](https://github.com/romualdo97/FriendVentures/blob/master/Showcase/ServicesConfig.png?raw=true)

All the registered services implement the `IBaseService` which adds the following access point for config data.

```
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
```

And can be used as below on specific service implementations.

```
const UServiceMockedConfig* Config = GetConfig<UServiceMockedConfig>();
```
