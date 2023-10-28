// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid/Grid.h"
#include "Manager/Turn.h"
#include "UnitAction/UnitAction.h"
#include "Unit.generated.h"

class UStatComponent;
class UUnitMoveActionComponent;
class UUnitActionComponent;
class UUnitAttackActionComponent;
class UUnitInteractActionComponent;
class UUnitWaitActionComponent;
class ADamageTextActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnitDelegateSignature);

UCLASS()
class TURNBASEDSTRATEGY_API AUnit : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUnit();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component", Meta = (AllowPrivateAccess = true))
	USceneComponent* SceneRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Reaction", Meta = (AllowPrivateAccess = true))
		TSubclassOf<ADamageTextActor> DamageTextActorBP;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component", Meta = (AllowPrivateAccess = true))
		UStatComponent* StatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component", Meta = (AllowPrivateAccess = true))
		UUnitMoveActionComponent* UnitMoveActionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component", Meta = (AllowPrivateAccess = true))
		UUnitAttackActionComponent* UnitAttackActionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component", Meta = (AllowPrivateAccess = true))
		UUnitInteractActionComponent* UnitInteractActionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component", Meta = (AllowPrivateAccess = true))
		UUnitWaitActionComponent* UnitWaitActionComponent;

	FUnitDelegateSignature OnFinishAllAction;
	FUnitDelegateSignature OnUnitDestroyed;
private:

	////현재 Grid 위치 값
	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	//	FGrid Grid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn", Meta = (AllowPrivateAccess = true))
	ETeamType TeamType;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void BeginDestroy() override;

	void BindToBattleManager();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

	FGrid GetGrid();

	bool HasActionComponent(EUnitActionType UnitActionType);
	UUnitActionComponent* GetUnitActionComponent(EUnitActionType UnitActionType);

	//UFUNCTION()
	//	void StartUnitTurn();

	UFUNCTION()
		void OnSelectedUnitChanged();

	void ActivateUnitAllActions();
	void FinishUnitAllAction();

	//AI Task에서 사용할 수 있도록 BlueprintCallable
	UFUNCTION(BlueprintCallable)
	bool IsThisUnitCanAction() const;

	ETeamType GetTeamType() const;
};
