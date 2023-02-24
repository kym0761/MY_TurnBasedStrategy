// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageTextWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetTextLibrary.h"

void UDamageTextWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UDamageTextWidget::SetDamageText(float InDamage)
{
	if (DamageTextBlock)
	{
		FText temp = UKismetTextLibrary::Conv_DoubleToText(InDamage, ERoundingMode::HalfToEven);

		DamageTextBlock->SetText(temp);
	}
}