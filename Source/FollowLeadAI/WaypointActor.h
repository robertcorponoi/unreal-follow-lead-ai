#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaypointActor.generated.h"

UCLASS()
class FOLLOWLEADAI_API AWaypointActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties.
	AWaypointActor();

	// The `UBoxComponent` of this WaypointActor that lets the AllyCharacter
	// know when it has arrived at this WaypointActor.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBoxComponent* WaypointBoxCollider;

	// The number waypoint that this WaypointActor is. The AllyCharacter will
	// follow waypoints in order.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waypoint)
	int32 WaypointNumber = 0;
};
