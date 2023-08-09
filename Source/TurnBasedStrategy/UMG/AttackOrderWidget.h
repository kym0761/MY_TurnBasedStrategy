// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttackOrderWidget.generated.h"

class UTextBlock;

/**
 * 한 공격에 대한 Visual 정보를 담은 Widget
 * AttackCalculationWidget의 VerticalBox_AttackOrder 안에 들어감.
 * TODO : 유닛 죽음, 회복 등의 정보도 보일 수 있게 해야함.
 */
UCLASS(abstract)
class TURNBASEDSTRATEGY_API UAttackOrderWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		UTextBlock* TextBlock_Damage;

public:

	void InitAttackOrderWidget(FAttackOrder& Order);

};
