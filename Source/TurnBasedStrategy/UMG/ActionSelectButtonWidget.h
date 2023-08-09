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
 * 유닛이 가능한 행동에 대한 버튼
 * UnitActionListWidget에 들어감.
 */
UCLASS(abstract)
class TURNBASEDSTRATEGY_API UActionSelectButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UActionSelectButtonWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		UTextBlock* TextBlock_ActionName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		UButton* Button_Select;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UUnitActionComponent* UnitAction;


	FOnButtonClickedCompleted OnButtonClickedCompleted;
protected:

	virtual void NativeConstruct() override;

public:

	void InitActionSelectButton(UUnitActionComponent* InputAction);
	void InitButton();

	UFUNCTION()
	void OnButtonClicked();

	void SetFocusThisButton();
};
