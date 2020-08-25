#include "WaypointActor.h"
#include "Components/BoxComponent.h"

/**
 * Sets the default values for the WaypointActor.
 */
AWaypointActor::AWaypointActor()
{
	// Create the box collider, set its default extents to something we can see,
	// and then attach it to the RootComponent.
	WaypointBoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("WaypointBoxCollider"));
	WaypointBoxCollider->SetBoxExtent(FVector(90.f, 90.f, 90.f));
	WaypointBoxCollider->SetupAttachment(RootComponent);
}
