#include "AllyAIController.h"
#include "AllyCharacter.h"
#include "../WaypointActor.h"
#include "../Player/PlayerCharacter.h"
#include "Tasks/AITask_MoveTo.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

/**
 * Sets up the default values for the AllyAIController.
 */
AAllyAIController::AAllyAIController()
{
	// Starts the AI logic for this AIController as soon as the AllyCharacter
	// is taken over so that we can issue commands immediately.
	bStartAILogicOnPossess = true;
}

/**
 * Called when the AllyAIController starts.
 */
void AAllyAIController::BeginPlay()
{
	Super::BeginPlay();

	// Set up the response to the PlayerCharacter's `OnAllyLeadRequest` delegate.
	AllyCharacter->PlayerCharacter->OnAllyLeadRequest.AddDynamic(this, &AAllyAIController::MakeAllyLead);

	// Move the AllyCharacter to the PlayerCharacter from the start.
	MoveToPlayerCharacter();
}

/**
 * Called when the AllyAIController takes over the AllyCharacter.
 *
 * @param AllyPawn The AllyCharacter pawn.
 */
void AAllyAIController::OnPossess(APawn* AllyPawn)
{
	Super::OnPossess(AllyPawn);

	// Attempt to cast the Pawn that was taken over to an AllyCharacter and if
	// successful then we assign it to our `AllyCharacter` variable.
	AllyCharacter = Cast<AAllyCharacter>(AllyPawn);
}

/**
 * Called when a move request has been completed.
 */
void AAllyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (AllyCharacter->State == AllyStates::FOLLOW)
	{
		// Check to see if the AllyCharacter is moving with a simple velocity check.
		bool bIsAllyCharacterMoving = AllyCharacter->GetCharacterMovement()->Velocity.Size() > 0.f;

		if (bIsAllyCharacterMoving)
		{
			// If the AllyCharacter is moving then it means that the PlayerCharacter is still moving
			// so we call `MoveToPlayerCharacter` to keep moving towards the PlayerCharacter.
			MoveToPlayerCharacter();
		}
		else
		{
			UWorld* World = GetWorld();
			if (World == nullptr) return;

			// Otherwise if the AllyCharacter is no longer moving then `OnMoveCompleted` will not run
			// again so we need to set up a repeating timer that checks to see if the PlayerCharacter
			// has started moving again and if so we cancel this timer and call `MoveToPlayerCharacter`
			// which just restarts this whole process.
			World->GetTimerManager().SetTimer(AllyFollowTimer, this, &AAllyAIController::CheckIfPlayerIsMoving, 0.05f, true);
			World->GetTimerManager().ClearTimer(AllySprintTimer);
		}
	}
	else if (AllyCharacter->State == AllyStates::LEAD)
	{
		if (AllyCharacter->CurrentWaypoint == AllyCharacter->EndWaypoint && AllyCharacter->bIsAtCurrentWaypoint)
		{
			// If the AllyCharacter is at the last waypoint then we can clear the lead timer and set
			// them back to the FOLLOW state.
			GetWorld()->GetTimerManager().ClearTimer(AllyLeadTimer);
			AllyCharacter->State = AllyStates::FOLLOW;

			// Set the AllyCharacter to move to the PlayerCharacter again to keep the follow loop going.
			MoveToPlayerCharacter();
		}
		else if (AllyCharacter->bIsAtCurrentWaypoint)
		{
			// Otherwise we set the AllyCharacter to move to the next waypoint.
			AllyCharacter->CurrentWaypoint = AllyCharacter->Waypoints[AllyCharacter->CurrentWaypoint->WaypointNumber + 1];
			AllyCharacter->bIsAtCurrentWaypoint = false;
		}
	}
}

/**
 * Called to move the AllyCharacter to the PlayerCharacter.
 */
void AAllyAIController::MoveToPlayerCharacter()
{
	// Return early if the PlayerCharacter hasn't been assigned to the AllyCharacter.
	if (AllyCharacter->PlayerCharacter == nullptr) return;

	// Get a random value between `MinDistanceFromPlayer` and `MaxDistanceFromPlayer` to use
	// as the second parameter.
	float AcceptanceRadius = UKismetMathLibrary::RandomFloatInRange(AllyCharacter->MinDistanceFromPlayer, AllyCharacter->MaxDistanceFromPlayer);

	// Move to the PlayerCharacter within the AcceptanceRadius.
	MoveToActor(AllyCharacter->PlayerCharacter, AcceptanceRadius);
}

/**
 * Called to move the AllyCharacter to a WaypointActor.
 */
