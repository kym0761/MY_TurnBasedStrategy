// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "UnitAnimInstance.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackHit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHitEnd);
/**
 * 
 */
UCLASS()
class TURNBASEDSTRATEGY_API UUnitAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:

	FOnAttackHit OnAttackHit;
	FOnAttackEnd OnAttackEnd;
	FOnHit OnHit;
	FOnHitEnd OnHitEnd;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AnimMontage")
		UAnimMontage* UnitMontage;

	UFUNCTION()
		void AnimNotify_AttackHit();
	UFUNCTION()
		void AnimNotify_AttackEnd();
	UFUNCTION()
		void AnimNotify_OnHit();
	UFUNCTION()
		void AnimNotify_HitEnd();

	void BindToOnAttackHit(FScriptDelegate ToBind);
	void BindToOnAttackEnd(FScriptDelegate ToBind);
	void BindToOnHit(FScriptDelegate ToBind);
	void BindToHitEnd(FScriptDelegate ToBind);

	void PlayUnitAttackMontage();
	void PlayUnitHitMontage();
};
