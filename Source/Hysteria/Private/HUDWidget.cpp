#include "HUDWidget.h"

#include "GridManager.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BtnPlaceBoulder)
		BtnPlaceBoulder->OnClicked.AddDynamic(this, &UHUDWidget::OnPlaceBoulderClicked);
	if (BtnPlaceFire)
		BtnPlaceFire->OnClicked.AddDynamic(this, &UHUDWidget::OnPlaceFireClicked);
	if (BtnPlaceWall)
		BtnPlaceWall->OnClicked.AddDynamic(this, &UHUDWidget::OnPlaceWallClicked);
	if (BtnErase)
		BtnErase->OnClicked.AddDynamic(this, &UHUDWidget::OnEraseClicked);
	if (BtnPlaceCoin)
		BtnPlaceCoin->OnClicked.AddDynamic(this, &UHUDWidget::OnPlaceCoinClicked);
	if (BtnPlaceWaterHose)
		BtnPlaceWaterHose->OnClicked.AddDynamic(this, &UHUDWidget::OnPlaceWaterHoseClicked);
	if (BtnPlaceApple)
		BtnPlaceApple->OnClicked.AddDynamic(this, &UHUDWidget::OnPlaceAppleClicked);
	if (BtnPlacePickaxe)
		BtnPlacePickaxe->OnClicked.AddDynamic(this, &UHUDWidget::OnPlacePickaxeClicked);


	if (BtnNextTurn)
		BtnNextTurn->OnClicked.AddDynamic(this, &UHUDWidget::OnNextTurnClicked);

	GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
}

void UHUDWidget::OnPlaceBoulderClicked()
{
	GridManager->SetTool(EInputAction::PlaceBoulder);
	OnToolButtonClicked(BtnPlaceBoulder);
}
void UHUDWidget::OnPlaceFireClicked()
{
	GridManager->SetTool(EInputAction::PlaceFire);
	OnToolButtonClicked(BtnPlaceFire);
}

void UHUDWidget::OnPlaceWallClicked()
{
	GridManager->SetTool(EInputAction::PlaceWall);
	OnToolButtonClicked(BtnPlaceWall);
}

void UHUDWidget::OnEraseClicked()
{
	GridManager->SetTool(EInputAction::Erase);
	OnToolButtonClicked(BtnErase);
}

void UHUDWidget::OnPlaceCoinClicked()
{
	GridManager->SetTool(EInputAction::PlaceCoin);
	OnToolButtonClicked(BtnPlaceCoin);
}

void UHUDWidget::OnPlaceWaterHoseClicked()
{
	GridManager->SetTool(EInputAction::PlaceWaterHose);
	OnToolButtonClicked(BtnPlaceWaterHose);
}

void UHUDWidget::OnPlaceAppleClicked()
{
	GridManager->SetTool(EInputAction::PlaceApple);
	OnToolButtonClicked(BtnPlaceApple);
}

void UHUDWidget::OnPlacePickaxeClicked()
{
	GridManager->SetTool(EInputAction::PlacePickaxe);
	OnToolButtonClicked(BtnPlacePickaxe);
}

void UHUDWidget::OnToolButtonClicked(UButton* ClickedButton)
{
	if (SelectedToolButton == ClickedButton)
	{
		// Deselect if clicking the already selected one
		UpdateButtonSelection(nullptr);
	}
	else
	{
		UpdateButtonSelection(ClickedButton);
	}

	// TODO: Fire tool selection event
}

void UHUDWidget::OnNextTurnClicked()
{
	GridManager->NextTurn();
}

void UHUDWidget::UpdateButtonSelection(UButton* NewlySelected)
{
	// Un-highlight old button
	if (SelectedToolButton)
		SelectedToolButton->SetBackgroundColor(FLinearColor::White);

	// Highlight new one if any
	if (NewlySelected)
		NewlySelected->SetBackgroundColor(FLinearColor::Yellow);

	if (!NewlySelected)
		GridManager->SetTool(EInputAction::None);
	
	SelectedToolButton = NewlySelected;
}
