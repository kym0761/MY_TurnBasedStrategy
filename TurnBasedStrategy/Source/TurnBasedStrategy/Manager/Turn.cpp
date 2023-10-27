// Fill out your copyright notice in the Description page of Project Settings.


#include "Turn.h"

FBattleOrder::FBattleOrder()
{
	OrderOwnerType = EOrderOwnerType::Attacker;
	Damage = 0;
	Attacker = nullptr;
	Defender = nullptr;
}
