// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitMoveActionComponent.h"
#include "UnitCore/UnitCharacter.h"

#include "Manager/GridManager.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Grid/GridObject.h"

UUnitMoveActionComponent::UUnitMoveActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	MaxActionRange = 5;

	ActionName = FString("Move");
}

void UUnitMoveActionComponent::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void UUnitMoveActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bMoveActivate)
	{
		AGridManager* gridManager = AGridManager::GetGridManager();
		if (!IsValid(gridManager))
		{
			UE_LOG(LogTemp, Warning, TEXT("Grid Manager is not Valid"));
			return;
		}

		if (Path.Num() > 0)
		{
			FVector currentLocation = GetOwner()->GetActorLocation();
			FVector worldLocation = gridManager->GridToWorld(Path[0]) + FVector(0.0f, 0.0f, currentLocation.Z);

			float dist = FVector::Distance(currentLocation, worldLocation);

			//UE_LOG(LogTemp, Warning, TEXT("Dist : %f"), dist);

			if (FMath::IsNearlyEqual(dist, 0.0f, 50.0f))
			{
				Path.RemoveAt(0);
				bIsMoving = false;
				return;
			}
			else if(bIsMoving == false)
			{
				//a simple temp Move Function.
				AAIController* aiController = Cast<AAIController>(Unit->GetController());
				if (!IsValid(aiController))
				{
					return;
				}

				aiController->MoveToLocation(worldLocation, 0.0f, false);
				bIsMoving = true;
				//UE_LOG(LogTemp, Warning, TEXT("Moving"));
				return;
			}

		}
		else
		{
			FVector currentLocation = GetOwner()->GetActorLocation();
			FGrid currentGrid = gridManager->WorldToGrid(currentLocation);

			//현재 위치가 목적지면 멈추어야함.
			if (currentGrid == Destination)
			{
				//a simple temp Move Function.
				AAIController* aiController = Cast<AAIController>(Unit->GetController());
				if (!IsValid(aiController))
				{
					return;
				}

				//Stop은 이후에 Rotation 이 바뀔 때의 문제를 해결하기 위해서 꼭 필요함.
				aiController->StopMovement();

				if (OnActionEnd.IsBound())
				{
					OnActionEnd.Broadcast();
				}
				UE_LOG(LogTemp, Warning, TEXT("Stopped."));
				return;
			}
		}

	}


}

