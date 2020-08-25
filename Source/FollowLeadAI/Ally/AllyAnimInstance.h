#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AllyAnimInstance.generated.h"

/**
 * Manages the booleans needed by the animation blueprint to decide what animation
 * needs to be run.
 */
UCLASS()
class FOLLOWLEADAI_API UAllyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// Indicates whether the AllyCharacter is moving in any direction or not.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsMoving;

	// Indicates whether the AllyCharacter is sprinting or not.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsSprinting;

	// Used by the animation blueprint to update the animation properties above
	// and decide what animations to play.
	UFUNCTION(BlueprintCallable, Category = "UpdateAnimationProperties")
	void UpdateAnimationProperties();
};
