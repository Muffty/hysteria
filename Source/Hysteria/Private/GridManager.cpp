#include "GridManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "CoreAI/WorldStateFactory.h"

AGridManager::AGridManager() : CurrentTool(EInputAction::None)
{
	Planner = new FMultiAgentMCTS<16, 16, 3>(HysteriaSim::CreateDemoMap());
	PrimaryActorTick.bCanEverTick = false;
	PlacedActors = TArray<AActor*>();
}

AGridManager::~AGridManager()
{
	delete Planner;
}

WorldState<16, 16, 3> AGridManager::GetPlannerWorld() const
{
	return Planner->GetCurrentState();
}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();

	// Enable input for this actor
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		EnableInput(PC);
		InputComponent->BindAction("SelectTile", IE_Pressed, this, &AGridManager::OnSelectTile);

		// Show mouse cursor
		PC->bShowMouseCursor = true;
		PC->bEnableClickEvents = true;
		PC->bEnableMouseOverEvents = true;
	}

	UE_LOG(LogTemp, Log, TEXT("GridManager initialized!"));

	ShowWorldState();
}

void AGridManager::NextTurn()
{
	if (!Planner)
		Planner = new FMultiAgentMCTS<16, 16, 3>(HysteriaSim::CreateDemoMap());
	Planner->Step();
	ShowWorldState();
}

void AGridManager::SetTool(const EInputAction NewTool)
{
	this->CurrentTool = NewTool;
	// Handle tool selection logic here
	switch (CurrentTool)
	{
	case EInputAction::Erase:
		UE_LOG(LogTemp, Log, TEXT("Tool set to Erase"));
		break;
	case EInputAction::PlaceBoulder:
		UE_LOG(LogTemp, Log, TEXT("Tool set to Place Boulder"));
		break;
	case EInputAction::PlaceFire:
		UE_LOG(LogTemp, Log, TEXT("Tool set to Place Fire"));
		break;
	case EInputAction::PlaceWall:
		UE_LOG(LogTemp, Log, TEXT("Tool set to Place Wall"));
		break;
	case EInputAction::PlaceCoin:
		UE_LOG(LogTemp, Log, TEXT("Tool set to Place Coin"));
		break;
	case EInputAction::PlaceWaterHose:
		UE_LOG(LogTemp, Log, TEXT("Tool set to Place Water Hose"));
		break;
	case EInputAction::PlaceApple:
		UE_LOG(LogTemp, Log, TEXT("Tool set to Place Apple"));
		break;
	case EInputAction::PlacePickaxe:
		UE_LOG(LogTemp, Log, TEXT("Tool set to Place Pickaxe"));
		break;
	case EInputAction::None:
		UE_LOG(LogTemp, Log, TEXT("No tool selected"));
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Unknown tool selected!"));
		break;
	}
}

void AGridManager::ShowWorldState()
{
	// Clear previously placed actors
	for (AActor* Actor : PlacedActors)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}

	// Clear the array
	PlacedActors.Empty();

	// Spawn agents

	for (int i = 0; i < GetPlannerWorld().GetAgentCount(); ++i)
	{
		FVector WorldPosition = ToWorldCoords(GetPlannerWorld().agents[i].x, GetPlannerWorld().agents[i].y);
		if (AgentBlueprintClass)
		{
			auto* spawned = GetWorld()->SpawnActor<AActor>(AgentBlueprintClass, WorldPosition, FRotator::ZeroRotator);
			PlacedActors.Add(spawned);
		}
	}

	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++)
		{
			FVector WorldPosition = ToWorldCoords(x, y);
			TSubclassOf<AActor> ItemBlueprintClass = nullptr;

			switch (GetPlannerWorld().grid[y][x])
			{
			case CellType::Wall:
				ItemBlueprintClass = WallBlueprintClass;
				break;
			case CellType::Fire:
				ItemBlueprintClass = FireBlueprintClass;
				break;
			case CellType::PlayerObstacle:
				ItemBlueprintClass = BoulderBlueprintClass;
				break;
			default:
				break;
			}

			if (ItemBlueprintClass == nullptr && GetPlannerWorld().items[y][x] != ItemType::None)
			{
				switch (GetPlannerWorld().items[y][x])
				{
				case ItemType::Coin:
					ItemBlueprintClass = CoinBlueprintClass;
					break;
				case ItemType::Hose:
					ItemBlueprintClass = WaterHoseBlueprintClass;
					break;
				case ItemType::Food:
					ItemBlueprintClass = AppleBlueprintClass;
					break;
				case ItemType::Pickaxe:
					ItemBlueprintClass = PickaxeBlueprintClass;
					break;
				default:
					ItemBlueprintClass = nullptr; // No item to spawn
				}
			}
			if (ItemBlueprintClass)
			{
				auto* spawned = GetWorld()->SpawnActor<
					AActor>(ItemBlueprintClass, WorldPosition, FRotator::ZeroRotator);
				PlacedActors.Add(spawned);
			}
		}
	}
}

