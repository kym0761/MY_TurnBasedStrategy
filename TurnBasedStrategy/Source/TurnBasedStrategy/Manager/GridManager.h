// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid/Grid.h"
#include "GridManager.generated.h"

class UGridObject;
class UPathNode;
class AUnit;
class UBillboardComponent;
class UInstancedGridVisualComponent;
class UPathFindingSystem;
class UGridSystem;

class ASRPG_GameMode;

/* 
 * GridSystem�� PathFindingSystem�� ��ȭ�� ���ؼ�
 * �� System�� ������ UObject�� �����ϰ�
 * PathFinding���� ����� GridManager���� �����ؾ���.
*/

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnyUnitMoved);

UCLASS()
class TURNBASEDSTRATEGY_API AGridManager : public AActor
{
	GENERATED_BODY()
	
#pragma region PrivateData
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
		UBillboardComponent* BillboardComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
	UGridSystem* GridSystem;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
	UPathFindingSystem* PathFindingSystem;

	/*
	* GridVisual
	* �� Instanced Mesh Component���� Grid�� ������ �ð������� �����ִ� ������ �Ѵ�.
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
		UInstancedGridVisualComponent* GridVisual_Move;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
		UInstancedGridVisualComponent* GridVisual_OK;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
		UInstancedGridVisualComponent* GridVisual_NO;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
		UInstancedGridVisualComponent* GridVisual_Warning;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
		UInstancedGridVisualComponent* GridVisual_DANGER;

	UPROPERTY()
		ASRPG_GameMode* SRPG_GameModeRef;

#pragma endregion

public:
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = true))
		int32 X_Length = 10;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = true))
		int32 Y_Length = 10;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = true))
		float CellSize = 100.0f;

public:	
	// Sets default values for this actor's properties
	AGridManager();

	FOnAnyUnitMoved OnAnyUnitMoved;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//virtual void OnConstruction(const FTransform& Transform);

	////Init
	//void CreateGridSystem();

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Setup();

	//*GridVisual Functions*/
	void ShowGridRange(const FGrid& Grid, int32 Range, EGridVisualType GridVisualType);
	void ShowFromGridSet(const TSet<FGrid>& GridSet, EGridVisualType GridVisualType, const float Height = 0.01f);
	void ShowFromGridVisualDataSet(const TSet<FGridVisualData>& GridVisualDataSet, const float Height = 0.01f);
	void RemoveAllGridVisual();


	static AGridManager* GetGridManager();

	//Test EnemyRangeFunction
	//���� BP_GridManager���� R��ư �Է¿� ���� Bind�� ���·� ������.
	//���Ŀ� ������ ����.
	UFUNCTION(BlueprintCallable)
		void ShowEnemyRange();

	UFUNCTION(BlueprintCallable)
		void ClearEnemyRange();
};