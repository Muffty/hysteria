// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner.h"

// Sets default values
ASpawner::ASpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpawner::BeginPlay()
{
	Super::BeginPlay();
	
	if (ActorToSpawn)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			for (int32 i = 0; i < NumberToSpawn; i++)
			{
				FVector SpawnLocation = GetActorLocation() + FMath::VRand() * SpawnRadius;
				SpawnLocation.Z = GetActorLocation().Z;
				FRotator SpawnRotation = FRotator::ZeroRotator;

				World->SpawnActor<AActor>(ActorToSpawn, SpawnLocation, SpawnRotation);
			}
		}
	}
}

// Called every frame
void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

