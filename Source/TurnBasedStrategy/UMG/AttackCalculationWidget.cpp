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

	//Button�� Focus�� ��������.
	if (IsValid(Button_Attack))
	{
		//TODO : IsFocusable Deprecated.. -> �ٵ� Set�Լ��� ���� �� ����.
		Button_Attack->IsFocusable = true;
	}
}

void UAttackCalculationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//Ȥ�ó� ������ Example Attack Order ���� �����ϰ� ������.
	VerticalBox_AttackOrder->ClearChildren();

	if (IsValid(Button_Attack))
	{
		Button_Attack->SetFocus();
	}
}

void UAttackCalculationWidget::InitAttackCalculationWidget(AActor* InAttacker, AActor* InDefender)
{
	// Attacker�� Defender �� ������ �����ְ�
	// AttackManager���Լ� AttackOrder ������ ������.

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

	//�Ʊ� Stat �� �̸�
	CalculationStat_Ally->UpdateCalculationUnitStat(Attacker, Defender);
	if (IsValid(TextBlock_AllyName))
	{
		TextBlock_AllyName->SetText(FText::FromString(Attacker->GetActorLabel()));
	}

	//���� Stat �� �̸�
	CalculationStat_Enemy->UpdateCalculationUnitStat(Defender, Attacker);
	if (IsValid(TextBlock_EnemyName))
	{
		TextBlock_EnemyName->SetText(FText::FromString(Defender->GetActorLabel()));
	}


	SetAttackOrders();

}

void UAttackCalculationWidget::OnButton_AttackClicked()
{
	//������ �����ϰ� Attack UI ����.

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
	//AttackManager���Լ� Attacker�� Defender�� ���� ���� ����� ������.

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

	//���� or �ݰ� ������ Parsing�Ͽ� UI�� ���� �ݿ�.
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
