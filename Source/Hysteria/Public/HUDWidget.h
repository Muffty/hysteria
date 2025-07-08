#pragma once

#include "CoreMinimal.h"
#include "GridManager.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

UCLASS()
class HYSTERIA_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// Buttons bound from UMG
	UPROPERTY(meta = (BindWidget))
	class UButton* BtnPlaceBoulder;
	UPROPERTY(meta = (BindWidget))
	UButton* BtnPlaceFire;
	UPROPERTY(meta = (BindWidget))
	UButton* BtnPlaceWall;
	UPROPERTY(meta = (BindWidget))
	UButton* BtnErase;
	UPROPERTY(meta = (BindWidget))
	UButton* BtnPlaceCoin;
	UPROPERTY(meta = (BindWidget))
	UButton* BtnPlaceWaterHose;
	UPROPERTY(meta = (BindWidget))
	UButton* BtnPlaceApple;
	UPROPERTY(meta = (BindWidget))
	UButton* BtnPlacePickaxe;
	UPROPERTY(meta = (BindWidget))
	UButton* BtnNextTurn;
	UPROPERTY()
	AGridManager* GridManager = nullptr;
	
	// Called by button events
	UFUNCTION()
	void OnPlaceBoulderClicked();
	UFUNCTION()
	void OnPlaceFireClicked();
	UFUNCTION()
	void OnPlaceWallClicked();
	UFUNCTION()
	void OnEraseClicked();
	UFUNCTION()
	void OnPlaceCoinClicked();
	UFUNCTION()
	void OnPlaceWaterHoseClicked();
	UFUNCTION()
	void OnPlaceAppleClicked();
	UFUNCTION()
	void OnPlacePickaxeClicked();
	void OnToolButtonClicked(UButton* NewlySelected);

	UFUNCTION()
	void OnNextTurnClicked();

	// Utility
	void UpdateButtonSelection(UButton* NewlySelected);

	// For tracking
	UPROPERTY()
	UButton* SelectedToolButton = nullptr;
};
