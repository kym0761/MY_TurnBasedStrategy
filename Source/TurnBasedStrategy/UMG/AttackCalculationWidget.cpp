// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackCalculationWidget.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "UnitAction/UnitAttackActionComponent.h"
#include "CalculationUnitStatWidget.h"
#include "Components/TextBlock.h"
#include "AttackOrderWidget.h"
#include "Manager/SRPG_GameMode.h"

#include "DebugHelper.h"

UAttackCalculationWidget::UAttackCalculationWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	//Button이 Focus가 잡혀야함.
	if (IsValid(Button_Attack))
	{
		//TODO : IsFocusable Deprecated.. -> 근데 Set함수가 없는 것 같음.
		Button_Attack->IsFocusable = true;
	}
}

void UAttackCalculationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//혹시나 존재할 Example Attack Order 들을 제거하고 시작함.
	VerticalBox_AttackOrder->ClearChildren();

	if (IsValid(Button_Attack))
	{
		Button_Attack->SetFocus();
	}
}

void UAttackCalculationWidget::InitAttackCalculationWidget(AActor* InAttacker, AActor* InDefender)
{
	// Attacker와 Defender 의 스탯을 보여주고
	// AttackManager에게서 AttackOrder 정보를 가져옴.

	if (IsValid(Button_Attack))
	{
		Button_Attack->OnClicked.AddDynamic(this, &UAttackCalculationWidget::OnButton_AttackClicked);
	}

	this->Attacker = InAttacker;
	this->Defender = InDefender;

	if (!IsValid(Attacker))
	{
		Debug::Print(DEBUG_TEXT("Attacker is Invalid."));
		return;
	}

	if (!IsValid(Defender))
	{
		Debug::Print(DEBUG_TEXT("Defender is Invalid."));
		return;
	}

	//아군 Stat 및 이름
	CalculationStat_Ally->UpdateCalculationUnitStat(Attacker, Defender);
	if (IsValid(TextBlock_AllyName))
	{
		TextBlock_AllyName->SetText(FText::FromString(Attacker->GetActorLabel()));
	}

	//적군 Stat 및 이름
	CalculationStat_Enemy->UpdateCalculationUnitStat(Defender, Attacker);
	if (IsValid(TextBlock_EnemyName))
	{
		TextBlock_EnemyName->SetText(FText::FromString(Defender->GetActorLabel()));
	}


	SetAttackOrders();

}

void UAttackCalculationWidget::OnButton_AttackClicked()
{
	//공격을 실행하고 Attack UI 제거.

	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());

	if (IsValid(gameMode))
	{
		gameMode->SetupAttackManaging(Attacker, Defender);
		gameMode->StartAttack();
	}


	RemoveFromParent();
	MarkAsGarbage();
}

void UAttackCalculationWidget::SetAttackOrders()
{
	//AttackManager에게서 Attacker와 Defender의 공격 예상 결과를 가져옴.

	if (!IsValid(AllyAttackOrder) || !IsValid(EnemyAttackOrder))
	{
		Debug::Print(DEBUG_TEXT("Please Set up UMG BP"));
		return;
	}

	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
	if (!IsValid(gameMode))
	{
		Debug::Print(DEBUG_TEXT("gameMode is Invalid."));
		return;
	}

	gameMode->SetupAttackManaging(Attacker, Defender);
	AttackOrders = gameMode->GetAttackOrder();

	//공격 or 반격 정보를 Parsing하여 UI에 최종 반영.
	for (FAttackOrder& order : AttackOrders)
	{
		UAttackOrderWidget* orderWidget = nullptr;
		switch (order.AttackOrderType)
		{
		case EAttackOrderType::Attack:
			orderWidget = CreateWidget<UAttackOrderWidget>(GetWorld(), AllyAttackOrder);
			break;
		case EAttackOrderType::Defend:
			orderWidget = CreateWidget<UAttackOrderWidget>(GetWorld(), EnemyAttackOrder);
			break;
		default:
			orderWidget = CreateWidget<UAttackOrderWidget>(GetWorld(), AllyAttackOrder);
			break;
		}

		if (IsValid(orderWidget))
		{
			VerticalBox_AttackOrder->AddChild(orderWidget);
			orderWidget->InitAttackOrderWidget(order);
		}

	}

}
