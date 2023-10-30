// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitMoveActionComponent.h"

#include "UnitCore/Unit.h"
#include "Manager/GridManager.h"
#include "Grid/GridObject.h"

#include "Kismet/GameplayStatics.h"
#include "AIController.h"

#include "DebugHelper.h"

UUnitMoveActionComponent::UUnitMoveActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	MaxActionRange = 5;

	ActionName = FString("Move");
}

void UUnitMoveActionComponent::BeginPlay()
{
	Super::BeginPlay();

	//GameModeRef = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
}

// Called every frame
void UUnitMoveActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//유닛 이동이 활성화되면 PathArray에 맞춰 유닛을 이동시킴.
	if (bMoveActivate)
	{
		AGridManager* gridManager = AGridManager::GetGridManager();

		if (!IsValid(gridManager))
		{
			Debug::Print(DEBUG_TEXT("gridManager is Invalid."));
			return;
		}

		AUnit* unit = GetOwningUnit();
		if (!IsValid(unit))
		{
			Debug::Print(DEBUG_TEXT("unit is Invalid."));
			return;
		}

		if (Path.Num() > 0)
		{
			FVector currentLocation = unit->GetActorLocation();
			FVector worldLocation = gridManager->GridToWorld(Path[0]);// +FVector(0.0f, 0.0f, currentLocation.Z);

			float dist = FVector::Distance(currentLocation, worldLocation);
			FVector rotVector = worldLocation - currentLocation;
			rotVector.Z = 0.0f;
			FRotator rot = rotVector.Rotation();

			//UE_LOG(LogTemp, Warning, TEXT("Dist : %f"), dist);

			//현재 지점으로 다 움직인 것 같다면, 다음 path로 변경.
			if (FMath::IsNearlyEqual(dist, 0.0f,0.01f))
			{
				Path.RemoveAt(0);
				if (!(Path.Num() > 0))
				{
					return;
				}
			}
			else
			{
				FVector movedLocation = FMath::VInterpConstantTo(unit->GetActorLocation(), worldLocation, DeltaTime, MovementSpeed);
				unit->SetActorLocation(movedLocation);
				FRotator movedRotation = FMath::RInterpConstantTo(unit->GetActorRotation(), rot, DeltaTime, RotationSpeed);
				unit->SetActorRotation(movedRotation);
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
				ActionEnd();
				Debug::Print(DEBUG_TEXT("Unit Stopped."));
				return;
			}
			else
			{
				Debug::Print(DEBUG_TEXT("Error. Didn't Reach to Destination."));
			}
		}
	}


}

TSet<FGrid> UUnitMoveActionComponent::GetValidActionGridSet() const
{
	//이동할 수 있는 범위
	TSet<FGrid> validSet;

	AUnit* unit = GetOwningUnit();
	if (!IsValid(unit))
	{
		return validSet;
	}

	FGrid unitGrid = unit->GetGrid();


	AGridManager* gridManager = AGridManager::GetGridManager();

	if (!IsValid(gridManager))
	{
		Debug::Print(DEBUG_TEXT("gridManager is Invalid."));
		return validSet;
	}


	for (int x = -MaxActionRange; x <= MaxActionRange; x++)
	{
		for (int y = -MaxActionRange; y <= MaxActionRange; y++)
		{
			if (FMath::Abs(x) + FMath::Abs(y) > MaxActionRange)
			{
				continue;
			}

			FGrid resultGrid = unitGrid + FGrid(x, y);

			//존재하지 않는 Grid
			if (!gridManager->IsValidGrid(resultGrid))
			{
				Debug::Print(DEBUG_TEXT("Grid is Invalid. : ") + resultGrid.ToString());
				continue;
			}

			//누군가 점유중이면 Skip
			if (gridManager->HasAnyUnitOnGrid(resultGrid))
			{
				Debug::Print(DEBUG_TEXT("Someone is on this Grid. : ") + resultGrid.ToString());
				continue;
			}

			//걸을 수 없는 위치?
			if (!gridManager->IsWalkableGrid(resultGrid))
			{
				Debug::Print(DEBUG_TEXT("Can't Walk on this Grid. : ") + resultGrid.ToString());
				continue;
			}

			bool bisFriend = false;
			AUnit* targetUnit = gridManager->GetUnitAtGrid(resultGrid);
			if (IsValid(targetUnit) && unit->GetTeamType() == targetUnit->GetTeamType())
			{
				bisFriend = true;
			}

			//도착 가능한 위치?
			//TODO : ignore 가능한지 안가능한지 따져봐야함.
			if (bisFriend && !gridManager->HasPath(unitGrid, resultGrid, MaxActionRange, true) || !gridManager->HasPath(unitGrid, resultGrid, MaxActionRange))
			{
				Debug::Print(DEBUG_TEXT("You Can't Reach to this Grid. : ") + resultGrid.ToString());
				continue;
			}

			//의도와 달리 먼 거리?
			if (gridManager->GetPathLength(unitGrid, resultGrid, MaxActionRange) == -1)
			{
				Debug::Print(DEBUG_TEXT("Too Far Grid. : ") + resultGrid.ToString());
				continue;
			}

			//통과하면 문제없으니 validSet에 추가
			validSet.Add(resultGrid);
		}
	}

	return validSet;
}

