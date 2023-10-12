// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitMoveActionComponent.h"
#include "UnitCore/Unit.h"

#include "Manager/GridManager.h"
#include "Manager/SRPG_GameMode.h"

#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Grid/GridObject.h"

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

	GameModeRef = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
}

// Called every frame
void UUnitMoveActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//���� �̵��� Ȱ��ȭ�Ǹ� PathArray�� ���� ������ �̵���Ŵ.
	if (bMoveActivate)
	{
		if (!IsValid(GameModeRef))
		{
			Debug::Print(DEBUG_TEXT("Grid Manager is Invalid."));
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
			FVector worldLocation = GameModeRef->GridToWorld(Path[0]);// +FVector(0.0f, 0.0f, currentLocation.Z);

			float dist = FVector::Distance(currentLocation, worldLocation);
			FVector rotVector = worldLocation - currentLocation;
			rotVector.Z = 0.0f;
			FRotator rot = rotVector.Rotation();

			//UE_LOG(LogTemp, Warning, TEXT("Dist : %f"), dist);

			//���� �������� �� ������ �� ���ٸ�, ���� path�� ����.
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
				FVector movedLocation = FMath::VInterpConstantTo(unit->GetActorLocation(), worldLocation, DeltaTime, 360);
				unit->SetActorLocation(movedLocation);
				FRotator movedRotation = FMath::RInterpConstantTo(unit->GetActorRotation(), rot, DeltaTime, 360);
				unit->SetActorRotation(movedRotation);
				return;
			}
		}
		else
		{
			FVector currentLocation = GetOwner()->GetActorLocation();
			FGrid currentGrid = GameModeRef->WorldToGrid(currentLocation);

			//���� ��ġ�� �������� ���߾����.
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
	//�̵��� �� �ִ� ����
	TSet<FGrid> validSet;

	AUnit* unit = GetOwningUnit();
	if (!IsValid(unit))
	{
		return validSet;
	}

	FGrid unitGrid = unit->GetGrid();

	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
	
	if (!IsValid(gameMode))
	{
		Debug::Print(DEBUG_TEXT("gameMode is Invalid."));
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


			//�������� �ʴ� Grid
			if (!gameMode->IsValidGrid(resultGrid))
			{
				Debug::Print(DEBUG_TEXT("Grid is Invalid. : ") + resultGrid.ToString());
				continue;
			}

			//������ �������̸� Skip
			if (gameMode->HasAnyUnitOnGrid(resultGrid))
			{
				Debug::Print(DEBUG_TEXT("Someone is on this Grid. : ") + resultGrid.ToString());
				continue;
			}

			//���� �� ���� ��ġ?
			if (!gameMode->IsWalkableGrid(resultGrid))
			{
				Debug::Print(DEBUG_TEXT("Can't Walk on this Grid. : ") + resultGrid.ToString());
				continue;
			}

			bool bisFriend = false;
			AUnit* targetUnit = gameMode->GetUnitAtGrid(resultGrid);
			if (IsValid(targetUnit) && GetOwner()->Tags.Num() > 0)
			{
				bisFriend = targetUnit->ActorHasTag(GetOwner()->Tags[0]);
			}

			//���� ������ ��ġ?
			//TODO : ignore �������� �Ȱ������� ����������.
			if (bisFriend && !gameMode->HasPath(unitGrid, resultGrid, MaxActionRange, true) || !gameMode->HasPath(unitGrid, resultGrid, MaxActionRange))
			{
				Debug::Print(DEBUG_TEXT("You Can't Reach to this Grid. : ") + resultGrid.ToString());
				continue;
			}

			//�ǵ��� �޸� �� �Ÿ�?
			if (gameMode->GetPathLength(unitGrid, resultGrid, MaxActionRange) == -1)
			{
				Debug::Print(DEBUG_TEXT("Too Far Grid. : ") + resultGrid.ToString());
				continue;
			}

			//����ϸ� ���������� validSet�� �߰�
			validSet.Add(resultGrid);
		}
	}

	return validSet;
}

