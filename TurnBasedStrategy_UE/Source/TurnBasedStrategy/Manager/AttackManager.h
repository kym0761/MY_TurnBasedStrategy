// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AttackManager.generated.h"

class UUnitAnimInstance;

UENUM(BlueprintType)
enum class EAttackOrderType : uint8
{
	Attack UMETA(DisplayName = "Attack"),

	Defend UMETA(DisplayName = "Defend")

};

USTRUCT(BlueprintType, Blueprintable)
struct FAttackOrder
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Order")
	EAttackOrderType AttackOrderType;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Order")
	int32 Damage;

};

UCLASS()
class TURNBASEDSTRATEGY_API AAttackManager : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AAttackManager();

private:
	UPROPERTY()
		AActor* Attacker;

	UPROPERTY()
		AActor* Defender;


	UPROPERTY()
		AActor* CurrentAttacker;

	UPROPERTY()
		AActor* CurrentDefender;

	UPROPERTY()
	TArray<FAttackOrder> OrderToPlay;

	UPROPERTY()
		bool bAttackerWaiting = false;

	UPROPERTY()
		bool bDefenderWaiting = false;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetupAttackManager(AActor* InAttacker, AActor* InDefender);

	void StartAttack();
	void PlayAttack();
	void FinishAttack();

	UFUNCTION()
		void OnAttackHit();
	UFUNCTION()
		void OnAttackEnd();
	UFUNCTION()
		void OnHit();
	UFUNCTION()
		void OnHitEnd();

	TArray<FAttackOrder> CalculateAttackOrder();
	static AAttackManager* GetAttackManager();


	void BindOnAttackHit(UUnitAnimInstance* AnimInst);
	void BindOnAttackEnd(UUnitAnimInstance* AnimInst);
	void BindOnHit(UUnitAnimInstance* AnimInst);
	void BindOnHitEnd(UUnitAnimInstance* AnimInst);

	void TryPlayNextOrder();
};
