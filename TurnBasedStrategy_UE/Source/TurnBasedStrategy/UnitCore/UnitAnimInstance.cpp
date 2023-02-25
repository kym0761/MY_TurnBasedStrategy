// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAnimInstance.h"
#include "StatComponent.h"

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

void UUnitAnimInstance::BindTo_OnAttackHit(FScriptDelegate ToBind)
{
	if (OnAttackHit.IsBound())
	{
		OnAttackHit.Clear();
	}
	OnAttackHit.Add(ToBind);
}

void UUnitAnimInstance::BindTo_OnAttackEnd(FScriptDelegate ToBind)
{
	if (OnAttackEnd.IsBound())
	{
		OnAttackEnd.Clear();
	}
	OnAttackEnd.Add(ToBind);
}

void UUnitAnimInstance::BindTo_OnHit(FScriptDelegate ToBind)
{
	if (OnHit.IsBound())
	{
		OnHit.Clear();
	}
	OnHit.Add(ToBind);
}

void UUnitAnimInstance::BindTo_OnHitEnd(FScriptDelegate ToBind)
{
	if (OnHitEnd.IsBound())
	{
		OnHitEnd.Clear();
	}
	OnHitEnd.Add(ToBind);
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

	APawn* owner = TryGetPawnOwner();
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