TSet<FGridVisualData> UUnitMoveActionComponent::GetValidActionGridVisualDataSet() const
{
	//�̵��� �� �ִ� ��ġ�� Range �����δ� ���������� �����δ� �̵� �Ұ����� ��ġ���� ���� ������.

	TSet<FGridVisualData> validVisualDataSet;

	AUnit* unit = GetOwningUnit();
	if (!IsValid(unit))
	{
		return validVisualDataSet;
	}

	FGrid unitGrid = unit->GetGrid();

	TSet<FGrid> validSet = GetValidActionGridSet();
	 
	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());

	if (!IsValid(gameMode))
	{
		Debug::Print(DEBUG_TEXT("gameMode is Invalid."));
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

			//�������� �ʴ� Grid
			if (!gameMode->IsValidGrid(resultGrid))
			{
				Debug::Print(DEBUG_TEXT("Grid is Invalid."));
				continue;
			}

			FGridVisualData testData;
			testData.Grid = resultGrid;
			testData.GridVisualType = EGridVisualType::Move;

			//if (gameMode->HasAnyUnitOnGrid(grid) || //������ ������
			//	!gameMode->IsWalkableGrid(grid) || //���� �� ���� ��ġ?
			//	!gameMode->HasPath(unitGrid, grid, false) || //���� �Ұ����� ��ġ?
			//	gameMode->GetPathLength(unitGrid, grid) > MaxActionRange) 	//�ǵ��� �޸� �� �Ÿ�?
			if(!validSet.Contains(resultGrid))
			{
				Debug::Print(DEBUG_TEXT("You Can't Select This Grid. : ") + resultGrid.ToString());
				testData.GridVisualType = EGridVisualType::NO;
			}


			bool bisFriend = false;
			auto targetUnit = gameMode->GetUnitAtGrid(resultGrid);
			if (IsValid(targetUnit) && GetOwner()->Tags.Num() > 0)
			{
				bisFriend = targetUnit->ActorHasTag(GetOwner()->Tags[0]);
				if (bisFriend) // ���� �Ʊ� ��ġ��� ��������� ������.
				{
					Debug::Print(DEBUG_TEXT("Your Ally is On This Grid. : ") + resultGrid.ToString());
					testData.GridVisualType = EGridVisualType::Warning;
				}
			}

			if (targetUnit == unit) //���� ���� ��ġ�� �̵������� �ɷ� ������.
			{
				Debug::Print(DEBUG_TEXT("Yourself : ") + resultGrid.ToString());
				testData.GridVisualType = EGridVisualType::Move;
			}

			//����ϸ� ���������� validSet�� �߰�
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
	AUnit* unit = GetOwningUnit();


	TArray<FGrid> pathArray = gameMode->FindPath(unit->GetGrid(), Grid, pathLength, MaxActionRange);

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
	//���־� ���鿡�� ���� ��� �־ �����ƺ����� �ϴ� ���� ��. �ʿ������ ��Ȱ��ȭ�� ��
	for (int i = 0; i < pathArray.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("pathArray[%d] : %s"), i, *pathArray[i].ToString())
		DrawDebugSphere(GetWorld(), gameMode->GridToWorld(pathArray[i]), 10, 12, FColor::Blue, false, 1.5f, 0, 2.0f);
	}

	FGrid dest = pathArray.Last();

	if (!gameMode->IsValidGrid(dest))
	{
		return;
	}
	
	Destination = dest;
	Path = pathArray;
	Prev_Grid = gameMode->WorldToGrid(unit->GetActorLocation());
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
	if (!IsValid(gameMode))
	{
		return;
	}
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
	gameMode->MoveUnitGrid(unit, Prev_Grid, Destination);

	//Remove Visual
	gridManager->RemoveAllGridVisual();

	bMoveActivate = false;

	//Super::ActionEnd()�� AI�� �����ൿ�� �϶�� ����� �������ٵ�,
	//gameMode�� �� ������ �ݿ��� ���Ŀ� Call�Ǿ���.
	Super::ActionEnd();
}

void UUnitMoveActionComponent::ActionSelected()
{
	Super::ActionSelected();
}

