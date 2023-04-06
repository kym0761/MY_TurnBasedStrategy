// Fill out your copyright notice in the Description page of Project Settings.


#include "GridManager.h"
#include "Grid/GridObject.h"
#include "Grid/PathNode.h"
#include "Grid/InstancedGridVisualComponent.h"
#include "Grid/PathFindingSystem.h"
#include "Grid/GridSystem.h"

#include "Components/BillboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UnitCore/UnitCharacter.h"

#include "UnitAction/UnitAttackActionComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AGridManager::AGridManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bRunConstructionScriptOnDrag = false;

	X_Length = 10;
	Y_Length = 10;
	CellSize = 100.0f;

	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("BillBoardComponent"));
	SetRootComponent(BillboardComponent);

	GridVisual_Move = CreateDefaultSubobject<UInstancedGridVisualComponent>(TEXT("GridVisual_Move"));
	GridVisual_Move->SetupAttachment(GetRootComponent());

	GridVisual_OK = CreateDefaultSubobject<UInstancedGridVisualComponent>(TEXT("GridVisual_OK"));
	GridVisual_OK->SetupAttachment(GetRootComponent());

	GridVisual_NO = CreateDefaultSubobject<UInstancedGridVisualComponent>(TEXT("GridVisual_NO"));
	GridVisual_NO->SetupAttachment(GetRootComponent());

	GridVisual_Warning = CreateDefaultSubobject<UInstancedGridVisualComponent>(TEXT("GridVisual_Warning"));
	GridVisual_Warning->SetupAttachment(GetRootComponent());

	GridVisual_DANGER = CreateDefaultSubobject<UInstancedGridVisualComponent>(TEXT("GridVisual_DANGER"));
	GridVisual_DANGER->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AGridManager::BeginPlay()
{
	Super::BeginPlay();

	CreateGridSystem();

}

//void AGridManager::OnConstruction(const FTransform& Transform)
//{
//	Super::OnConstruction(Transform);
//
//
//
//}

void AGridManager::CreateGridSystem()
{
	if (IsValid(GridSystem))
	{
		GridSystem = nullptr;
	}
	GridSystem = NewObject<UGridSystem>();

	GridSystem->SetGridSystem(
		X_Length,
		Y_Length,
		[](UGridSystem* gs, FGrid grid) {
			UGridObject* gridObj = NewObject<UGridObject>();
			gridObj->SetGrid(grid);
			gridObj->SetGridSystem(gs);
			return gridObj;
		});

	if (IsValid(PathFindingSystem))
	{
		PathFindingSystem = nullptr;
	}

	PathFindingSystem = NewObject<UPathFindingSystem>();

	PathFindingSystem->SetPathFindingSystem(
		X_Length,
		Y_Length,
		[](UPathFindingSystem* pfs, FGrid grid)
		{
			UPathNode* pathNode = NewObject<UPathNode>();
			pathNode->SetGrid(grid);
			return pathNode;
		}
	);

	//Grid �ʿ� ��ֹ� ����. ��� �Ұ�.
	auto map = PathFindingSystem->GetPathNodeMap();
	for (int32 i = 0; i < X_Length; i++)
	{
		for (int32 j = 0; j < Y_Length; j++)
		{
			FGrid grid(i,j);
			FVector pos = GridToWorld(grid);

			FVector startPos = pos + FVector(0.0f, 0.0f, 5000.0f);
			FVector endPos = FVector(startPos.X, startPos.Y, -5000.0f);

			TArray<TEnumAsByte<EObjectTypeQuery>> objects;
			
			objects.Add(UEngineTypes::ConvertToObjectType(
				ECollisionChannel::ECC_GameTraceChannel2)); // ObjectType : Obstacle.. DefaultEngine.ini ����
			
			TArray<AActor*> ignores;
			TArray<FHitResult> outHits;

			UKismetSystemLibrary::LineTraceMultiForObjects(
				GetWorld(),
				startPos,
				endPos,
				objects,
				true,
				ignores,
				//EDrawDebugTrace::None,
				EDrawDebugTrace::ForDuration,
				outHits,
				true,
				FLinearColor::Red,
				FLinearColor::Blue,
				5.0f
			);

			//UE_LOG(LogTemp, Warning, TEXT(" Grid(%d : %d) Trace Num : %d"), i, j, outHits.Num());

			//outhits�� 1�� �̻��� ���� ������ �ִٸ�, �ش� ��ġ�� ��ֹ��� �����ϰ� �ִ� ����.
			if (outHits.Num() > 0)
			{
				UPathNode* pathNode = map[grid];
				if (IsValid(pathNode))
				{
					pathNode->SetIsWalkable(false);
				}
			}

		}
	}

}

