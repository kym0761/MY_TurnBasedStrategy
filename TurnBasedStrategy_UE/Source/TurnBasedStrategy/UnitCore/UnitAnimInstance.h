// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "UnitAnimInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNotifyWorks);
/**
 * 
 */
UCLASS()
class TURNBASEDSTRATEGY_API UUnitAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:

	UUnitAnimInstance();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AnimMontage")
		UAnimMontage* UnitMontage;

	UPROPERTY()
		FOnNotifyWorks OnAttackHit;
	UPROPERTY()
		FOnNotifyWorks OnAttackEnd;
	UPROPERTY()
		FOnNotifyWorks OnHit;
	UPROPERTY()
		FOnNotifyWorks OnHitEnd;

protected:

public:

	UFUNCTION()
		void AnimNotify_AttackHit();
	UFUNCTION()
		void AnimNotify_AttackEnd();
	UFUNCTION()
		void AnimNotify_Hit();
	UFUNCTION()
		void AnimNotify_HitEnd();

	void BindTo_OnAttackHit(FScriptDelegate ToBind);
	void BindTo_OnAttackEnd(FScriptDelegate ToBind);
	void BindTo_OnHit(FScriptDelegate ToBind);
	void BindTo_OnHitEnd(FScriptDelegate ToBind);

	void PlayUnitAttackMontage();
	void PlayUnitHitMontage();
};
