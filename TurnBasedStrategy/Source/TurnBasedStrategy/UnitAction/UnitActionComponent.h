// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UnitAction.h"
#include "UnitActionComponent.generated.h"

class AUnit;
class ASRPG_GameMode;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnitActionDelegate);

UCLASS(abstract /*, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent)*/)
class TURNBASEDSTRATEGY_API UUnitActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUnitActionComponent();

	FUnitActionDelegate OnActionCompleteForControlPawn;
protected:

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	//AUnit* Unit;

	//�ִ� Action ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	int32 MaxActionRange;

	//������ �̹��Ͽ� �ൿ ��������?
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bCanDoActionThisTurn;

	//Action�� �̸�. �����ڿ��� ActionName�� �����ϸ� UI���� ����.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	FString ActionName;

private:

	//�ܺ��� ����� �������� Call�ϱ� ����. �Ƹ� �������� ���� �� ���ɼ��� ����.
	//�̴� ActionStart()���� Broadcast�ǹǷ�, �ٸ� ������ ���� Call�Ϸ��� ������ ��.
	//�Ʒ� 3���� ��������...
	//����� ���� �Ǽ��� �����ϱ� ���� private���� ����������, ���߿� public���� �ٲ� ���ɼ��� ����.
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
	AUnit* GetOwningUnit() const;

	//UI�� ������� Action�� ���ؼ� TakeAction ������ �ߵ��ؾ���.
	virtual void DealWithGridBeforeAction(const FGrid& Grid);

	virtual void TakeAction(const FGrid& Grid);
	virtual bool IsValidActionGrid(const FGrid& Grid) const;
	virtual TSet<FGridVisualData> GetValidActionGridVisualDataSet() const;
	virtual TSet<FGrid> GetValidActionGridSet() const;

	//AUnitCharacter* GetUnit() const;

	bool IsCanDoActionThisTurn() const;
	void SetCanDoActionThisTurn(bool InputBool);
	bool IsCurrentlyAvailableAction() const;

	/*AI �ൿ*/
	virtual FGrid ThinkAIBestActionGrid();
	virtual int32 CalculateActionValue(FGrid& CandidateGrid);
	virtual void AI_Action();

	virtual void SelectThisAction();

	void BindToOnActionStart(FScriptDelegate ToBind);
	void BindToOnActionEnd(FScriptDelegate ToBind);
	void BindToOnActionSelected(FScriptDelegate ToBind);

protected:

	//Action ������ �� ������ Call�ؾ���. Super:: �ʿ���.
	virtual void ActionStart();
	//Action�� ���� �� ������ Call�ؾ���. Super:: �ʿ���.
	virtual void ActionEnd();
	//Action�� ���õ��� �� ������ Call�ؾ���. Super:: �ʿ���. - ?? �̰� �Ƹ� �ܺο��� �Ҹ��� �� ��.
	virtual void ActionSelected();

};
