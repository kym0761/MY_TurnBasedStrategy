// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackOrderWidget.h"
#include "Components/TextBlock.h"

void UAttackOrderWidget::InitAttackOrderWidget(FAttackOrder& Order)
{
	if (!IsValid(TextBlock_Damage))
	{
		return;
	}

	TextBlock_Damage->SetText(FText::FromString(FString::FromInt(Order.Damage)));

}
