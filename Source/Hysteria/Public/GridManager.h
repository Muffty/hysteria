#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

UCLASS()
class HYSTERIA_API AGridManager : public AActor
{
	GENERATED_BODY()

public:
	AGridManager();

protected:
	virtual void BeginPlay() override;

	// Mouse click handler
	UFUNCTION()
	void OnSelectTile();

	// Helper for projecting the mouse cursor to world/grid
	bool GetMouseGridCoords(int32& OutX, int32& OutY) const;

public:
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
};
