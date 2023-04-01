// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UnitAction.h"
#include "UnitActionComponent.generated.h"

class AUnitCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnitActionDelegate);

UCLASS(abstract ,ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TURNBASEDSTRATEGY_API UUnitActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUnitActionComponent();

	FUnitActionDelegate OnActionCompleteForControlPawn;
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	AUnitCharacter* Unit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	int32 MaxActionRange;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bCanDoActionThisTurn;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	FString ActionName;

private:

	//외부의 기능을 동적으로 Call하기 위함. 아마 쓸때마다 비우게 될 가능성이 높음.
	//이는 ActionStart()에서 Broadcast되므로, 다른 곳에서 직접 Call하려고 하지말 것.
	//아래 3개도 마찬가지...
	//현재는 추후 실수를 방지하기 위해 private으로 설정했지만, 나중에 public으로 바꿀 가능성도 있음.
	FUnitActionDelegate OnActionStart;
	FUnitActionDelegate OnActionEnd;
	FUnitActionDelegate OnActionSelected;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	FString GetActionName() const;

	//UI를 띄워야할 Action을 위해서 TakeAction 이전에 발동해야함.
	virtual void DealWithGridBeforeAction(const FGrid& Grid);

	virtual void TakeAction(const FGrid& Grid);
	virtual bool IsValidActionGrid(const FGrid& Grid) const;
	virtual TArray<FGridVisualData> GetValidActionGridVisualDataArray() const;
	virtual TArray<FGrid> GetValidActionGridArray() const;
	AUnitCharacter* GetUnit() const;

	bool IsCanDoActionThisTurn() const;
	void SetCanDoActionThisTurn(bool InputBool);
	bool IsCurrentlyAvailableAction() const;


	virtual FGrid ThinkAIBestActionGrid();
	virtual int32 CalculateActionValue(FGrid& CandidateGrid);
	virtual void TestFunction();

	UFUNCTION(BlueprintCallable)
		void TestUnitAction();

	virtual void SelectThisAction();

	void BindToOnActionStart(FScriptDelegate ToBind);
	void BindToOnActionEnd(FScriptDelegate ToBind);
	void BindToOnActionSelected(FScriptDelegate ToBind);

protected:

	//Action 시작할 때 무조건 Call해야함. Super:: 필요함.
	virtual void ActionStart();
	//Action이 끝날 때 무조건 Call해야함. Super:: 필요함.
	virtual void ActionEnd();
	//Action이 선택됐을 때 무조건 Call해야함. Super:: 필요함. - ?? 이거 아마 외부에서 불리게 될 듯.
	virtual void ActionSelected();

};
