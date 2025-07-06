// WanderAgent.cpp
#include "WanderAgent.h"
#include "NavigationSystem.h"
#include "AIController.h"

AWanderAgent::AWanderAgent()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AWanderAgent::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(WanderTimerHandle, this, &AWanderAgent::WanderToRandomLocation, 3.0f, true);
}


void AWanderAgent::WanderToRandomLocation()
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys) return;

	FNavLocation Location;
	const FVector Origin = GetActorLocation();

	if (NavSys->GetRandomReachablePointInRadius(Origin, 800.f, Location))
	{
		AAIController* AICon = Cast<AAIController>(GetController());
		if (AICon)
		{
			AICon->MoveToLocation(Location.Location);
		}
		
	}
}