TArray<FGrid> UUnitMoveActionComponent::GetValidActionGridArray() const
{
	TArray<FGrid> validArray;

	FGrid unitGrid = Unit->GetGrid();

	AGridManager* gridManager = AGridManager::GetGridManager();
	
	if (!IsValid(gridManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid Manager is not Valid"));
		return validArray;
	}


	for (int x = -MaxActionRange; x <= MaxActionRange; x++)
	{
		for (int y = -MaxActionRange; y <= MaxActionRange; y++)
		{
			FGrid resultGrid = FGrid(x, y);
			resultGrid += unitGrid;


			//존재하지 않는 Grid
			if (!gridManager->IsValidGrid(resultGrid))
			{
				continue;
			}
			
			//////지금 현재 Unit의 위치
			//if (resultGrid == unitGrid)
			//{
			//	continue;
			//}

			//누군가 점유중이면 Skip
			if (gridManager->HasAnyUnitOnGrid(resultGrid))
			{
				continue;
			}

			//걸을 수 있는 위치?
			if (!gridManager->IsWalkableGrid(resultGrid))
			{
				continue;
			}

			bool bisFriend = false;
			AUnitCharacter* targetUnit = gridManager->GetUnitAtGrid(resultGrid);
			if (IsValid(targetUnit) && GetOwner()->Tags.Num() > 0)
			{
				bisFriend = targetUnit->ActorHasTag(GetOwner()->Tags[0]);
			}

			//도착 가능한 위치?
			if (bisFriend && !gridManager->HasPath(unitGrid, resultGrid, true) || !gridManager->HasPath(unitGrid, resultGrid))
			{
				continue;
			}

			//의도와 달리 먼 거리?
			if (gridManager->GetPathLength(unitGrid, resultGrid) > MaxActionRange)
			{
				continue;
			}

			//통과하면 문제없으니 validArray에 추가

			validArray.Add(resultGrid);
		}
	}


	return validArray;
}

TArray<FGridVisualData> UUnitMoveActionComponent::GetValidActionGridVisualDataArray() const
{
	TArray<FGridVisualData> validVisualDataArray;
	FGrid unitGrid = Unit->GetGrid();

	AGridManager* gridManager = AGridManager::GetGridManager();

	if (!IsValid(gridManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid Manager is not Valid"));
		return validVisualDataArray;
	}


	for (int x = -MaxActionRange; x <= MaxActionRange; x++)
	{
		for (int y = -MaxActionRange; y <= MaxActionRange; y++)
		{
			if (FMath::Abs(x) + FMath::Abs(y) > MaxActionRange)
			{
				continue;
			}

			FGrid resultGrid = FGrid(x, y);
			resultGrid += unitGrid;
			
			//존재하지 않는 Grid
			if (!gridManager->IsValidGrid(resultGrid))
			{
				continue;
			}

			FGridVisualData testData;
			testData.Grid = resultGrid;
			testData.GridVisualType = EGridVisualType::Move;

			if (resultGrid == unitGrid) //지금 현재 Unit의 위치
			{
				testData.GridVisualType = EGridVisualType::Warning;
			}
			else if (gridManager->HasAnyUnitOnGrid(resultGrid) || //누군가 점유중
				!gridManager->IsWalkableGrid(resultGrid) || //걸을 수 없는 위치?
				!gridManager->HasPath(unitGrid, resultGrid, false) || //도착 불가능한 위치?
				gridManager->GetPathLength(unitGrid, resultGrid) > MaxActionRange) 	//의도와 달리 먼 거리?
			{
				testData.GridVisualType = EGridVisualType::NO;
			}

			bool bisFriend = false;
			auto targetUnit = gridManager->GetUnitAtGrid(resultGrid);
			if (IsValid(targetUnit) && GetOwner()->Tags.Num()>0)
			{
				bisFriend = targetUnit->ActorHasTag(GetOwner()->Tags[0]);
				if (bisFriend) // 만약 아군 위치라면 노란색으로 변경함.
				{
					testData.GridVisualType = EGridVisualType::Warning;
				}
			}
			
			if (targetUnit == GetOwner())
			{
				testData.GridVisualType = EGridVisualType::Move;
			}


			//통과하면 문제없으니 validArray에 추가

			validVisualDataArray.Add(testData);
		}
	}


	return validVisualDataArray;
}

void UUnitMoveActionComponent::TakeAction(const FGrid& Grid)
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
		UE_LOG(LogTemp, Warning, TEXT("it's over MaxRange to Move here."));
		return;
	}

	if (pathLength == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't reach to the Target Grid. there is no Path"));
		return;
	}

	if (!pathArray.Contains(Grid))
	{
		UE_LOG(LogTemp, Warning, TEXT("You Can't Move to Invalid Grid"));
		return;
	}


	//~~ Move Debug.

	for (int i = 0; i < pathArray.Num(); i++)
	{
		DrawDebugSphere(GetWorld(), gridManager->GridToWorld(pathArray[i]), 10, 12, FColor::Blue, false, 1.5f, 0, 2.0f);
	}

	FGrid dest = pathArray.Last();

	if (!gridManager->IsValidGrid(dest))
	{
		return;
	}
	
	Destination = dest;
	Path = pathArray;

	if (OnActionStart.IsBound())
	{
		OnActionStart.Broadcast();
	}


}

void UUnitMoveActionComponent::DealWithGridBeforeAction(const FGrid& Grid)
{
	TakeAction(Grid);
}

void UUnitMoveActionComponent::OnActionStartFunc()
{
	Super::OnActionStartFunc();

	bMoveActivate = true;
}

