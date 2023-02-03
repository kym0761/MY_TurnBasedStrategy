// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitActionListWidget.generated.h"

class UVerticalBox;
class AUnitCharacter;
class UActionSelectButtonWidget;
/**
 * 
 */
UCLASS(abstract)
class TURNBASEDSTRATEGY_API UUnitActionListWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Component")
		TSubclassOf<UActionSelectButtonWidget> ChooseActionButtonWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		UVerticalBox* VerticalBox_ActionList;

	void InitUnitActionsWidget(AUnitCharacter* SelectedCharacter);

	UFUNCTION()
		void OnButtonClickedCompletedFunc();

};
