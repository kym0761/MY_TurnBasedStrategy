// Fill out your copyright notice in the Description page of Project Settings.


#include "CalculationUnitStatWidget.h"
#include "../StatComponent.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UCalculationUnitStatWidget::InitCalculationUnitStat(AActor* ActorHasStatComponent)
{
	CurrentStatComponent =
		ActorHasStatComponent->FindComponentByClass<UStatComponent>();

	UpdateUnitStatWidget();

}

void UCalculationUnitStatWidget::UpdateUnitStatWidget()
{
	if (!IsValid(CurrentStatComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentStatComponent is Not Valid"));
		return;
	}

	//temporal..
	
	float hp = CurrentStatComponent->GetHP();
	float maxhp = CurrentStatComponent->GetMaxHP();
	ProgressBar_HP->SetPercent(hp / maxhp);

	TextBlock_HP->SetText(FText::FromString(FString::SanitizeFloat(hp,0)));

	int32 str = CurrentStatComponent->GetSTR();
	TextBlock_DMG->SetText(FText::FromString(FString::FromInt(str)));

	float hit = 99.0f;
	TextBlock_Hit->SetText(FText::FromString(FString::SanitizeFloat(hit)));


	float crit = 1.2f;
	TextBlock_Crit->SetText(FText::FromString(FString::SanitizeFloat(crit)));
}