// Called every frame
void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AGridManager::IsValidGrid(const FGrid& Grid) const
{
	return (Grid.X >= 0) && (Grid.Y >= 0) && (Grid.X < X_Length) && (Grid.Y < Y_Length);
}

void AGridManager::RemoveAllGridVisual()
{
	GridVisual_Move->RemoveGridVisuals();
	GridVisual_OK->RemoveGridVisuals();
	GridVisual_NO->RemoveGridVisuals();
	GridVisual_Warning->RemoveGridVisuals();
}

FGrid AGridManager::WorldToGrid(const FVector& WorldPosition) const
{
	FGrid grid;
	FVector managerLocation = GetActorLocation();

	grid.X = FMath::RoundToInt32((WorldPosition.X- managerLocation.X) / CellSize);
	grid.Y = FMath::RoundToInt32((WorldPosition.Y- managerLocation.Y) / CellSize);

	//UE_LOG(LogTemp, Warning, TEXT("Grid Pos : %s"), *grid.ToString());

	return grid;
}

FVector AGridManager::GridToWorld(const FGrid& Grid) const
{
	FVector worldPosition = GetActorLocation();
	worldPosition.X += Grid.X * CellSize;
	worldPosition.Y += Grid.Y * CellSize;

	//UE_LOG(LogTemp, Warning, TEXT("World Pos : %s"),*worldPosition.ToString());

	return worldPosition;
}

UGridObject* AGridManager::GetValidGridObject(const FGrid& Grid) const
{
	UGridObject* gridObj = GridSystem->GetValidGridObject(Grid);

	if (!IsValid(gridObj))
	{
		return nullptr;
	}

	return gridObj;
}

void AGridManager::ShowGridRange(const FGrid& Grid, int32 Range, EGridVisualType GridVisualType)
{
	//Grid �� �������� Range��ŭ Show����.

	TSet<FGrid> gridSet;

	for (int32 x = -Range; x <= Range; x++)
	{
		for (int32 y = -Range; y <= Range; y++)
		{
			FGrid temp = Grid + FGrid(x, y);

			if (!IsValidGrid(temp))
			{
				continue;
			}

			int32 distance = FMath::Abs(x) + FMath::Abs(y);
			if (distance > Range)
			{
				continue;
			}

			gridSet.Add(temp);
		}
	}

	ShowFromGridSet(gridSet, GridVisualType);
}

void AGridManager::ShowFromGridSet(const TSet<FGrid>& GridSet, EGridVisualType GridVisualType, const float Height)
{
	UInstancedGridVisualComponent* toDraw = nullptr;

	switch (GridVisualType)
	{
	case EGridVisualType::Move:
		toDraw = GridVisual_Move;
		break;
	case EGridVisualType::NO:
		toDraw = GridVisual_NO;
		break;
	case EGridVisualType::OK:
		toDraw = GridVisual_OK;
		break;
	case EGridVisualType::Warning:
		toDraw = GridVisual_Warning;
		break;
	case EGridVisualType::DANGER:
		toDraw = GridVisual_DANGER;
		break;
	default:
		break;
	}

	if (IsValid(toDraw))
	{
		toDraw->DrawGridVisualswithGridSet(GridSet,Height);
	}

}

