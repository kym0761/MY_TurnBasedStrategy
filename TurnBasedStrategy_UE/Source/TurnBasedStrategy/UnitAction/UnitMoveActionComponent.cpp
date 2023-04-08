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

	//���� �̵��� Ȱ��ȭ�Ǹ� PathArray�� ���� ������ �̵���Ŵ.
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

			//���� ��ġ�� �������� ���߾����.
			if (currentGrid == Destination)
			{
				//a simple temp Move Function.
				AAIController* aiController = Cast<AAIController>(Unit->GetController());
				if (!IsValid(aiController))
				{
					return;
				}

				//Stop�� ���Ŀ� Rotation �� �ٲ� ���� ������ �ذ��ϱ� ���ؼ� �� �ʿ���.
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
	//�̵��� �� �ִ� ����

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


			//�������� �ʴ� Grid
			if (!gameMode->IsValidGrid(resultGrid))
			{
				continue;
			}

			//������ �������̸� Skip
			if (gameMode->HasAnyUnitOnGrid(resultGrid))
			{
				continue;
			}

			//���� �� ���� ��ġ?
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

			//���� ������ ��ġ?
			if (bisFriend && !gameMode->HasPath(unitGrid, resultGrid, true) || !gameMode->HasPath(unitGrid, resultGrid))
			{
				continue;
			}

			//�ǵ��� �޸� �� �Ÿ�?
			if (gameMode->GetPathLength(unitGrid, resultGrid) > MaxActionRange)
			{
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

			//�������� �ʴ� Grid
			if (!gameMode->IsValidGrid(resultGrid))
			{
				continue;
			}

			FGridVisualData testData;
			testData.Grid = resultGrid;
			testData.GridVisualType = EGridVisualType::Move;

			if (gameMode->HasAnyUnitOnGrid(resultGrid) || //������ ������
				!gameMode->IsWalkableGrid(resultGrid) || //���� �� ���� ��ġ?
				!gameMode->HasPath(unitGrid, resultGrid, false) || //���� �Ұ����� ��ġ?
				gameMode->GetPathLength(unitGrid, resultGrid) > MaxActionRange) 	//�ǵ��� �޸� �� �Ÿ�?
			{
				testData.GridVisualType = EGridVisualType::NO;
			}

			bool bisFriend = false;
			auto targetUnit = gameMode->GetUnitAtGrid(resultGrid);
			if (IsValid(targetUnit) && GetOwner()->Tags.Num() > 0)
			{
				bisFriend = targetUnit->ActorHasTag(GetOwner()->Tags[0]);
				if (bisFriend) // ���� �Ʊ� ��ġ��� ��������� ������.
				{
					testData.GridVisualType = EGridVisualType::Warning;
				}
			}
			
			if (targetUnit == GetOwner()) //���� ���� ��ġ�� �̵������� �ɷ� ������.
			{
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
	//���־� ���鿡�� ���� ��� �־ �����ƺ����� �ϴ� ���� ��. �ʿ������ ��Ȱ��ȭ�� ��
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

	TSet<FGrid> grids = GetValidActionGridSet(); //�̵��� �� �ִ� ��ġ ����.
	TArray<FActionValueToken> actionValues;

	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
	if (!IsValid(gameMode))
	{
		//�Ұ�.
		return FGrid(-1, -1);
	}

	//�� �ൿ�� ���� AI�� Unit.
	AUnitCharacter* owner_Casted = Cast<AUnitCharacter>(GetOwner());
	if (!IsValid(owner_Casted))
	{
		//�Ұ�.
		return FGrid(-1, -1);
	}

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

	//owner�� �������� ������ ������ �Ұ���.
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

	//�̵��� ��ġ�� �������� �ִٸ� �ش� ��ġ�� �̵����� ���ƾ���.
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

	//TODO : TargetGrid�� �� ������ �����ؼ� Distance�� ������ -1�� ����.
	//������ �����ϴ� Grid�� ���ؼ�, ���� ���ְ��� �Ÿ� ��� �� ��ġ ���.
	for (auto gridPair : gridObjMap)
	{
		AUnitCharacter* targetUnit = gridPair.Value->GetUnit();

		//Unit�� Invalid�ϸ� ��ŵ.
		if (!IsValid(targetUnit))
		{
			continue;
		}
		
		//Target�� ���� Owner�� ������ ��ŵ.
		if (targetUnit == owner)
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
		gameMode->FindPath(CandidateGrid, targetGrid, distance, false, true);
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

void UUnitMoveActionComponent::TestFunction()
{
	TakeAction(ThinkAIBestActionGrid());
}
