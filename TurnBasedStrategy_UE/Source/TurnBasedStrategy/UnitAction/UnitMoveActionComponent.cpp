// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitMoveActionComponent.h"
#include "UnitCore/UnitCharacter.h"

#include "Manager/GridManager.h"
#include "Manager/SRPG_GameMode.h"

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

	//유닛 이동이 활성화되면 PathArray에 맞춰 유닛을 이동시킴.
	if (bMoveActivate)
	{
		ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
		if (!IsValid(gameMode))
		{
			UE_LOG(LogTemp, Warning, TEXT("Grid Manager is not Valid"));
			return;
		}

		if (Path.Num() > 0)
		{
			FVector currentLocation = GetOwner()->GetActorLocation();
			FVector worldLocation = gameMode->GridToWorld(Path[0]) + FVector(0.0f, 0.0f, currentLocation.Z);

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
			FGrid currentGrid = gameMode->WorldToGrid(currentLocation);

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

				ActionEnd();
				UE_LOG(LogTemp, Warning, TEXT("Stopped."));
				return;
			}
		}

	}


}

TSet<FGrid> UUnitMoveActionComponent::GetValidActionGridSet() const
{
	//이동할 수 있는 범위

	TSet<FGrid> validSet;

	FGrid unitGrid = Unit->GetGrid();

	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
	
	if (!IsValid(gameMode))
	{
		UE_LOG(LogTemp, Warning, TEXT("gameMode is not Valid"));
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

			FGrid resultGrid = FGrid(x, y);
			resultGrid += unitGrid;


			//존재하지 않는 Grid
			if (!gameMode->IsValidGrid(resultGrid))
			{
				continue;
			}

			//누군가 점유중이면 Skip
			if (gameMode->HasAnyUnitOnGrid(resultGrid))
			{
				continue;
			}

			//걸을 수 없는 위치?
			if (!gameMode->IsWalkableGrid(resultGrid))
			{
				continue;
			}

			bool bisFriend = false;
			AUnitCharacter* targetUnit = gameMode->GetUnitAtGrid(resultGrid);
			if (IsValid(targetUnit) && GetOwner()->Tags.Num() > 0)
			{
				bisFriend = targetUnit->ActorHasTag(GetOwner()->Tags[0]);
			}

			//도착 가능한 위치?
			if (bisFriend && !gameMode->HasPath(unitGrid, resultGrid, true) || !gameMode->HasPath(unitGrid, resultGrid))
			{
				continue;
			}

			//의도와 달리 먼 거리?
			if (gameMode->GetPathLength(unitGrid, resultGrid) > MaxActionRange)
			{
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
	FGrid unitGrid = Unit->GetGrid();

	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());

	if (!IsValid(gameMode))
	{
		UE_LOG(LogTemp, Warning, TEXT("gameMode is not Valid"));
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

			FGrid resultGrid = FGrid(x, y);
			resultGrid += unitGrid;

			//존재하지 않는 Grid
			if (!gameMode->IsValidGrid(resultGrid))
			{
				continue;
			}

			FGridVisualData testData;
			testData.Grid = resultGrid;
			testData.GridVisualType = EGridVisualType::Move;

			if (gameMode->HasAnyUnitOnGrid(resultGrid) || //누군가 점유중
				!gameMode->IsWalkableGrid(resultGrid) || //걸을 수 없는 위치?
				!gameMode->HasPath(unitGrid, resultGrid, false) || //도착 불가능한 위치?
				gameMode->GetPathLength(unitGrid, resultGrid) > MaxActionRange) 	//의도와 달리 먼 거리?
			{
				testData.GridVisualType = EGridVisualType::NO;
			}

			bool bisFriend = false;
			auto targetUnit = gameMode->GetUnitAtGrid(resultGrid);
			if (IsValid(targetUnit) && GetOwner()->Tags.Num() > 0)
			{
				bisFriend = targetUnit->ActorHasTag(GetOwner()->Tags[0]);
				if (bisFriend) // 만약 아군 위치라면 노란색으로 변경함.
				{
					testData.GridVisualType = EGridVisualType::Warning;
				}
			}
			
			if (targetUnit == GetOwner()) //현재 유닛 위치는 이동가능한 걸로 판정함.
			{
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
	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
	if (!IsValid(gameMode))
	{
		return;
	}

	int32 pathLength;
	TArray<FGrid> pathArray = gameMode->FindPath(Unit->GetGrid(), Grid, pathLength);

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


	// ! Move Debuging Sphere
	//비주얼 측면에서 현재 계속 있어도 괜찮아보여서 일단 유지 중. 필요없으면 비활성화할 것
	for (int i = 0; i < pathArray.Num(); i++)
	{
		DrawDebugSphere(GetWorld(), gameMode->GridToWorld(pathArray[i]), 10, 12, FColor::Blue, false, 1.5f, 0, 2.0f);
	}

	FGrid dest = pathArray.Last();

	if (!gameMode->IsValidGrid(dest))
	{
		return;
	}
	
	Destination = dest;
	Path = pathArray;

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
	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
	AGridManager* gridManager = AGridManager::GetGridManager();

	//Update Grid Data
	gameMode->MoveUnitGrid(Unit, Unit->GetGrid(), Destination);

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

	TSet<FGrid> grids = GetValidActionGridSet(); //이동할 수 있는 위치 전부.
	TArray<FActionValueToken> actionValues;

	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
	if (!IsValid(gameMode))
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

	//owner가 적절하지 않으면 밸류계산 불가능.
	AActor* owner = GetOwner();
	if (!IsValid(owner) || owner->Tags.Num() == 0)
	{
		return -1;
	}

	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
	if (!IsValid(gameMode))
	{
		return -1;
	}

	//이동할 위치에 누군가가 있다면 해당 위치로 이동하지 말아야함.
	if (gameMode->HasAnyUnitOnGrid(CandidateGrid))
	{
		return -10000;
	}

	FName teamTag = owner->Tags[0];
	int32 distanceToTarget = TNumericLimits<int32>::Max();
	TMap<FGrid, UGridObject*> gridObjMap = gameMode->GetAllGridObjectsThatHasUnit();

	auto owner_cast = Cast<AUnitCharacter>(owner);
	FGrid ownerGrid;
	if (IsValid(owner_cast))
	{
		ownerGrid = owner_cast->GetGrid();
	}

	//TODO : TargetGrid에 적 유닛이 존재해서 Distance가 무조건 -1이 나옴.
	//유닛이 존재하는 Grid에 대해서, 현재 유닛과의 거리 계산 및 가치 계산.
	for (auto gridPair : gridObjMap)
	{
		AUnitCharacter* targetUnit = gridPair.Value->GetUnit();

		//Unit이 Invalid하면 스킵.
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

		//distanceToTarget : Target 유닛과의 거리
		//target과의 거리가 가까울수록 선택함
		//이동하는데 필요한 거리가 가까울수록 선택함.
		FGrid targetGrid = gridPair.Key;
		int32 distance;
		gameMode->FindPath(CandidateGrid, targetGrid, distance, false, true);
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

void UUnitMoveActionComponent::TestFunction()
{
	TakeAction(ThinkAIBestActionGrid());
}
