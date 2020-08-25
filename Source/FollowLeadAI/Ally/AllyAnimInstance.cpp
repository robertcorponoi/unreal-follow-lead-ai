#include "AllyAnimInstance.h"
#include "AllyCharacter.h"
#include "Math/Rotator.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

/**
 * Used by the animation blueprint to update the animation properties above
 * and decide what animations to play.
 */
void UAllyAnimInstance::UpdateAnimationProperties()
{
	// Try to get the Pawn being animated and return if a nullptr.
	APawn* AllyPawn = TryGetPawnOwner();
	if (AllyPawn == nullptr) return;

	// Try to cast the Pawn to our AllyCharacter since that's the only
	// thing we want to animate.
	AAllyCharacter* AllyCharacter = Cast<AAllyCharacter>(AllyPawn);
	if (AllyCharacter == nullptr) return;

	// Get the AllyCharacter's normalized velocity.
	UCharacterMovementComponent* AllyCharacterMovement = AllyCharacter->GetCharacterMovement();
	FVector AllyCharacterMovementVelocity = AllyCharacterMovement->Velocity;
	AllyCharacterMovementVelocity.Normalize();

	// Get the AllyCharacter's CapsuleComponent rotation and un-rotate it.
	UCapsuleComponent* AllyCapsule = AllyCharacter->GetCapsuleComponent();
	FRotator AllyCapsuleRotation = AllyCapsule->GetComponentRotation();
	FVector AllyVelocityUnrotated = AllyCapsuleRotation.UnrotateVector(AllyCharacterMovementVelocity);

	// Set the animation properties based on the unrotated velocity.
	bIsMoving = AllyVelocityUnrotated.X > 0.1 || AllyVelocityUnrotated.Y > 0.1;
	bIsSprinting = AllyCharacter->bIsSprinting;
}