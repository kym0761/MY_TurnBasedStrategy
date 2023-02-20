// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAnimInstance.h"

void UUnitAnimInstance::AnimNotify_AttackHit()
{

	if (OnAttackHit.IsBound())
	{
		OnAttackHit.Broadcast();
		OnAttackHit.Clear();
	}
}

void UUnitAnimInstance::AnimNotify_AttackEnd()
{
	if (OnAttackEnd.IsBound())
	{
		OnAttackEnd.Broadcast();
		OnAttackEnd.Clear();
	}
}

void UUnitAnimInstance::AnimNotify_OnHit()
{
	if (OnHit.IsBound())
	{
		OnHit.Broadcast();
		OnHit.Clear();
	}
}

void UUnitAnimInstance::AnimNotify_HitEnd()
{
	if (OnHitEnd.IsBound())
	{
		OnHitEnd.Broadcast();
		OnHitEnd.Clear();
	}
}

void UUnitAnimInstance::BindToOnAttackHit(FScriptDelegate ToBind)
{
	OnAttackHit.Clear();
	OnAttackHit.Add(ToBind);
}

void UUnitAnimInstance::BindToOnAttackEnd(FScriptDelegate ToBind)
{
	OnAttackEnd.Clear();
	OnAttackEnd.Add(ToBind);
}

void UUnitAnimInstance::BindToOnHit(FScriptDelegate ToBind)
{
	OnHit.Clear();
	OnHit.Add(ToBind);
}

void UUnitAnimInstance::BindToHitEnd(FScriptDelegate ToBind)
{
	OnHitEnd.Clear();
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

	Montage_Play(UnitMontage);
	Montage_JumpToSection(FName("Hit01"));
}
