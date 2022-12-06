#pragma once
#include "Engine/DataTable.h"
#include "FriendDataTableRow.generated.h"

/**
* Encapsulates the data of a friend.
* @remark This struct will be used to create the datatable that will serve as a mocked model for the exercise.
*/
USTRUCT(BlueprintType)
struct FRIENDVENTURES_API FFriendDataTableRow : public FTableRowBase
{
	GENERATED_BODY()
	
	/**
	 * The nickname of the friend (it may also be an non-localized FText, instead).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString Nickname{};
	
	/**
	 * The real name of the user
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString RealName{};
	
	/**
	 * The level of the user
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 Level{};
};
