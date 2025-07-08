#pragma once

#include "CoreMinimal.h"
#include "CoreAI/FMultiAgentMCTS.h"
#include "CoreAI/WorldState.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

enum class EInputAction
{
	None,
	Erase,
	PlaceBoulder,
	PlaceFire,
	PlaceWall,
	PlaceCoin,
	PlaceWaterHose,
	PlaceApple,
	PlacePickaxe
};

UCLASS()
class HYSTERIA_API AGridManager : public AActor
{
	GENERATED_BODY()

public:
	AGridManager();
	~AGridManager();

protected:
	FVector ToWorldCoords(int32 X, int32 Y) const;
	void ShowWorldState();
	FMultiAgentMCTS<16, 16, 3>* Planner = nullptr;
	EInputAction CurrentTool;
	TArray<AActor*> PlacedActors;
	
	WorldState<16, 16, 3> GetPlannerWorld() const;
	virtual void BeginPlay() override;

	void HandleToolAt(int32 X, int32 Y);
	// Mouse click handler
	UFUNCTION()
	void OnSelectTile();

	// Helper for projecting the mouse cursor to world/grid
	bool GetMouseGridCoords(int32& OutX, int32& OutY) const;

public:
	void SetTool(EInputAction NewTool);
	void NextTurn();

	// Set these as needed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 GridWidth = 16;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 GridHeight = 16;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	float TileSize = 100.0f;

	// Last clicked/selected cell
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	int32 SelectedX = -1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	int32 SelectedY = -1;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prefabs")
	TSubclassOf<AActor> AgentBlueprintClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prefabs")
	TSubclassOf<AActor> CoinBlueprintClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prefabs")
	TSubclassOf<AActor> BoulderBlueprintClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prefabs")
	TSubclassOf<AActor> FireBlueprintClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prefabs")
	TSubclassOf<AActor> WallBlueprintClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prefabs")
	TSubclassOf<AActor> WaterHoseBlueprintClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prefabs")
	TSubclassOf<AActor> AppleBlueprintClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prefabs")
	TSubclassOf<AActor> PickaxeBlueprintClass;
};
