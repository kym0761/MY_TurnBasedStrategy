// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Manager/AttackManager.h"
#include "AttackCalculationWidget.generated.h"

class UVerticalBox;
class UButton;
class UCalculationUnitStatWidget;
class UTextBlock;
class UAttackOrderWidget;
/**
 * 
 */
UCLASS()
class TURNBASEDSTRATEGY_API UAttackCalculationWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (BindWidget))
	UVerticalBox* VerticalBox_AttackOrder;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		UButton* Button_Attack;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		UCalculationUnitStatWidget* CalculationStat_Ally;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		UCalculationUnitStatWidget* CalculationStat_Enemy;

	UPROPERTY()
		AActor* Attacker;

	UPROPERTY()
		AActor* Defender;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		UTextBlock* TextBlock_AllyName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		UTextBlock* TextBlock_EnemyName;
	
	UPROPERTY()
	TArray<FAttackOrder> AttackOrders;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category ="AttackOrder")
	TSubclassOf<UAttackOrderWidget> AllyAttackOrder;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackOrder")
	TSubclassOf<UAttackOrderWidget> EnemyAttackOrder;

protected:

	virtual void NativeConstruct() override;

public:

	void InitAttackCalculationWidget(AActor* InAttacker, AActor* InDefender);

	UFUNCTION()
	void OnButton_AttackClicked();

	void SetAttackOrders();

};
