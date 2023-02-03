// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ActionSelectButtonWidget.generated.h"

class UButton;
class UTextBlock;
class UUnitActionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonClickedCompleted);

/**
 * 
 */
UCLASS(abstract)
class TURNBASEDSTRATEGY_API UActionSelectButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		UTextBlock* TextBlock_ActionName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		UButton* Button_Select;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UUnitActionComponent* UnitAction;


	FOnButtonClickedCompleted OnButtonClickedCompleted;
protected:



public:

	void InitActionSelectButton(UUnitActionComponent* InputAction);
	void InitButton();

	UFUNCTION()
	void OnButtonClicked();
};
