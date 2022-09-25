// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitMoveActionComponent.h"
#include "UnitCharacter.h"

#include "GridManager.h"
#include "Kismet/GameplayStatics.h"


UUnitMoveActionComponent::UUnitMoveActionComponent()
{
	MaxActionRange = 5;
}

void UUnitMoveActionComponent::BeginPlay()
{
	Super::BeginPlay();

	Unit = Cast<AUnitCharacter>(GetOwner());

}

FString UUnitMoveActionComponent::GetActionName() const
{
	return FString("Move");
}

TArray<FGrid> UUnitMoveActionComponent::GetValidActionGridArray() const
{
	TArray<FGrid> validArray;

	FGrid unitGrid = Unit->GetGrid();

	AGridManager* gridManager = AGridManager::GetGridManager();
	
	if (!IsValid(gridManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid Manager not Valid"));
		return validArray;
	}


	for (int x = -MaxActionRange; x <= MaxActionRange; x++)
	{
		for (int y = -MaxActionRange; y <= MaxActionRange; y++)
		{
			FGrid offsetGrid = FGrid(x, y);

			FGrid testGrid = unitGrid + offsetGrid;


			//�������� �ʴ� Grid
			if (!gridManager->IsValidGrid(testGrid))
			{
				continue;
			}
			
			////���� ���� Unit�� ��ġ
			//if (testGrid == unitGrid)
			//{
			//	continue;
			//}

			//������ �������̸� Skip
			if (gridManager->HasAnyUnitOnGrid(testGrid))
			{
				continue;
			}

			//���� �� �ִ� ��ġ?
			if (!gridManager->IsWalkableGrid(testGrid))
			{
				continue;
			}

			//���� ������ ��ġ?
			if (!gridManager->HasPath(unitGrid, testGrid))
			{
				continue;
			}

			//�ǵ��� �޸� �� �Ÿ�?
			if (gridManager->GetPathLength(unitGrid, testGrid) > MaxActionRange)
			{
				continue;
			}

			//����ϸ� ���������� validArray�� �߰�

			validArray.Add(testGrid);
		}
	}


	return validArray;
}

void UUnitMoveActionComponent::TakeAction(FGrid Grid)
{
	AGridManager* gridManager = AGridManager::GetGridManager();
	if (!IsValid(gridManager))
	{
		return;
	}

	int32 pathLength;
	TArray<FGrid> pathArray = gridManager->FindPath(Unit->GetGrid(), Grid, pathLength);

	if (pathLength > MaxActionRange)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Valid MovePosition"));
		return;
	}

	for (int i = 0; i < pathArray.Num(); i++)
	{
		DrawDebugSphere(GetWorld(), gridManager->GridToWorld(pathArray[i]), 10, 12, FColor::Blue, false, 1.5f, 0, 2.0f);
	}


}
