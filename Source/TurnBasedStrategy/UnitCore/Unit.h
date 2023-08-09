// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid/Grid.h"
#include "UnitAction/UnitAction.h"
#include "Unit.generated.h"

class UStatComponent;
class UUnitMoveActionComponent;
class UUnitActionComponent;
class UUnitAttackActionComponent;
class UUnitInteractActionComponent;
class UWaitActionComponent;
class ADamageTextActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnitDelegateSignature_n);

UCLASS()
class TURNBASEDSTRATEGY_API AUnit : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUnit();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	USceneComponent* SceneRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Reaction")
		TSubclassOf<ADamageTextActor> DamageTextActorBP;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		UStatComponent* StatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		UUnitMoveActionComponent* UnitMoveActionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		UUnitAttackActionComponent* UnitAttackActionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		UUnitInteractActionComponent* UnitInteractActionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		UWaitActionComponent* WaitActionComponent;

	FUnitDelegateSignature_n OnFinishAllAction;

private:

	////현재 Grid 위치 값
	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	//	FGrid Grid;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

	FGrid GetGrid();

	bool HasActionComponent(EUnitActionType UnitActionType);
	UUnitActionComponent* GetUnitActionComponent(EUnitActionType UnitActionType);

	void InitUnit();

	UFUNCTION()
		void StartUnitTurn();

	UFUNCTION()
		void OnSelectedUnitChanged();

	void ActivateUnitAllAction();
	void FinishUnitAllAction();

	bool IsThisUnitCanAction() const;

};