FVector AGridManager::ToWorldCoords(int32 const X, int32 const Y) const
{
	return FVector(750 - Y * 100, -650 + X * 100, 0);
}

void AGridManager::OnSelectTile()
{
	int32 X, Y;
	if (GetMouseGridCoords(X, Y))
	{
		HandleToolAt(X, Y);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No tile under mouse!"));
	}
}

void AGridManager::HandleToolAt(int32 X, int32 Y)
{
	// Check if the coordinates are valid
	if (X < 0 || X >= GridWidth || Y < 0 || Y >= GridHeight)
	{
		UE_LOG(LogTemp, Warning, TEXT("Clicked out of bounds: (%d, %d)"), X, Y);
		return;
	}

	// Store selected coordinates
	SelectedX = X;
	SelectedY = Y;

	switch (CurrentTool)
	{
	case EInputAction::Erase:
		Planner->GetCurrentState().SetItem(X, Y, ItemType::None); // Erase item
		Planner->GetCurrentState().SetTile(X, Y, CellType::Empty); // Erase cell
		break;
	case EInputAction::PlaceBoulder:
		Planner->GetCurrentState().SetTile(X, Y, CellType::PlayerObstacle); // Place boulder
		break;
	case EInputAction::PlaceFire:
		Planner->GetCurrentState().SetTile(X, Y, CellType::Fire); // Place fire
		break;
	case EInputAction::PlaceWall:
		Planner->GetCurrentState().SetTile(X, Y, CellType::Wall); // Place wall
		break;
	case EInputAction::PlaceCoin:
		Planner->GetCurrentState().SetItem(X, Y, ItemType::Coin); // Place coin
		break;
	case EInputAction::PlaceWaterHose:
		Planner->GetCurrentState().SetItem(X, Y, ItemType::Hose); // Place water hose
		break;
	case EInputAction::PlaceApple:
		Planner->GetCurrentState().SetItem(X, Y, ItemType::Food); // Place apple
		break;
	case EInputAction::PlacePickaxe:
		Planner->GetCurrentState().SetItem(X, Y, ItemType::Pickaxe); // Place pickaxe
		break;
	default:
		break; // No action for None tool
	}

	ShowWorldState(); // Refresh the grid display
}

bool AGridManager::GetMouseGridCoords(int32& OutX, int32& OutY) const
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return false;

	float MouseX, MouseY;
	if (!PC->GetMousePosition(MouseX, MouseY)) return false;

	FVector WorldOrigin, WorldDirection;
	if (!PC->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldOrigin, WorldDirection)) return false;

	// Intersect with Z = this actor's Z (e.g. grid at Z=0, or at actor location)
	float ZPlane = GetActorLocation().Z;
	if (FMath::IsNearlyZero(WorldDirection.Z)) return false; // Avoid divide by zero

	float T = (ZPlane - WorldOrigin.Z) / WorldDirection.Z;
	if (T < 0) return false; // Clicked above the plane

	FVector Hit = WorldOrigin + T * WorldDirection;

	// Convert world coordinates to grid indices
	int32 X = FMath::FloorToInt((Hit.X - GetActorLocation().X) / TileSize);
	int32 Y = FMath::FloorToInt((Hit.Y - GetActorLocation().Y) / TileSize);


	//move grid
	X = -(X - 7);
	Y = Y + 7;

	// Check bounds
	if (X < 0 || X >= GridWidth || Y < 0 || Y >= GridHeight)
		return false;


	OutX = Y;
	OutY = X;
	UE_LOG(LogTemp, Warning, TEXT("Clicked at: (%d, %d)"), OutX, OutY);
	return true;
}
