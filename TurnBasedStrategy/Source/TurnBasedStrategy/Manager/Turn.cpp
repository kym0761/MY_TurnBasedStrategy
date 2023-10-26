// Fill out your copyright notice in the Description page of Project Settings.


#include "Turn.h"

FAttackOrder::FAttackOrder()
{
	AttackOrderType = EAttackOrderType::Attack;
	Damage = 0;
	Attacker = nullptr;
	Defender = nullptr;
}

FBattleOrder::FBattleOrder()
{
	OrderOwnerType = EOrderOwnerType::Attacker;
	Damage = 0;
	Attacker = nullptr;
	Defender = nullptr;
}