void AGridManager::ShowFromGridVisualDataSet(const TSet<FGridVisualData>& GridVisualDataSet, const float Height)
{
	TSet<FGrid> moveArr;
	TSet<FGrid> noArr;
	TSet<FGrid> okArr;
	TSet<FGrid> warningArr;

	for (const FGridVisualData& visualData : GridVisualDataSet)
	{
		switch (visualData.GridVisualType)
		{
		case EGridVisualType::Move:
			moveArr.Add(visualData.Grid);
			break;
		case EGridVisualType::NO:
			noArr.Add(visualData.Grid);
			break;
		case EGridVisualType::OK:
			okArr.Add(visualData.Grid);
			break;
		case EGridVisualType::Warning:
			warningArr.Add(visualData.Grid);
			break;
		default:
			break;
		}
	}

	if (IsValid(GridVisual_Move))
	{
		GridVisual_Move->DrawGridVisualswithGridSet(moveArr);
	}

	if (IsValid(GridVisual_NO))
	{
		GridVisual_NO->DrawGridVisualswithGridSet(noArr);
	}

	if (IsValid(GridVisual_OK))
	{
		GridVisual_OK->DrawGridVisualswithGridSet(okArr);
	}

	if (IsValid(GridVisual_Warning))
	{
		GridVisual_Warning->DrawGridVisualswithGridSet(warningArr);
	}
}

TArray<FGrid> AGridManager::FindPath(const FGrid& Start, const FGrid& End, int32& PathLength, bool bCanIgnoreUnit)
{
	//!����! return �ϱ� ���� PathLength�� ������Ѿ���.
	
	//PathLength�� ���� ������, ��ֹ��̳� �ٸ� ��Ÿ ������ �� ���Ƽ� ������ ��
	//�̵��¿� ���� ���� �� �ִ� ��ġ���� Ȯ���ؾ���.
	//����) �̵����� 5�� ������ �ش� ��ġ���� ���� PathLength�� 5 ������ ���� �ش� ��ġ�� �� �� ����.

	//openList = �̵� ������ ��ġ. PriorityQueueó�� ����� ������ TArray 
	//closeSet = �̵� �Ұ������� Ȯ���� ��ġ. ����� �������� �ǵ��ϱ� ���� TSet
	TArray<UPathNode*> openList;
	TSet<UPathNode*> closeSet;

	//Heapȭ == PriorityQueue
	openList.Heapify(UPathNode::PathFindingPredicated);

	//���� ��ġ
	UPathNode* startNode = PathFindingSystem->GetValidPathNode(Start);
	if (!IsValid(startNode))
	{
		UE_LOG(LogTemp, Warning, TEXT("StartNode is Not Valid"));
		PathLength = -1;
		return TArray<FGrid>();
	}

	//��ǥ ��ġ
 	UPathNode* endNode = PathFindingSystem->GetValidPathNode(End);
	if (!IsValid(endNode))
	{
		UE_LOG(LogTemp, Warning, TEXT("EndNode is Not Valid"));
		PathLength = -1;
		return TArray<FGrid>();
	}

	//��� PathNode�� �ʱ�ȭ �� �ڿ� ������.
	InitAllPathFindingNodes();

	startNode->SetGCost(0);
	startNode->SetHCost(CalculateGridDistance(Start, End));
	startNode->CalculateFCost();

	openList.Add(startNode);

	while (openList.Num() > 0)
	{
		UPathNode* currentNode = GetLowestFCostNode(openList);

		if (currentNode == endNode) //Break Point.
		{
			FGrid currentGrid = currentNode->GetGrid();
			AUnitCharacter* currentUnit = GetUnitAtGrid(currentGrid);
			AUnitCharacter* startUnit = GetUnitAtGrid(Start);
			
			//�����̵� �Ʊ��̵� �������� �����Ѵٸ�, �ش� Grid�� ������ �� �����Ƿ�
			//��ΰ� ������ �� ���Ե�.
			//�ٸ�, �ڱ� �ڽ��̶�� ������.
			if (IsValid(currentUnit) && currentUnit != startUnit)
			{
				PathLength = -1;
				return TArray<FGrid>();
			}

			PathLength = endNode->GetFCost();
			return CalculatePath(endNode);
		}

		openList.Remove(currentNode);
		closeSet.Add(currentNode);

		//UE_LOG(LogTemp, Warning, TEXT("Node Position : %d, %d"), currentNode->GetGrid().X, currentNode->GetGrid().Y);

		TArray<UPathNode*> nearNodeArray = GetNearNodeArray(currentNode);
		for (UPathNode* nearNode : nearNodeArray)
		{
			//close Set �ȿ� �ִ� ���� ����.
			if (closeSet.Contains(nearNode))
			{
				continue;
			}

			//���� �� ���� ��ġ ����.
			if (nearNode->GetIsWalkable() == false)
			{
				closeSet.Add(nearNode);
				continue;
			}

			//bCanIgnoreUnit�� true�� ��, �� ������ ����� �� ����.
			//GridVisual�� ���Ⱑ �ƴ϶� MoveActionComponent���� ValidGridVisual�� üũ��.
			//���� ������ GridSystem�� ������ �ʿ���.
			UGridObject* gridObj = GridSystem->GetValidGridObject(nearNode->GetGrid());
			if (!bCanIgnoreUnit && IsValid(gridObj) && gridObj->HasAnyUnit())
			{
				AUnitCharacter* currentUnit = gridObj->GetUnit();
				AUnitCharacter* startUnit = GetUnitAtGrid(Start);
				if (IsValid(currentUnit) && IsValid(startUnit))
				{
					if (currentUnit->ActorHasTag(startUnit->Tags[0]))
					{
						//�Ʊ��̸� �ϴ� ����� ������.
					}
					else
					{
						//�Ʊ��� �ƴ϶�� ����� �Ұ�����.
						closeSet.Add(nearNode);
						continue;
					}
				}
			}


			//F = G + H;
			//G : ��������� Cost
			//H : ������ ����Ǵ� Cost
			int tempGCost = currentNode->GetGCost() + CalculateGridDistance(currentNode->GetGrid(), nearNode->GetGrid());

			if (tempGCost < nearNode->GetGCost())
			{
				nearNode->SetParentNode(currentNode);
				nearNode->SetGCost(tempGCost);
				nearNode->SetHCost(CalculateGridDistance(nearNode->GetGrid(), End));
				nearNode->CalculateFCost();

				if (!openList.Contains(nearNode))
				{
					openList.Add(nearNode);
				}
			}
		}
	}

	//openList.Num() > 0�� ������ ���� ������ �ùٸ� ��θ� ��ã�Ҵٸ� ����.
	PathLength = -1;
	return TArray<FGrid>();
}

