#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AllyCharacter.generated.h"

/**
 * The states that the AllyCharacter can be in.
 */
UENUM(BlueprintType)
enum class AllyStates : uint8 {
	FOLLOW	UMETA(DisplayName = "FOLLOW"),
	LEAD	UMETA(DisplayName = "LEAD"),
};

/**
 * The AllyCharacter follows and leads the PlayerCharacter.
 */
UCLASS(config = Game)
class FOLLOWLEADAI_API AAllyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties.
	AAllyCharacter();

	// The SkeletalMeshComponent of the AllyCharacter.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* AllySkeletalMesh;

	// The BoxComponent that to use as a trigger for detecting Waypoints.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* AllyBoxCollider;

	// The PlayerCharacter to follow and lead.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ally)
	class APlayerCharacter* PlayerCharacter;

	// The current state of the AllyCharacter.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AllyStates State = AllyStates::FOLLOW;

	// Indicates whether the AllyCharacter is sprinting or not.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsSprinting = false;

	// The minimum distance the AllyCharacter should be from the PlayerCharacter.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ally)
	float MinDistanceFromPlayer = 100.f;

	// The maximum distance the AllyCharacter should be from the PlayerCharacter.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ally)
	float MaxDistanceFromPlayer = 500.f;

	// The maximum distance the AllyCharacter can be from the PlayerCharacter before
	// they start sprinting to catch up to the PlayerCharacter.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ally)
	float MaxDistanceFromPlayerBeforeSprint = MaxDistanceFromPlayer + 100.f;

	// The maximum distance the PlayerCharacter can be from the AllyCharacter when
	// following before the AllyCharacter waits for them to catch up.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ally)
	float MaxDistanceFromPlayerWhileLeading = 500.f;

	// The WaypointActor that the AllyCharacter is currently moving towards.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AWaypointActor* CurrentWaypoint;

	// The WaypointActor that the AllyCharacter is ending at.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AWaypointActor* EndWaypoint;

	// Indicates whether the AllyCharacter has arrived at the WaypointActor marked
	// as the `CurrentWaypoint` or not.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsAtCurrentWaypoint = false;

	// A map of all of the Waypoints in the current level.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<int, AWaypointActor*> Waypoints;

	// Indicates whether the AllyCharacter should wait for the PlayerCharacter when leading.
	// This is set by the AllyAIController.
	bool bShouldWaitForPlayerWhenLeading = false;

protected:
	// The speed at which the AllyCharacter should walk at.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float WalkSpeed = 200.f;

	// The speed at which the AllyCharacter should sprint at.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float SprintSpeed = 500.f;

public:
	/**
	 * Called when the AllyCharacter is created.
	 */
	void BeginPlay() override;

	/**
	 * Called when a component enters the AllyCharacter's box collider.
	 */
	UFUNCTION()
	void OnComponentEnterBoxCollider(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**
	 * Called to make the AllyCharacter sprint.
	 */
	UFUNCTION()
	void SprintStart();

	/**
	 * Called to make the AllyCharacter stop sprinting
	 */
	UFUNCTION()
	void SprintStop();
};