void AAllyAIController::MoveToWaypoint()
{
	// Make sure that this is only called when the AllyCharacter is in the
	// LEAD state.
	if (AllyCharacter->State != AllyStates::LEAD) return;

	// If `AllyCharacter->bShouldWaitForPlayerWhenLeading` is true then we need to check
	// to see if the PlayerCharacter is too far from the AllyCharacter and if so then we
	// stop movement until the PlayerCharacter gets closer. Otherwise we just continue to
	// the WaypointActor.
	float AllyDistanceFromPlayer = AllyCharacter->GetDistanceTo(AllyCharacter->PlayerCharacter);
	if (AllyCharacter->bShouldWaitForPlayerWhenLeading && AllyDistanceFromPlayer >= AllyCharacter->MaxDistanceFromPlayerWhileLeading)
	{
		StopMovement();
	}
	else
	{
		MoveToActor(AllyCharacter->CurrentWaypoint);
	}
}

/**
 * Called by the `AllyFollowTimer` to check to see if the PlayerCharacter is
 * is moving or not.
 */
void AAllyAIController::CheckIfPlayerIsMoving()
{
	// Check to see if the PlayerCharacter is moving by a simple velocity check.
	bool bIsPlayerCharacterMoving = AllyCharacter->PlayerCharacter->GetCharacterMovement()->Velocity.Size() > 0.f;

	if (bIsPlayerCharacterMoving)
	{
		UWorld* World = GetWorld();
		if (World == nullptr) return;

		// Clear the timer as the movement is going to get handled by the `OnMoveCompleted`
		// method until the AllyCharacter stops moving again.
		World->GetTimerManager().ClearTimer(AllyFollowTimer);

		// Set the timer that manages the AllyCharacter's movement properties such as walking
		// and sprinting.
		World->GetTimerManager().SetTimer(AllySprintTimer, this, &AAllyAIController::ManageAllySprint, 0.5, true);

		// Call `MoveToPlayerCharacter` to start this process all over again.
		MoveToPlayerCharacter();
	}
}

/**
 * Called by the `AllySprintTimer` to make the AllyCharacter start or stop sprinting.
 */
void AAllyAIController::ManageAllySprint()
{
	float DistanceFromPlayerCharacter = AllyCharacter->GetDistanceTo(AllyCharacter->PlayerCharacter);

	UCharacterMovementComponent* Movement = AllyCharacter->GetCharacterMovement();

	if (AllyCharacter->State == AllyStates::FOLLOW)
	{
		if (DistanceFromPlayerCharacter >= AllyCharacter->MaxDistanceFromPlayerBeforeSprint && !AllyCharacter->bIsSprinting)
		{
			// If the AllyCharacter is out of the `MaxDistanceFromPlayerBeforeSprint` value then
			// we set the AllyCharacter to sprint.
			AllyCharacter->SprintStart();
		}
		else if (DistanceFromPlayerCharacter < AllyCharacter->MaxDistanceFromPlayerBeforeSprint && AllyCharacter->bIsSprinting)
		{
			// If the AllyCharacter is not out of the `MaxDistanceFromPlayerBeforeSprint` value
			// and it is sprinting then we set it back to the walking speed.
			AllyCharacter->SprintStop();
		}
	}
}

/**
 * Responds to the `OnAllyLeadRequest` to put the AllyCharacter in the LEAD
 * state and make them move to a waypoint.
 */
void AAllyAIController::MakeAllyLead(int WaypointA, int WaypointB, bool bShouldWaitForPlayer)
{
	// Put the AllyCharacter in the LEAD state.
	AllyCharacter->State = AllyStates::LEAD;

	// Clear the AllyFollowTimer if the AllyCharacter was in the FOLLOW state before.
	GetWorld()->GetTimerManager().ClearTimer(AllyFollowTimer);

	// Set the AllyCharcter's `CurrentWaypoint` to `WaypointA` and `EndWaypoint` to `WaypointB`.
	AllyCharacter->CurrentWaypoint = AllyCharacter->Waypoints[WaypointA];
	AllyCharacter->EndWaypoint = AllyCharacter->Waypoints[WaypointB];

	AllyCharacter->bShouldWaitForPlayerWhenLeading = bShouldWaitForPlayer;

	// Move to the next WaypointActor which could be WaypointA, WaypointB, or a WaypointActor
	// in between.
	GetWorld()->GetTimerManager().SetTimer(AllyLeadTimer, this, &AAllyAIController::MoveToWaypoint, 0.5f, true);
}
 