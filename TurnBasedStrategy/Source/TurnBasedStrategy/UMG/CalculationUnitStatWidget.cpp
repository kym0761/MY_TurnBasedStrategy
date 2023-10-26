// Fill out your copyright notice in the Description page of Project Settings.


#include "CalculationUnitStatWidget.h"
#include "UnitCore/StatComponent.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
//#include "Manager/SRPG_GameMode.h"

#include "Manager/BattleManager.h"

#include "DebugHelper.h"

void UCalculationUnitStatWidget::UpdateCalculationUnitStat(AActor* StatOwner, AActor* Opponent)
{
	auto battleManager = ABattleManager::GetBattleManager();

	if (!battleManager)
	{
		Debug::Print("battleManager is Invalid.");
		return;
	}

	
	auto ownerStat = StatOwner->FindComponentByClass<UStatComponent>();

	float hp = ownerStat->GetHP();
	float maxhp = ownerStat->GetMaxHP();
	ProgressBar_HP->SetPercent(hp / maxhp);

	TextBlock_HP->SetText(FText::FromString(FString::SanitizeFloat(hp, 0)));

	int32 str = ownerStat->GetSTR();
	TextBlock_DMG->SetText(FText::FromString(FString::FromInt(str)));

	float hit = battleManager->CalculateAccuracy(StatOwner, Opponent) * 100.0f;
	TextBlock_Hit->SetText(FText::FromString(FString::SanitizeFloat(hit) + FString(" %")));

	float crit = battleManager->CalculateCriticalRate(StatOwner, Opponent) * 100.0f;
	TextBlock_Crit->SetText(FText::FromString(FString::SanitizeFloat(crit) + FString(" %")));

}