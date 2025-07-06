// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spawner.generated.h"

UCLASS()
class HYSTERIA_API ASpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// what to spawn
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<AActor> ActorToSpawn;

	// how many to spawn
	UPROPERTY(EditAnywhere, Category = "Spawning")
	int32 NumberToSpawn = 1;

	// spawn radius
	UPROPERTY(EditAnywhere, Category = "Spawning")
	float SpawnRadius = 500.f;
};