FGrid UUnitMoveActionComponent::ThinkAIBestActionGrid()
{
	//��� ��ġ�� �̵��ϱ� ���� �������� �����.

	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
	if (!IsValid(gameMode))
	{
		//�Ұ�.
		return FGrid(-1, -1);
	}

	//�� �ൿ�� ���� AI�� Unit.
	AUnit* unit = GetOwningUnit();
	if (!IsValid(unit))
	{
		//�Ұ�.
		return FGrid(-1, -1);
	}

	TSet<FGrid> grids = GetValidActionGridSet(); //�̵��� �� �ִ� ��ġ ����.
	TArray<FActionValueToken> actionValues;

	//�̵� ������ ��ġ ���� Ȯ���Ͽ� �ش� ��ġ�� Value�� ���.
	for (FGrid& grid : grids)
	{
		FActionValueToken actionValueToken;
		actionValueToken.Grid = grid;
		actionValueToken.ActionValue = CalculateActionValue(grid);

		actionValues.Add(actionValueToken);
	}

	if (actionValues.Num() == 0)
	{
		//Ȯ���� Grid�� ����.
		return FGrid(-1, -1);
	}

	Algo::Sort(actionValues, [](FActionValueToken& a, FActionValueToken& b)
		{
			return a.ActionValue > b.ActionValue;
		});

	//���� Value�� ���ؼ�, ���� ���� ���� (���� ���� ����� �� �ִ�) Grid�� ����.
	FActionValueToken selectedActionValueToken = actionValues[0];

	return selectedActionValueToken.Grid;
}

int32 UUnitMoveActionComponent::CalculateActionValue(FGrid& CandidateGrid)
{
	//unit�� �������� ������ ������ �Ұ���.
	AUnit* unit = GetOwningUnit();
	if (!IsValid(unit) || unit->Tags.Num() == 0)
	{
		return -1;
	}

	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
	if (!IsValid(gameMode))
	{
		return -1;
	}

	//�̵��� ��ġ�� �������� �ִٸ� �ش� ��ġ�� �̵����� ���ƾ���.
	if (gameMode->HasAnyUnitOnGrid(CandidateGrid))
	{
		return -10000;
	}

	FName teamTag = unit->Tags[0];
	int32 distanceToTarget = TNumericLimits<int32>::Max();
	TMap<FGrid, UGridObject*> gridObjMap = gameMode->GetAllGridObjectsThatHasUnit();


	FGrid unitGrid = unit->GetGrid();
	//TODO : TargetGrid�� �� ������ �����ؼ� Distance�� ������ -1�� ����.
	//������ �����ϴ� Grid�� ���ؼ�, ���� ���ְ��� �Ÿ� ��� �� ��ġ ���.
	for (auto gridPair : gridObjMap)
	{
		AUnit* targetUnit = gridPair.Value->GetUnit();

		//Unit�� Invalid�ϸ� ��ŵ.
		if (!IsValid(targetUnit))
		{
			continue;
		}
		
		//Target�� ���� unit�� ������ ��ŵ.
		if (targetUnit == unit)
		{
			continue;
		}

		// Ÿ���� ���� ���̸� ��ŵ
		if (targetUnit->ActorHasTag(teamTag))
		{
			continue;
		}

		//distanceToTarget : Target ���ְ��� �Ÿ�
		//target���� �Ÿ��� �������� ������
		//�̵��ϴµ� �ʿ��� �Ÿ��� �������� ������.
		FGrid targetGrid = gridPair.Key;
		int32 distance;
		gameMode->FindPath(CandidateGrid, targetGrid, distance, MaxActionRange, false, true);
		distanceToTarget = (distanceToTarget > distance && distance != -1) ? distance : distanceToTarget;
	}

	int32 reverseValueOffset = 10000;

	//���� �Ÿ��� 1�̸�, 10000-1 = 9999
	//���� �Ÿ��� 5��, 10000-5 = 9995
	//��, ���� �Ÿ��� ����� ���� Value�� ũ��.
	//����, �̵��Ÿ��� �ſ� ��� ���� �ſ� ũ�ٸ� 10000���� ���� ���� ������ ���� ���� ������
	//Grid SRPG Ư���� �̵��Ÿ��� 40 �̻� �����̷��ϰų� ���� ũ�Ⱑ ����� ũ�ٸ� ���� �ɸ��Ƿ� ���������� ������ �ȵ� ���̴�.
	return reverseValueOffset - distanceToTarget;
}

void UUnitMoveActionComponent::AI_Action()
{
	TakeAction(ThinkAIBestActionGrid());
}