int32 AGridManager::CalculateGridDistance(const FGrid& a, const FGrid& b) const
{
	return FMath::Abs(a.X - b.X) + FMath::Abs(a.Y - b.Y);
}

UPathNode* AGridManager::GetLowestFCostNode(TArray<UPathNode*>& PathNodeList)
{
	if (PathNodeList.Num() == 0)
	{
		return nullptr;
	}

	//PathNodeList�� FindPath���� Heapify�� �ƴ�. Heap�� ù��° ���Ұ� ���� F ���� ���� Node��.
	UPathNode* lowestNode = PathNodeList[0];

	return lowestNode;
}

TArray<FGrid> AGridManager::CalculatePath(UPathNode* EndNode) const
{
	if (!IsValid(EndNode))
	{
		return TArray<FGrid>();
	}

	//Path�� ��� ��
	//PathFinding�� �� ��, ��ǥ ���� ������ Parent�� ��� �ö󰡸鼭 Grid ���� ����.
	//�� Grid ����� �������� Start -> End ������ Path��.
	TArray<FGrid> gridArray;

	gridArray.Add(EndNode->GetGrid());
	UPathNode* current = EndNode;
	while (IsValid(current))
	{
		gridArray.Add(current->GetGrid());
		current = current->GetParentNode();
	}

	//����� reverse�ϸ� �ùٸ� ������ ��ΰ� ����.
	Algo::Reverse(gridArray);

	return gridArray;
}