TSet<FGridVisualData> UUnitMoveActionComponent::GetValidActionGridVisualDataSet() const
{
	//이동할 수 있는 위치와 Range 상으로는 가능하지만 실제로는 이동 불가능한 위치들을 전부 도출함.

	TSet<FGridVisualData> validVisualDataSet;

	AUnit* unit = GetOwningUnit();
	if (!IsValid(unit))
	{
		return validVisualDataSet;
	}

	FGrid unitGrid = unit->GetGrid();

	TSet<FGrid> validSet = GetValidActionGridSet();
	 
	AGridManager* gridManager = AGridManager::GetGridManager();

	if (!IsValid(gridManager))
	{
		Debug::Print(DEBUG_TEXT("gridManager is Invalid."));
		return validVisualDataSet;
	}

	for (int x = -MaxActionRange; x <= MaxActionRange; x++)
	{
		for (int y = -MaxActionRange; y <= MaxActionRange; y++)
		{
			if (FMath::Abs(x) + FMath::Abs(y) > MaxActionRange)
			{
				continue;
			}

			FGrid resultGrid = unitGrid + FGrid(x, y);

			//존재하지 않는 Grid
			if (!gridManager->IsValidGrid(resultGrid))
			{
				Debug::Print(DEBUG_TEXT("Grid is Invalid."));
				continue;
			}

			FGridVisualData testData;
			testData.Grid = resultGrid;
			testData.GridVisualType = EGridVisualType::Move;

			//if (gameMode->HasAnyUnitOnGrid(grid) || //누군가 점유중
			//	!gameMode->IsWalkableGrid(grid) || //걸을 수 없는 위치?
			//	!gameMode->HasPath(unitGrid, grid, false) || //도착 불가능한 위치?
			//	gameMode->GetPathLength(unitGrid, grid) > MaxActionRange) 	//의도와 달리 먼 거리?
			if(!validSet.Contains(resultGrid))
			{
				Debug::Print(DEBUG_TEXT("You Can't Select This Grid. : ") + resultGrid.ToString());
				testData.GridVisualType = EGridVisualType::NO;
			}

			auto targetUnit = gridManager->GetUnitAtGrid(resultGrid);
			if (IsValid(targetUnit) && unit->GetTeamType() == targetUnit->GetTeamType())
			{
				Debug::Print(DEBUG_TEXT("Your Ally is On This Grid. : ") + resultGrid.ToString());
				testData.GridVisualType = EGridVisualType::Warning;
			}

			if (targetUnit == unit) //현재 유닛 위치는 이동가능한 걸로 판정함.
			{
				Debug::Print(DEBUG_TEXT("Yourself : ") + resultGrid.ToString());
				testData.GridVisualType = EGridVisualType::Move;
			}

			//통과하면 문제없으니 validSet에 추가
			validVisualDataSet.Add(testData);

		}
	}

	return validVisualDataSet;
}

void UUnitMoveActionComponent::TakeAction(const FGrid& Grid)
{
	AGridManager* gridManager = AGridManager::GetGridManager();

	if (!IsValid(gridManager))
	{
		return;
	}

	int32 pathLength;
	AUnit* unit = GetOwningUnit();


	TArray<FGrid> pathArray = gridManager->FindPath(unit->GetGrid(), Grid, pathLength, MaxActionRange);

	if (pathLength > MaxActionRange)
	{
		Debug::Print(DEBUG_TEXT("it's over MaxRange to Move here. : ") + Grid.ToString());
		return;
	}

	if (pathLength == -1)
	{
		Debug::Print(DEBUG_TEXT("Can't reach to the Target Grid. there is no Path. : ") + Grid.ToString());
		return;
	}

	if (!pathArray.Contains(Grid))
	{
		Debug::Print(DEBUG_TEXT("You Can't Move to the Invalid Grid. : ") + Grid.ToString());
		return;
	}


	// ! Move Debuging Sphere
	//비주얼 측면에서 현재 계속 있어도 괜찮아보여서 일단 유지 중. 필요없으면 비활성화할 것
	for (int i = 0; i < pathArray.Num(); i++)
	{
		Debug::Print(TEXT("pathArray[%d] : %s") + pathArray[i].ToString());
		DrawDebugSphere(GetWorld(), gridManager->GridToWorld(pathArray[i]), 10, 12, FColor::Blue, false, 1.5f, 0, 2.0f);
	}

	FGrid dest = pathArray.Last();

	if (!gridManager->IsValidGrid(dest))
	{
		return;
	}
	
	Destination = dest;
	Path = pathArray;
	Prev_Grid = gridManager->WorldToGrid(unit->GetActorLocation());
	ActionStart();
}

