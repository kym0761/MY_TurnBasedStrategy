// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSelectButtonWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "UnitAction/UnitActionComponent.h"
#include "Kismet/GameplayStatics.h"
//#include "UnitControl/UnitSelectPawn.h"
#include "Manager/GridManager.h"
#include "UnitControl/UnitControlPawn.h"

void UActionSelectButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bIsFocusable = true;
}

void UActionSelectButtonWidget::InitActionSelectButton(UUnitActionComponent* InputAction)
{
	if (IsValid(InputAction))
	{
		UnitAction = InputAction;
	}

	InitButton();

}

void UActionSelectButtonWidget::InitButton()
{
	if (IsValid(Button_Select))
	{
		Button_Select->OnClicked.AddDynamic(this, &UActionSelectButtonWidget::OnButtonClicked);
	}

	if (IsValid(TextBlock_ActionName))
	{
		if (IsValid(UnitAction))
		{
			TextBlock_ActionName->SetText(FText::FromString(UnitAction->GetActionName()));
		}
	}
}

void UActionSelectButtonWidget::OnButtonClicked()
{
	if (!IsValid(UnitAction))
	{
		return;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Button Clicked! --> %s"), *UnitAction->GetActionName());
	// Do Something.

	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	AUnitControlPawn* pawn = Cast<AUnitControlPawn>(playerController->GetPawn());
	pawn->SetSelectedAction(UnitAction);
	pawn->SetControlPawnMode(EPawnMode::Action);

	AGridManager* gridManager = AGridManager::GetGridManager();
	if (IsValid(gridManager))
	{
		gridManager->RemoveAllGridVisual();
		gridManager->ShowFromGridVisualDataArray(UnitAction->GetValidActionGridVisualDataArray());
	}

	if (OnButtonClickedCompleted.IsBound())
	{
		OnButtonClickedCompleted.Broadcast();
	}


}