TArray<UPathNode*> AGridManager::GetNearNodeArray(UPathNode* CurrentNode) const
{
	//PathFinding���� ����� ���� ��ġ�� �����¿� Node�� ã�� Function
	//����, �������� �ʴ� ����� NearNode�� �߰����� ����.
	//TODO : ��ֹ� ó���� ���� ���� �ʾ���.

	TArray<UPathNode*> nearNodeList;

	FGrid grid = CurrentNode->GetGrid();

	TArray<int32> dx{ 1,-1,0,0 };
	TArray<int32> dy{ 0,0,1,-1 };

	for (int32 i = 0; i<dx.Num();i++)
	{
		FGrid near = grid;
		near.X += dx[i];
		near.Y += dy[i];

		if (IsValidGrid(near))
		{
			UPathNode* nearNode = PathFindingSystem->GetValidPathNode(near);

			if (!IsValid(nearNode))
			{
				continue;
			}

			nearNodeList.Add(nearNode);
		}
	}

	return nearNodeList;
}

bool AGridManager::HasPath(const FGrid& Start, const FGrid& End, bool bCanIgnoreUnit)
{
	//FindPath�� ��� �� PathLength�� -1�̸� ��ġ������ ��ΰ� �������� ����.

	int32 pathLength = 0;
	FindPath(Start, End, pathLength, bCanIgnoreUnit);

	return pathLength != -1;
}

void AGridManager::InitAllPathFindingNodes()
{
	//PathFindingGridSystem�� Grid ���� PathFinding�� �̿��� �� �ֵ��� ���� �ʱ�ȭ.
	//G = ���Ѵ�(int32 �ִ밪) , H = 0 , F = G + H = ���Ѵ�

	TMap<FGrid, UPathNode*> pathNodes = PathFindingSystem->GetPathNodeMap();

	for (TPair<FGrid, UPathNode*> pathNodePair : pathNodes)
	{
		if (!IsValid(pathNodePair.Value))
		{
			//UE_LOG(LogTemp, Warning, TEXT("PathNode is Not Valid"));
			continue;
		}

		UPathNode* pathNode = pathNodePair.Value;

		pathNode->SetGCost(TNumericLimits<int32>::Max());
		pathNode->SetHCost(0);
		pathNode->CalculateFCost();
		pathNode->SetParentNode(nullptr);
	}
}


TArray<AUnitCharacter*> AGridManager::GetUnitArrayAtGrid(const FGrid& GridValue) const
{
	//Grid ���� �ִ� Unit�� ����.
	//�� Function�� Unit�� �������� ���� ������.

	UGridObject* gridObject = GridSystem->GetValidGridObject(GridValue);
	if (!IsValid(gridObject))
	{
		return TArray<AUnitCharacter*>();
	}


	return gridObject->GetUnitArray();

}

TArray<AUnitCharacter*> AGridManager::GetAllUnitInGridSystem() const
{
	//���� Grid ���� �����ϴ� ��� Unit Character�� ��.

	TMap<FGrid,UGridObject*> gridObjs = GetAllGridObjectsThatHasUnit();

	TArray<AUnitCharacter*> unitArray;

	for (auto gridObj : gridObjs)
	{
		auto unit = gridObj.Value->GetUnit();
		if (!IsValid(unit))
		{
			continue;
		}
		unitArray.Add(unit);
	}

	return unitArray;
}

AUnitCharacter* AGridManager::GetUnitAtGrid(const FGrid& GridValue) const
{
	//Grid ���� �ִ� Unit�� ����.
	//�� Function�� Unit�� �ϳ��� ���� ������.

	TArray<AUnitCharacter*> gridArray = GetUnitArrayAtGrid(GridValue);

	if (gridArray.Num() == 0)
	{
		return nullptr;
	}

	return gridArray[0];
}

