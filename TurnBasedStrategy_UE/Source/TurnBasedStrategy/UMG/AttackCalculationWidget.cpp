// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackCalculationWidget.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "UnitAction/UnitAttackActionComponent.h"
#include "CalculationUnitStatWidget.h"
#include "Components/TextBlock.h"
#include "AttackOrderWidget.h"

UAttackCalculationWidget::UAttackCalculationWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	//Button�� Focus�� ��������.
	if (IsValid(Button_Attack))
	{
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
		UE_LOG(LogTemp, Warning, TEXT("Attacker is Invalid. - UAttackCalculationWidget::InitAttackCalculationWidget()"));

		return;
	}

	if (!IsValid(Defender))
	{
		UE_LOG(LogTemp, Warning, TEXT("Defender is Invalid. - UAttackCalculationWidget::InitAttackCalculationWidget()"));

		return;
	}

	//�Ʊ� Stat �� �̸�
	CalculationStat_Ally->InitCalculationUnitStat(Attacker);
	if (IsValid(TextBlock_AllyName))
	{
		TextBlock_AllyName->SetText(FText::FromString(Attacker->GetActorLabel()));
	}

	//���� Stat �� �̸�
	CalculationStat_Enemy->InitCalculationUnitStat(Defender);
	if (IsValid(TextBlock_EnemyName))
	{
		TextBlock_EnemyName->SetText(FText::FromString(Defender->GetActorLabel()));
	}


	SetAttackOrders();

}

void UAttackCalculationWidget::OnButton_AttackClicked()
{
	//������ �����ϰ� Attack UI ����.

	AAttackManager* attackManager = AAttackManager::GetAttackManager();
	if (IsValid(attackManager))
	{
		attackManager->SetupAttackManager(Attacker, Defender);
		attackManager->StartAttack();
	}


	RemoveFromParent();
	MarkAsGarbage();
}

void UAttackCalculationWidget::SetAttackOrders()
{
	//AttackManager���Լ� Attacker�� Defender�� ���� ���� ����� ������.

	if (!IsValid(AllyAttackOrder) || !IsValid(EnemyAttackOrder))
	{
		UE_LOG(LogTemp, Warning, TEXT("Please Set up UMG BP. -- UAttackCalculationWidget::OnButton_AttackClicked()"));
		return;
	}

	AAttackManager* attackManager = AAttackManager::GetAttackManager();
	if (!IsValid(attackManager))
	{

		UE_LOG(LogTemp, Warning, TEXT("AttackManager is Invalid. -- UAttackCalculationWidget::OnButton_AttackClicked()"));
		return;
	}

	attackManager->SetupAttackManager(Attacker, Defender);
	AttackOrders = attackManager->GetAttackOrder();

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
