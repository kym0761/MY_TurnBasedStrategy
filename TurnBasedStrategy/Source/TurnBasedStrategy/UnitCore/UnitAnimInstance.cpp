// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAnimInstance.h"
#include "StatComponent.h"

#include "Manager/BattleManager.h"

UUnitAnimInstance::UUnitAnimInstance()
{

}

void UUnitAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	BindToBattleManager();
}

void UUnitAnimInstance::BindToBattleManager()
{
	auto battleManager = ABattleManager::GetBattleManager();
	if (IsValid(battleManager))
	{
		//공격과 관련된 Notify에 따라 발동되는 Delegate에 BattleManager의 기능을 바인드한다.

		FScriptDelegate bindToOnAttackHit;
		bindToOnAttackHit.BindUFunction(battleManager, FName("OnAttackHit"));
		OnAttackHit.Add(bindToOnAttackHit);

		FScriptDelegate bindToOnAttackEnd;
		bindToOnAttackEnd.BindUFunction(battleManager, FName("OnAttackEnd"));
		OnAttackEnd.Add(bindToOnAttackEnd);

		FScriptDelegate bindToOnHit;
		bindToOnHit.BindUFunction(battleManager, FName("OnHit"));
		OnHit.Add(bindToOnHit);

		FScriptDelegate bindToOnHitEnd;
		bindToOnHitEnd.BindUFunction(battleManager, FName("OnHitEnd"));
		OnHitEnd.Add(bindToOnHitEnd);
	}
}

void UUnitAnimInstance::AnimNotify_AttackHit()
{
	if (OnAttackHit.IsBound())
	{
		OnAttackHit.Broadcast();
	}
}

void UUnitAnimInstance::AnimNotify_AttackEnd()
{
	if (OnAttackEnd.IsBound())
	{
		OnAttackEnd.Broadcast();
	}
}

void UUnitAnimInstance::AnimNotify_Hit()
{
	if (OnHit.IsBound())
	{
		OnHit.Broadcast();
	}
}

void UUnitAnimInstance::AnimNotify_HitEnd()
{
	if (OnHitEnd.IsBound())
	{
		OnHitEnd.Broadcast();
	}
}

void UUnitAnimInstance::PlayUnitAttackMontage()
{
	if (!IsValid(UnitMontage))
	{
		return;
	}

	Montage_Play(UnitMontage);
	Montage_JumpToSection(FName("Attack01"));
}

void UUnitAnimInstance::PlayUnitHitMontage()
{
	if (!IsValid(UnitMontage))
	{
		return;
	}


	AActor* owner = GetOwningActor();
	UStatComponent* stat = nullptr;
	if (IsValid(owner))
	{
		stat = owner->FindComponentByClass<UStatComponent>();
	}

	float hp = 1.0f;
	if (IsValid(stat))
	{
		hp = stat->GetHP();
	}

	Montage_Play(UnitMontage);

	if (hp <= 0.0f)
	{
		Montage_JumpToSection(FName("Death01"));
	}
	else
	{
		Montage_JumpToSection(FName("Hit01"));
	}

}

void UUnitAnimInstance::PlayUnitAvoidMontage()
{
	if (!IsValid(UnitMontage))
	{
		return;
	}

	Montage_Play(UnitMontage);
	Montage_JumpToSection(FName("Avoid01"));
}
