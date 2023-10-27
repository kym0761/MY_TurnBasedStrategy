// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UnitAction.h"
#include "UnitActionComponent.generated.h"

class AUnit;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnitActionDelegate);

UCLASS(abstract /*, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent)*/)
class TURNBASEDSTRATEGY_API UUnitActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUnitActionComponent();

protected:

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	//AUnit* Unit;

	//최대 Action 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	int32 MaxActionRange;

	//유닛이 이번턴에 행동 가능한지?
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bCanDoActionThisTurn;

	//Action의 이름. 생성자에서 ActionName을 세팅하면 UI에서 보임.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	FString ActionName;

public:

	//이는 ActionStart()에서 Broadcast되므로, 다른 곳에서 직접 Call하려고 하지말 것.
	//아래 3개도 마찬가지...
	FUnitActionDelegate OnActionStart;
	FUnitActionDelegate OnActionEnd;
	FUnitActionDelegate OnActionSelected;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void InitializeComponent() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FString GetActionName() const;
	AUnit* GetOwningUnit() const;

	//UI를 띄워야할 Action을 위해서 TakeAction 이전에 발동해야함.
	virtual void DealWithGridBeforeAction(const FGrid& Grid);

	virtual void TakeAction(const FGrid& Grid);
	virtual bool IsValidActionGrid(const FGrid& Grid) const;
	virtual TSet<FGridVisualData> GetValidActionGridVisualDataSet() const;
	virtual TSet<FGrid> GetValidActionGridSet() const;

	//AUnitCharacter* GetUnit() const;

	bool IsCanDoActionThisTurn() const;
	void SetCanDoActionThisTurn(bool InputBool);
	bool IsCurrentlyAvailableAction() const;

	/*AI 행동*/
	virtual FGrid ThinkAIBestActionGrid();
	virtual int32 CalculateActionValue(FGrid& CandidateGrid);
	virtual void AI_Action();

	virtual void SelectThisAction();

protected:

	//Action 시작할 때 무조건 Call해야함. Super:: 필요함.
	virtual void ActionStart();
	//Action이 끝날 때 무조건 Call해야함. Super:: 필요함.
	virtual void ActionEnd();
	//Action이 선택됐을 때 무조건 Call해야함. Super:: 필요함. - ?? 이거 아마 외부에서 불리게 될 듯.
	virtual void ActionSelected();

};
