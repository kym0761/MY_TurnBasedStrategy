// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Turn.h"
#include "BattleManager.generated.h"

class UUnitAttackActionComponent;
class UUnitAnimInstance;

UCLASS()
class TURNBASEDSTRATEGY_API ABattleManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABattleManager();

	static ABattleManager* GetBattleManager();

protected:
	
	//한번 공격을 시도할 때 실행되는 공격 목록
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack", Meta = (AllowPrivateAccess = "true"))
	TArray<FBattleOrder> OrdersToPlay;

	//Attacker Cache
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> AttackActionUnit;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack", Meta = (AllowPrivateAccess = "true"))
	bool bAttackerWaiting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack", Meta = (AllowPrivateAccess = "true"))
	bool bDefenderWaiting = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*Battle Manager Functions*/
	TArray<FBattleOrder> CalculateBattleOrders(AActor* Attacker, AActor* Defender);
	float CalculateCriticalRate(AActor* Attacker, AActor* Defender);
	float CalculateAccuracy(AActor* Attacker, AActor* Defender);

	/* 동작 순서. 
	 
	StartBattle (+SetupBattle)->
	PlayBattle -> OnPlayBattleAnimationCompleted // OrdersToPlay가 빌 때까지 Loop
	->FinishBattle
	*/
	void SetupBattle(AActor* Attacker, AActor* Defender);
	void StartBattle(AActor* Attacker, AActor* Defender);
	void PlayBattle();
	void OnPlayBattleAnimationCompleted();
	void FinishBattle();


	UFUNCTION()
	void OnAttackHit();
	UFUNCTION()
	void OnAttackEnd();
	UFUNCTION()
	void OnHit();
	UFUNCTION()
	void OnHitEnd();
	UFUNCTION()
	void OnUnitDestroyed();

	TArray<FBattleOrder> GetBattleOrder() const;
};
