#include "AllyCharacter.h"
#include "../WaypointActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

/**
 * Sets the default values for the AllyCharacter.
 */
AAllyCharacter::AAllyCharacter()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>AllySkeletalMeshAsset(TEXT("SkeletalMesh'/Game/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin'"));
	static ConstructorHelpers::FObjectFinder<UAnimBlueprint>AllyAnimBlueprint(TEXT("AnimBlueprint'/Game/Blueprints/AllyAnimBlueprint.AllyAnimBlueprint'"));

	// Set the SkeletalMeshComponent to the Character's mesh and adjust its properties.
	AllySkeletalMesh = GetMesh();
	AllySkeletalMesh->SetSkeletalMesh(AllySkeletalMeshAsset.Object);
	AllySkeletalMesh->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, -90.f, 0.f));
	AllySkeletalMesh->SetAnimInstanceClass(AllyAnimBlueprint.Object->GeneratedClass);

	// Set the initial speed to the `WalkSpeed`.
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	// Create the BoxComponent, set its extents, and attach it to the Root.
	AllyBoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	AllyBoxCollider->SetBoxExtent(FVector(90.f, 90.f, 85.f));
	AllyBoxCollider->SetGenerateOverlapEvents(true);
	AllyBoxCollider->SetCollisionProfileName(TEXT("Trigger"));
	AllyBoxCollider->OnComponentBeginOverlap.AddDynamic(this, &AAllyCharacter::OnComponentEnterBoxCollider);
	AllyBoxCollider->SetupAttachment(RootComponent);
}

/**
 * Called when the AllyCharacter is created.
 */
void AAllyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Get all of the WaypointActors in the level.
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWaypointActor::StaticClass(), FoundActors);

	// For each WaypointActor we found we have to try and cast it as a WaypointActor
	// as they are found as Actors not WaypointActors. If we can successfully cast it
	// to a WaypointActor then we add it to the WaypointActors map.
	for (AActor* FoundActor : FoundActors)
	{
		AWaypointActor* Waypoint = Cast<AWaypointActor>(FoundActor);

		// If we can successfully cast the Actor to a WaypointActor then we add it to
		// the map with the WaypointNumber as the key and WaypointActor as the value.
		if (Waypoint != nullptr) Waypoints.Add(Waypoint->WaypointNumber, Waypoint);
	}

	// After all of the Waypoints have been added to the Waypoints map then we sort the
	// map by its keys, which are the WaypointNumbers.
	Waypoints.KeySort([](int A, int B) { return A < B; });
}

/**
 * Called when a component enters the AllyCharacter's box collider.
 */
void AAllyCharacter::OnComponentEnterBoxCollider(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Return early if anything is null so we can avoid a potential crash.
	if ((OtherActor == nullptr) || (OtherActor == this) || (OtherComp == nullptr)) return;

	// Try to cast the `OtherActor` to a WaypointActor.
	AWaypointActor* Waypoint = Cast<AWaypointActor>(OtherActor);

	// If the AllyCharacter is in the LEAD state and we were able to successfully cast the `OtherActor`
	// to a WaypointActor, then we set `bIsAtCurrentWaypoint` to `true`.
	if (State == AllyStates::LEAD && Waypoint != nullptr) 
	{
		bIsAtCurrentWaypoint = true;
	}
}

/**
 * Called to make the AllyCharacter sprint.
 */
void AAllyCharacter::SprintStart()
{
	bIsSprinting = true;
	if (GetCharacterMovement()) GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

/**
 * Called to make the AllyCharacter stop sprinting.
 */
void AAllyCharacter::SprintStop()
{
	bIsSprinting = false;
	if (GetCharacterMovement()) GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}
