#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AllyAIController.generated.h"

class AWaypoint;

/**
 * The AllyAIController controls the movement and behavior of the AllyCharacter.
 */
UCLASS()
class FOLLOWLEADAI_API AAllyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AAllyAIController();

protected:
	// A reference to the AllyCharacter.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
	class AAllyCharacter* AllyCharacter;

	// The repeating timer used to manage the AllyCharacter's sprint state.
	FTimerHandle AllySprintTimer;

	// The repeating timer used to make the AllyCharacter follow the PlayerCharacter.
	FTimerHandle AllyFollowTimer;

	// The repeating timer used to make the AllyCharacter lead the PlayerCharacter.
	FTimerHandle AllyLeadTimer;

protected:
	/**
	 * Called when the AllyAIController starts.
	 */
	virtual void BeginPlay() override;

	/**
	 * Called when the AllyAIController takes over the AllyCharacter.
	 *
	 * @param AllyPawn The AllyCharacter pawn.
	 */
	virtual void OnPossess(APawn* AllyPawn) override;

	/**
	 * Called when a move request has been completed.
	 */
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	/**
	 * Called to move the AllyCharacter to the PlayerCharacter.
	 */
	void MoveToPlayerCharacter();

	/**
	 * Called to move the AllyCharacter to its `CurrentWaypoint`.
	 */
	void MoveToWaypoint();

	/**
	 * Called by the `AllyFollowTimer` to check to see if the PlayerCharacter is
     * is moving or not.
	 */
	UFUNCTION()
	void CheckIfPlayerIsMoving();

	/**
	 * Called by the `AllySprintTimer` to make the AllyCharacter start or stop sprinting.
	 */
	UFUNCTION()
	void ManageAllySprint();

	/**
	 * Called when`OnAllyLeadRequest` is broadcast to put the AllyCharacter in the LEAD
     * state and make them move to a waypoint.
     */
	 UFUNCTION()
	 void MakeAllyLead(int32 WaypointA, int32 WaypointB, bool bShouldWaitForPlayer);
};
