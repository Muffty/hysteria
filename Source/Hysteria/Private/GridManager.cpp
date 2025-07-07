#include "GridManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

AGridManager::AGridManager()
{
    PrimaryActorTick.bCanEverTick = false;
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
}

void AGridManager::OnSelectTile()
{
    int32 X, Y;
    if (GetMouseGridCoords(X, Y))
    {
        SelectedX = X;
        SelectedY = Y;

        UE_LOG(LogTemp, Log, TEXT("Tile selected: X=%d, Y=%d"), SelectedX, SelectedY);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No tile under mouse!"));
    }
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

    // Check bounds
    /*if (X < 0 || X >= GridWidth || Y < 0 || Y >= GridHeight)
        return false;
*/
    
    OutX = X;
    OutY = Y;
    return true;
}
