// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Manager/AttackManager.h"
#include "AttackOrderWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class TURNBASEDSTRATEGY_API UAttackOrderWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		UTextBlock* TextBlock_Damage;


public:

	void InitAttackOrderWidget(FAttackOrder& Order);

};
