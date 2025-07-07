#pragma once

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
	bool isPanicking;
};
enum class EActionType {
	MoveUp,
	MoveDown,
	MoveLeft,
	MoveRight,
	
	Pickup,
	Drop,
	UseItem,
	Wait
};
struct FAgentAction {
	EActionType Type;
};