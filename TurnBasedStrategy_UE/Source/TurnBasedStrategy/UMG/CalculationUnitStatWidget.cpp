// Fill out your copyright notice in the Description page of Project Settings.


#include "CalculationUnitStatWidget.h"
#include "UnitCore/StatComponent.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Manager/SRPG_GameMode.h"

void UCalculationUnitStatWidget::UpdateCalculationUnitStat(AActor* StatOwner, AActor* Opponent)
{
	auto gamemode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());

	if (!gamemode)
	{
		return;
	}

	
	auto ownerStat = StatOwner->FindComponentByClass<UStatComponent>();
	//auto opponentStat = Opponent->FindComponentByClass<UStatComponent>();


	float hp = ownerStat->GetHP();
	float maxhp = ownerStat->GetMaxHP();
	ProgressBar_HP->SetPercent(hp / maxhp);

	TextBlock_HP->SetText(FText::FromString(FString::SanitizeFloat(hp, 0)));

	int32 str = ownerStat->GetSTR();
	TextBlock_DMG->SetText(FText::FromString(FString::FromInt(str)));

	float hit = gamemode->CalculateAccuracy(StatOwner, Opponent) * 100.0f;
	TextBlock_Hit->SetText(FText::FromString(FString::SanitizeFloat(hit) + FString(" %")));

	float crit = gamemode->CalculateCriticalRate(StatOwner, Opponent) * 100.0f;
	TextBlock_Crit->SetText(FText::FromString(FString::SanitizeFloat(crit) + FString(" %")));

}