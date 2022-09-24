// Fill out your copyright notice in the Description page of Project Settings.


#include "StatComponent.h"

// Sets default values for this component's properties
UStatComponent::UStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	HP = 100;
	MaxHP = 100;

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

	if (HP <= 0)
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
