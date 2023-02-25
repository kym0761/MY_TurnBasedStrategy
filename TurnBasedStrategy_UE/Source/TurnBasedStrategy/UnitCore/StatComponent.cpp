// Fill out your copyright notice in the Description page of Project Settings.


#include "StatComponent.h"

// Sets default values for this component's properties
UStatComponent::UStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	HP = 20;
	MaxHP = 20;


	STR = 5;
	INT = 5;
	SPD = 5;
	SKL = 5;
	LUK = 5;
	DEF = 5;
	RES = 5;

}


// Called when the game starts
void UStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UStatComponent::DealDamage(float DamageAmount)
{
	HP = FMath::Clamp(HP - DamageAmount, 0, MaxHP);

	if (HP <= 0.0f)
	{
		//death
	}

	return 0.0f;
}

float UStatComponent::GetHP() const
{
	return HP;
}

float UStatComponent::GetMaxHP() const
{
	return MaxHP;
}

int32 UStatComponent::GetSTR() const
{
	return STR;
}

int32 UStatComponent::GetINT() const
{
	return INT;
}

int32 UStatComponent::GetSPD() const
{
	return SPD;
}

int32 UStatComponent::GetSKL() const
{
	return SKL;
}

int32 UStatComponent::GetLUK() const
{
	return LUK;
}

int32 UStatComponent::GetDEF() const
{
	return DEF;
}

int32 UStatComponent::GetRES() const
{
	return RES;
}
