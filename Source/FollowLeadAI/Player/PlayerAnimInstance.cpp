#include "PlayerAnimInstance.h"
#include "PlayerCharacter.h"
#include "Math/Rotator.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

/**
 * Used by the animation blueprint to update the animation properties above
 * and decide what animations to play.
 */
void UPlayerAnimInstance::UpdateAnimationProperties()
{
	// Try to get the Pawn being animated and return if a nullptr.
	APawn* PlayerPawn = TryGetPawnOwner();
	if (PlayerPawn == nullptr) return;

	// Try to cast the Pawn to our PlayerCharacter since that's the only
	// thing we want to animate.
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PlayerPawn);
	if (PlayerCharacter == nullptr) return;

	// Get the PlayerCharacter's normalized velocity.
	UCharacterMovementComponent* PlayerCharacterMovement = PlayerCharacter->GetCharacterMovement();
	FVector PlayerCharacterMovementVelocity = PlayerCharacterMovement->Velocity;
	PlayerCharacterMovementVelocity.Normalize();

	// Get the PlayerCharacter's CapsuleComponent rotation and un-rotate it.
	UCapsuleComponent* PlayerCapsule = PlayerCharacter->GetCapsuleComponent();
	FRotator PlayerCapsuleRotation = PlayerCapsule->GetComponentRotation();
	FVector PlayerVelocityUnrotated = PlayerCapsuleRotation.UnrotateVector(PlayerCharacterMovementVelocity);

	// Set the animation properties based on the unrotated velocity.
	bIsMoving = PlayerVelocityUnrotated.X > 0.1 || PlayerVelocityUnrotated.Y > 0.1;
	bIsSprinting = PlayerCharacter->bIsSprinting;
}