void UUnitMoveActionComponent::OnActionEndFunc()
{
	Super::OnActionEndFunc();

	AGridManager* gridManager = AGridManager::GetGridManager();

	//Update Grid Data
	gridManager->MoveUnitGrid(Unit, Unit->GetGrid(), Destination);
	gridManager->RemoveAllGridVisual();

	bMoveActivate = false;


}

void UUnitMoveActionComponent::OnActionSelectedFunc()
{
	Super::OnActionSelectedFunc();
}

FGrid UUnitMoveActionComponent::ThinkAIBestActionGrid()
{
	TArray<FGrid> grids = GetValidActionGridArray(); //이동할 수 있는 위치 전부.
	TArray<FActionValueToken> actionValues;

	AGridManager* gridManager = AGridManager::GetGridManager();
	if (!IsValid(gridManager))
	{
		//불가.
		return FGrid(-1, -1);
	}

	//이 행동을 취할 AI의 Unit.
	AUnitCharacter* owner_Casted = Cast<AUnitCharacter>(GetOwner());
	if (!IsValid(owner_Casted))
	{
		//불가.
		return FGrid(-1, -1);
	}

	//이동 가능한 위치 전부 확인하여 해당 위치의 Value를 계산.
	for (FGrid& grid : grids)
	{
		FActionValueToken actionValueToken;
		actionValueToken.Grid = grid;
		actionValueToken.ActionValue = CalculateActionValue(grid);

		actionValues.Add(actionValueToken);
	}

	if (actionValues.Num() == 0)
	{
		//확인할 Grid가 없음.
		return FGrid(-1, -1);
	}

	Algo::Sort(actionValues, [](FActionValueToken& a, FActionValueToken& b)
		{
			return a.ActionValue > b.ActionValue;
		});

	//계산된 Value에 대해서, 가장 값이 높은 (가장 상대와 가까울 수 있는) Grid를 선택.
	FActionValueToken selectedActionValueToken = actionValues[0];

	return selectedActionValueToken.Grid;
}

int32 UUnitMoveActionComponent::CalculateActionValue(FGrid& CandidateGrid)
{
	AActor* owner = GetOwner();
	if (!IsValid(owner) || owner->Tags.Num() == 0)
	{
		return -1;
	}

	AGridManager* gridManager = AGridManager::GetGridManager();
	if (!IsValid(gridManager))
	{
		return -1;
	}

	FName teamTag = owner->Tags[0];
	int32 resultDistance = TNumericLimits<int32>::Max();
	TArray<UGridObject*> gridObjectArray = gridManager->GetAllGridObjectThatHasUnit();

	//유닛이 존재하는 Grid에 대해서, 현재 유닛과의 거리 계산 및 가치 계산.
	for (UGridObject* gridObj : gridObjectArray)
	{
		AUnitCharacter* targetUnit = gridObj->GetUnit();

		//해당 위치에 유닛이 없음.
		if (!IsValid(targetUnit))
		{
			continue;
		}
		
		//Target과 현재 Owner와 같으면 스킵.
		if (targetUnit == owner)
		{
			continue;
		}

		// 타겟이 같은 팀이면 스킵
		if (targetUnit->ActorHasTag(teamTag))
		{
			continue;
		}

		FGrid targetGrid = gridObj->GetGrid();

		int32 distance = gridManager->CalculateGridDistance(CandidateGrid, targetGrid);

		resultDistance = (resultDistance > distance) ? distance : resultDistance;
	}

	int32 reverseValueOffset = 10000;

	//상대와 거리가 1이면, 10000-1 = 9999
	//상대와 거리가 5면, 10000-5 = 9995
	//즉, 상대와 거리가 가까울 수록 Value가 크다.
	//만약, 이동거리가 매우 길고 맵이 매우 크다면 10000으로 잡은 값에 문제가 생길 수도 있지만
	//Grid SRPG 특성상 이동거리가 40 이상이며 맵의 크기가 충분히 크다면 랙이 걸리므로 실질적으로 문제가 안될 것이다.
	return reverseValueOffset - resultDistance;
}

void UUnitMoveActionComponent::TestFunction()
{
	TakeAction(ThinkAIBestActionGrid());
}
