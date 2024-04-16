// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyDelegates.h"
#include "Animation/AnimInstance.h"
#include "UnitAnimInstance.generated.h"


/**
 * 이 프로젝트의 공격 방식은 턴방식이라서, 애니메이션에서 공격 행동과 피격 행동이 끝났음을 확인한 뒤에 다음 공격을 시도한다.
 * 
 * 공격자의 애니메이션에서 OnAttack이 발동하면, 상대가 Hit 애니메이션을 동작하면서 닳은 HP의 양을 표시해야한다.
 * OnAttackEnd는 공격자의 애니메이션이 끝났다는 것을 알린다.
 * 상대는 OnHitEnd가 동작해서 상대의 애니메이션도 끝난 것을 알려야한다.
 * 두 캐릭터의 애니메이션이 끝났다면, 다음 동작을 실행해야한다.
 * 
 * OnHit는 필요가 없을지도 모름 추후에 필요없는 것이 확정되면 삭제할 예정.
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
	FDynamicMulticastVoid OnAttackHit;
	UPROPERTY()
	FDynamicMulticastVoid OnAttackEnd;
	UPROPERTY()
	FDynamicMulticastVoid OnHit;
	UPROPERTY()
	FDynamicMulticastVoid OnHitEnd;

protected:

	virtual void NativeBeginPlay() override;

	void BindToBattleManager();

public:

	UFUNCTION()
	void AnimNotify_AttackHit();
	UFUNCTION()
	void AnimNotify_AttackEnd();
	UFUNCTION()
	void AnimNotify_Hit();
	UFUNCTION()
	void AnimNotify_HitEnd();

	void PlayUnitAttackMontage();
	void PlayUnitHitMontage();
	void PlayUnitAvoidMontage();
};