bool AGridManager::HasAnyUnitOnGrid(const FGrid& GridValue) const
{
	//Grid ���� Unit�� �����ϴ��� Ȯ��.

	UGridObject* gridObj = GridSystem->GetValidGridObject(GridValue);
	if (IsValid(gridObj))
	{
		return gridObj->HasAnyUnit();
	}
	
	return false;
}

bool AGridManager::IsWalkableGrid(const FGrid& GridValue) const
{
	//��ֹ�, ���� ��, ���? ���� ������ �ö� �� ���� ��ġ���� Ȯ����
	//TODO : ���� ������ ��츦 �����ؾ���.

	UPathNode* pathNode = PathFindingSystem->GetValidPathNode(GridValue);
	if (IsValid(pathNode))
	{
		return pathNode->GetIsWalkable();
	}

	return false;
}

int32 AGridManager::GetPathLength(const FGrid& Start, const FGrid& End)
{
	int32 pathLength = 0;
	FindPath(Start,End, pathLength);
	return pathLength;
}

void AGridManager::AddUnitAtGrid(AUnitCharacter* Unit, const FGrid& GridValue)
{
	UGridObject* gridObject = GridSystem->GetValidGridObject(GridValue);
	if (IsValid(gridObject))
	{
		gridObject->AddUnit(Unit);
	}
}

void AGridManager::RemoveUnitAtGrid(AUnitCharacter* Unit, const FGrid& GridValue)
{
	UGridObject* gridObject = GridSystem->GetValidGridObject(GridValue);
	if (IsValid(gridObject))
	{
		gridObject->RemoveUnit(Unit);
	}
}

void AGridManager::MoveUnitGrid(AUnitCharacter* Unit, const FGrid& From, const FGrid& to)
{
	RemoveUnitAtGrid(Unit, From);
	AddUnitAtGrid(Unit, to);

	//Unit�� �ڽ��� ���� ��ġ�� �����ϰ� ����.
	Unit->SetGrid(to);

	if (OnAnyUnitMoved.IsBound())
	{
		OnAnyUnitMoved.Broadcast();
	}
}

TMap<FGrid, UGridObject*> AGridManager::GetAllGridObjectsThatHasUnit() const
{
	if (!IsValid(GridSystem))
	{
		return TMap<FGrid, UGridObject*>();
	}

	return GridSystem->GetAllGridObjectsThatHasUnit();
}

AGridManager* AGridManager::GetGridManager()
{
	if (!IsValid(GEngine) || !IsValid(GEngine->GameViewport))
	{
		return nullptr;
	}

	UWorld* world = GEngine->GameViewport->GetWorld();
	AGridManager* gridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(world, AGridManager::StaticClass()));

	return gridManager;
}

void AGridManager::ShowEnemyRange()
{
	//1. Enemy Unit�� ���� ã��.
	//2. Enemy Unit�� ������ Attackable Grid�� �˻���.
	//3. TSet�� �ߺ��� ������� �����Ƿ� �ߺ��� �� �ɷ��� ���·� ���Ŀ� Show.

	GridVisual_DANGER->ClearInstances();
	
	TArray<AUnitCharacter*> unitArr = GetAllUnitInGridSystem();
	TArray<AUnitCharacter*> enemyArr;
	TSet<FGrid> resultGrids;

	for (auto unit : unitArr)
	{
		if (!IsValid(unit))
		{
			continue;
		}

		if (unit->ActorHasTag(TEXT("Enemy")))
		{
			enemyArr.Add(unit);
		}
	}

	for (auto enemy : enemyArr)
	{
		auto unitAttackComp= enemy->FindComponentByClass<UUnitAttackActionComponent>();
		if (IsValid(unitAttackComp))
		{
			TSet<FGrid> attackableGrids = unitAttackComp->GetEnemyAttackableGridRange();
			for (auto grid : attackableGrids)
			{
				resultGrids.Add(grid);
			}
		}
	}

	ShowFromGridSet(resultGrids, EGridVisualType::DANGER, 0.001f);

}

void AGridManager::ClearEnemyRange()
{
	GridVisual_DANGER->ClearInstances();
}
