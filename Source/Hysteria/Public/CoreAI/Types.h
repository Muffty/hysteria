#pragma once
#ifdef HYSTERIA_USE_UNREAL
	#include "Containers/Array.h"
	#include "Containers/Map.h"
	#include "Containers/UnrealString.h"
	#include "Misc/Optional.h"
	#include "Templates/SharedPointer.h"
	#define HYSTERIA_VECTOR      TArray
	#define HYSTERIA_MAP         TMap
	#define HYSTERIA_OPTIONAL    TOptional
	#define HYSTERIA_STRING      FString
	#define HYSTERIA_SHARED_PTR  TSharedPtr
#else
	#include <vector>
	#include <map>
	#include <optional>
	#include <string>
	#include <memory>
	#define HYSTERIA_VECTOR      std::vector
	#define HYSTERIA_MAP         std::map
	#define HYSTERIA_OPTIONAL    std::optional
	#define HYSTERIA_STRING      std::string
	#define HYSTERIA_SHARED_PTR  std::shared_ptr
#endif
enum class CellType : uint8_t
{
	Empty,
	Wall,
	Fire,
	PlayerObstacle
};

enum class ItemType : uint8_t
{
	None,
	Food,
	Hose,
	Pickaxe,
	Coin
};

struct AgentState {
	uint8_t x, y;
	bool hasItem;
	ItemType item;
	int score;
	bool isPanicking;
};
enum class EActionType {
	MoveDown,
	MoveUp,
	MoveLeft,
	MoveRight,
	
	Pickup,
	Drop,
	UseItem,
	Wait
};
struct FAgentAction {
	EActionType Type;
	FAgentAction(EActionType InType) : Type(InType) {}
	FAgentAction() : Type(EActionType::Wait) {} // Optional: default constructor
};