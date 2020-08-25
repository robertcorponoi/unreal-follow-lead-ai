#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

/**
 * Sets the default values for the PlayerCharacter.
 */
APlayerCharacter::APlayerCharacter()
{
	// Load the resources needed for the PlayerCharacter.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>PlayerSkeletalMeshAsset(TEXT("SkeletalMesh'/Game/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin'"));
	static ConstructorHelpers::FObjectFinder<UAnimBlueprint>PlayerAnimBlueprint(TEXT("AnimBlueprint'/Game/Blueprints/PlayerAnimBlueprint.PlayerAnimBlueprint'"));

	// Set the SkeletalMeshComponent to the Character's mesh and adjust its properties.
	PlayerSkeletalMesh = GetMesh();
	PlayerSkeletalMesh->SetSkeletalMesh(PlayerSkeletalMeshAsset.Object);
	PlayerSkeletalMesh->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, -90.f, 0.f));
	PlayerSkeletalMesh->SetAnimInstanceClass(PlayerAnimBlueprint.Object->GeneratedClass);

	// Create the SpringArmComponent and attach it to the RootComponent.
	PlayerCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("PlayerCameraSpringArm"));
	PlayerCameraSpringArm->TargetArmLength = 500.f;
	PlayerCameraSpringArm->bUsePawnControlRotation = true;
	PlayerCameraSpringArm->SetupAttachment(RootComponent);

	// Create the CameraComponent and attach it to the SpringArmComponent.
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->bUsePawnControlRotation = false;
	PlayerCamera->SetupAttachment(PlayerCameraSpringArm, USpringArmComponent::SocketName);

	// Sets the capsule collider's size.
	GetCapsuleComponent()->InitCapsuleSize(40.f, 100.f);

	// Set the PlayerCharacter to not rotate when the controller rotates.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Set the default values for the PlayerCharacter's movement.
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	// Set the PlayerCharacter to be the default player of the game.
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

/**
 * Called to bind functionality to input.
 *
 * @param PlayerInputComponent An Actor component for input bindings.
 */
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// Set up the methods to respond to the PlayerCharacter moving forward, backward,
	// left, and right.
	PlayerInputComponent->BindAxis("MoveForwardBackward", this, &APlayerCharacter::MoveForwardBackward);
	PlayerInputComponent->BindAxis("MoveLeftRight", this, &APlayerCharacter::MoveLeftRight);

	// Set the "TurnLeftRight" and "LookUpDown" axis inputs to control the yaw and pitch
	// of the camera.
	PlayerInputComponent->BindAxis("TurnLeftRight", this, &APlayerCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUpDown", this, &APlayerCharacter::AddControllerPitchInput);

	// Set up the methods to respond to the sprint action input being pressed and released.
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::SprintStop);

	// Set up the method to respond to the AllyLead action input being pressed.
	PlayerInputComponent->BindAction("AllyLead", IE_Pressed, this, &APlayerCharacter::LeadAction);
}

/**
 * Called when the PlayerCamera moves forward and backward.
 *
 * @param Value The axis value from the input.
 */
void APlayerCharacter::MoveForwardBackward(float Value)
{
	// Return early if the Controller is a nullptr or the axis input value is zero.
	if (GetController() == nullptr || Value == 0.f) return;

	// Find the forward rotation.
	const FRotator Rotation = GetController()->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// Find the forward vector.
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	// Apply movement to the forward direction of the PlayerCharacter with a value
	// of `Value`.
	AddMovementInput(Direction, Value);
}

/**
 * Called when the PlayerCamera moves left and right.
 *
 * @param Value The axis value from the input.
 */
void APlayerCharacter::MoveLeftRight(float Value)
{
	// Return early if the Controller is a nullptr or the `Value` is zero.
	if (GetController() == nullptr || Value == 0.f) return;

	// Find the right rotation.
	const FRotator Rotation = GetController()->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// Find the right direction.
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// Add movement to the right axis with a value of `Value`.
	AddMovementInput(Direction, Value);
 }

/**
 * Called when the sprint input action button is pressed down and it sets the
 * `bIsSprinting` boolean to `true` so the animator knows to play the sprint animation.
 */
void APlayerCharacter::SprintStart()
{
	bIsSprinting = true;
	if (GetCharacterMovement()) GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

/**
 * Called when the sprint input action button is pressed down and it sets the
 * `bIsSprinting` boolean to `false` so the animator knows to stop the sprint animation.
 */
void APlayerCharacter::SprintStop()
{
	bIsSprinting = false;
	if (GetCharacterMovement()) GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

/**
 * Called when the "Lead" action input is pressed.
 */
void APlayerCharacter::LeadAction()
{
	// Make the AllyCharacter lead the PlayerCharacter from the first waypoint to the
	// second waypoint and waiting for the PlayerCharacter to be in range.
	OnAllyLeadRequest.Broadcast(0, 1, true);
}