void UUnitMoveActionComponent::DealWithGridBeforeAction(const FGrid& Grid)
{
	TakeAction(Grid);
}

void UUnitMoveActionComponent::ActionStart()
{
	Super::ActionStart();

	bMoveActivate = true;
}

void UUnitMoveActionComponent::ActionEnd()
{
	AGridManager* gridManager = AGridManager::GetGridManager();
	if (!IsValid(gridManager))
	{
		return;
	}
	AUnit* unit= GetOwningUnit();
	if (!IsValid(unit))
	{
		return;
	}

	//Update Grid Data
	gridManager->MoveUnitGrid(unit, Prev_Grid, Destination);

	//Remove Visual
	gridManager->RemoveAllGridVisual();

	bMoveActivate = false;

	//Super::ActionEnd()에 AI가 다음행동을 하라는 명령을 실행할텐데,
	//gameMode의 값 변경이 반영된 이후에 Call되야함.
	Super::ActionEnd();
}

void UUnitMoveActionComponent::ActionSelected()
{
	Super::ActionSelected();
}

FGrid UUnitMoveActionComponent::ThinkAIBestActionGrid()
{
	//어느 위치가 이동하기 가장 적절한지 계산함.
	AGridManager* gridManager = AGridManager::GetGridManager();
	if (!IsValid(gridManager))
	{
		//불가.
		return FGrid(-1, -1);
	}

	//이 행동을 취할 AI의 Unit.
	AUnit* unit = GetOwningUnit();
	if (!IsValid(unit))
	{
		//불가.
		return FGrid(-1, -1);
	}

	TSet<FGrid> grids = GetValidActionGridSet(); //이동할 수 있는 위치 전부.
	TArray<FActionValueToken> actionValues;

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
	//개선이 필요할 수도?
	//길막 위치로 이동하려고 함.
	//적이 충분히 적과 근접한 상황인데도 다른 유닛을 찾거나 먼 거리를 가려고 함.

	//unit이 적절하지 않으면 밸류계산 불가능.
	AUnit* unit = GetOwningUnit();
	if (!IsValid(unit))
	{
		return -1;
	}

	AGridManager* gridManager = AGridManager::GetGridManager();
	if (!IsValid(gridManager))
	{
		return -1;
	}

	//이동할 위치에 누군가가 있다면 해당 위치로 이동하지 말아야함.
	if (gridManager->HasAnyUnitOnGrid(CandidateGrid))
	{
		return -10000;
	}

	int32 distanceToTarget = TNumericLimits<int32>::Max();
	TMap<FGrid, UGridObject*> gridObjMap = gridManager->GetAllGridObjectsThatHasUnit();


	FGrid unitGrid = unit->GetGrid();
	//TODO : TargetGrid에 적 유닛이 존재해서 Distance가 무조건 -1이 나옴.
	//유닛이 존재하는 Grid에 대해서, 현재 유닛과의 거리 계산 및 가치 계산.
	for (auto gridPair : gridObjMap)
	{
		AUnit* targetUnit = gridPair.Value->GetUnit();

		//Unit이 Invalid하면 스킵.
		if (!IsValid(targetUnit))
		{
			continue;
		}
		
		//Target과 현재 unit과 같으면 스킵.
		if (targetUnit == unit)
		{
			continue;
		}

		// 타겟이 같은 팀이면 스킵
		if (targetUnit->GetTeamType() == unit->GetTeamType())
		{
			continue;
		}

		//distanceToTarget : Target 유닛과의 거리
		//target과의 거리가 가까울수록 선택함
		//이동하는데 필요한 거리가 가까울수록 선택함.
		FGrid targetGrid = gridPair.Key;
		int32 distance;
		gridManager->FindPath(CandidateGrid, targetGrid, distance, MaxActionRange, false, true);
		distanceToTarget = (distanceToTarget > distance && distance != -1) ? distance : distanceToTarget;
	}

	int32 reverseValueOffset = 10000;

	//상대와 거리가 1이면, 10000-1 = 9999
	//상대와 거리가 5면, 10000-5 = 9995
	//즉, 상대와 거리가 가까울 수록 Value가 크다.
	//만약, 이동거리가 매우 길고 맵이 매우 크다면 10000으로 잡은 값에 문제가 생길 수도 있지만
	//Grid SRPG 특성상 이동거리가 40 이상 움직이려하거나 맵의 크기가 충분히 크다면 랙이 걸리므로 실질적으로 문제가 안될 것이다.
	return reverseValueOffset - distanceToTarget;
}

void UUnitMoveActionComponent::AI_Action()
{
	TakeAction(ThinkAIBestActionGrid());
}
