#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

// Creates a delegate that's used to tell the AllyAIController to make
// the AllyCharacter lead the way.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAllyLeadRequest, int32, StartWaypoint, int32, EndWaypoint, bool, bShouldWaitForPlayer);

/**
 * PlayerCharacter is the main Character of the game controlled by the player.
 */
UCLASS(config = Game)
class FOLLOWLEADAI_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties.
	APlayerCharacter();

	// The PlayerCharacter's skeletal mesh.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
	class USkeletalMeshComponent* PlayerSkeletalMesh;

	// The PlayerCharacter's camera.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* PlayerCamera;

	// The boom for the PlayerCharacter's camera.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* PlayerCameraSpringArm;

	// Indicates whether the PlayerCharacter is sprinting or not.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	bool bIsSprinting = false;

	// Broadcast when the PlayerCharacter triggers an object in the level that
	// that makes the AllyCharacter switch to the LEAD state or when the PlayerCharacter
	// is deemed to be "lost".
	UPROPERTY(BlueprintAssignable, Category = "StateEvents")
	FAllyLeadRequest OnAllyLeadRequest;

protected:
	// The speed at which the PlayerCharacter should walk at.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float WalkSpeed = 200.f;

	// The speed at which the PlayerCharacter should sprint at.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float SprintSpeed = 500.f;

protected:
	/**
	 * Called when the PlayerCamera moves forward and backward.
     *
     * @param Value The axis value from the input.
	 */
	void MoveForwardBackward(float Value);

	/**
	 * Called when the PlayerCamera moves left and right.
	 *
	 * @param Value The axis value from the input.
	 */
	void MoveLeftRight(float Value);

	/**
	 * Called when the sprint input action button is pressed down.
	 */
	 void SprintStart();

	 /**
	  * Called when the sprint input action button is released.
	  */
	 void SprintStop();

	 /**
	  * Called when the "Lead" action input is pressed.
	  */
	void LeadAction();

	/**
	 * Called to bind functionality to input.
	 *
	 * @param PlayerInputComponent An Actor component for input bindings.
	 */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
