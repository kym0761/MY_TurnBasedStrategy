// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AttackManager.generated.h"

class UUnitAnimInstance;
class UUnitAttackActionComponent;

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

	FAttackOrder();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Order")
		EAttackOrderType AttackOrderType;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Order")
		int32 Damage;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Order")
		AActor* Attacker;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Order")
		AActor* Defender;

};

UCLASS()
class TURNBASEDSTRATEGY_API AAttackManager : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AAttackManager();

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack", Meta = (AllowPrivateAccess = "true"))
		TArray<FAttackOrder> OrderToPlay;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack", Meta = (AllowPrivateAccess = "true"))
		FAttackOrder CurrentAttackOrder;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack", Meta = (AllowPrivateAccess = "true"))
		bool bAttackerWaiting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack", Meta = (AllowPrivateAccess = "true"))
		bool bDefenderWaiting = false;

	UPROPERTY()
	UUnitAttackActionComponent* CurrentAttackActionComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetupAttackManager(AActor* Attacker, AActor* Defender);

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

	TArray<FAttackOrder> CalculateAttackOrder(AActor* Attacker, AActor* Defender);

	static AAttackManager* GetAttackManager();


	void BindOnAttackHit(UUnitAnimInstance* AnimInst);
	void BindOnAttackEnd(UUnitAnimInstance* AnimInst);
	void BindOnHit(UUnitAnimInstance* AnimInst);
	void BindOnHitEnd(UUnitAnimInstance* AnimInst);

	void TryPlayNextOrder();


	TArray<FAttackOrder> GetAttackOrder() const;

	int32 CalculateGridValue_ToAttack(AActor* Attacker, AActor* Defender);
};
