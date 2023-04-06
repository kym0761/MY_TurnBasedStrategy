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

	//Grid 맵에 장애물 적용. 통과 불가.
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
				ECollisionChannel::ECC_GameTraceChannel2)); // ObjectType : Obstacle.. DefaultEngine.ini 참고
			
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

			//outhits이 1개 이상의 값을 가지고 있다면, 해당 위치에 장애물이 존재하고 있는 것임.
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
	//Grid 한 지점에서 Range만큼 Show해줌.

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
	//!주의! return 하기 전에 PathLength를 변경시켜야함.
	
	//PathLength의 존재 이유는, 장애물이나 다른 기타 이유로 빙 돌아서 가야할 때
	//이동력에 의해 닿을 수 있는 위치인지 확인해야함.
	//예시) 이동력이 5인 유닛은 해당 위치까지 가는 PathLength가 5 이하일 때만 해당 위치를 갈 수 있음.

	//openList = 이동 가능할 위치. PriorityQueue처럼 사용할 것으로 TArray 
	//closeSet = 이동 불가능함이 확정된 위치. 사용한 것인지만 판독하기 위한 TSet
	TArray<UPathNode*> openList;
	TSet<UPathNode*> closeSet;

	//Heap화 == PriorityQueue
	openList.Heapify(UPathNode::PathFindingPredicated);

	//시작 위치
	UPathNode* startNode = PathFindingSystem->GetValidPathNode(Start);
	if (!IsValid(startNode))
	{
		UE_LOG(LogTemp, Warning, TEXT("StartNode is Not Valid"));
		PathLength = -1;
		return TArray<FGrid>();
	}

	//목표 위치
 	UPathNode* endNode = PathFindingSystem->GetValidPathNode(End);
	if (!IsValid(endNode))
	{
		UE_LOG(LogTemp, Warning, TEXT("EndNode is Not Valid"));
		PathLength = -1;
		return TArray<FGrid>();
	}

	//모든 PathNode를 초기화 한 뒤에 시작함.
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
			
			//적군이든 아군이든 누군가가 존재한다면, 해당 Grid를 점유할 수 없으므로
			//경로가 존재할 수 없게됨.
			//다만, 자기 자신이라면 가능함.
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
			//close Set 안에 있는 노드는 무시.
			if (closeSet.Contains(nearNode))
			{
				continue;
			}

			//걸을 수 없는 위치 무시.
			if (nearNode->GetIsWalkable() == false)
			{
				closeSet.Add(nearNode);
				continue;
			}

			//bCanIgnoreUnit이 true일 때, 적 유닛을 통과할 수 있음.
			//GridVisual은 여기가 아니라 MoveActionComponent에서 ValidGridVisual을 체크함.
			//유닛 정보는 GridSystem에 접근이 필요함.
			UGridObject* gridObj = GridSystem->GetValidGridObject(nearNode->GetGrid());
			if (!bCanIgnoreUnit && IsValid(gridObj) && gridObj->HasAnyUnit())
			{
				AUnitCharacter* currentUnit = gridObj->GetUnit();
				AUnitCharacter* startUnit = GetUnitAtGrid(Start);
				if (IsValid(currentUnit) && IsValid(startUnit))
				{
					if (currentUnit->ActorHasTag(startUnit->Tags[0]))
					{
						//아군이면 일단 통과가 가능함.
					}
					else
					{
						//아군이 아니라면 통과가 불가능함.
						closeSet.Add(nearNode);
						continue;
					}
				}
			}


			//F = G + H;
			//G : 현재까지의 Cost
			//H : 앞으로 예상되는 Cost
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

	//openList.Num() > 0인 조건이 끝날 때까지 올바른 경로를 못찾았다면 실패.
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

	//PathNodeList는 FindPath에서 Heapify가 됐다. Heap의 첫번째 원소가 가장 F 값이 낮은 Node다.
	UPathNode* lowestNode = PathNodeList[0];

	return lowestNode;
}

TArray<FGrid> AGridManager::CalculatePath(UPathNode* EndNode) const
{
	if (!IsValid(EndNode))
	{
		return TArray<FGrid>();
	}

	//Path를 얻는 법
	//PathFinding을 한 후, 목표 지점 노드부터 Parent를 계속 올라가면서 Grid 값을 얻어낸다.
	//그 Grid 결과를 뒤집으면 Start -> End 까지의 Path다.
	TArray<FGrid> gridArray;

	gridArray.Add(EndNode->GetGrid());
	UPathNode* current = EndNode;
	while (IsValid(current))
	{
		gridArray.Add(current->GetGrid());
		current = current->GetParentNode();
	}

	//결과를 reverse하면 올바른 방향의 경로가 나옴.
	Algo::Reverse(gridArray);

	return gridArray;
}

TArray<UPathNode*> AGridManager::GetNearNodeArray(UPathNode* CurrentNode) const
{
	//PathFinding에서 사용할 현재 위치의 상하좌우 Node를 찾는 Function
	//만약, 존재하지 않는 노드라면 NearNode로 추가하지 않음.
	//TODO : 장애물 처리를 아직 하지 않았음.

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
	//FindPath의 결과 중 PathLength가 -1이면 위치까지의 경로가 존재하지 않음.

	int32 pathLength = 0;
	FindPath(Start, End, pathLength, bCanIgnoreUnit);

	return pathLength != -1;
}

void AGridManager::InitAllPathFindingNodes()
{
	//PathFindingGridSystem의 Grid 값을 PathFinding에 이용할 수 있도록 전부 초기화.
	//G = 무한대(int32 최대값) , H = 0 , F = G + H = 무한대

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
	//Grid 위에 있는 Unit을 얻음.
	//이 Function은 Unit이 여러개일 때를 가정함.

	UGridObject* gridObject = GridSystem->GetValidGridObject(GridValue);
	if (!IsValid(gridObject))
	{
		return TArray<AUnitCharacter*>();
	}


	return gridObject->GetUnitArray();

}

TArray<AUnitCharacter*> AGridManager::GetAllUnitInGridSystem() const
{
	//현재 Grid 위에 존재하는 모든 Unit Character를 얻어냄.

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
	//Grid 위에 있는 Unit을 얻음.
	//이 Function은 Unit이 하나일 때를 가정함.

	TArray<AUnitCharacter*> gridArray = GetUnitArrayAtGrid(GridValue);

	if (gridArray.Num() == 0)
	{
		return nullptr;
	}

	return gridArray[0];
}

bool AGridManager::HasAnyUnitOnGrid(const FGrid& GridValue) const
{
	//Grid 위에 Unit이 존재하는지 확인.

	UGridObject* gridObj = GridSystem->GetValidGridObject(GridValue);
	if (IsValid(gridObj))
	{
		return gridObj->HasAnyUnit();
	}
	
	return false;
}

bool AGridManager::IsWalkableGrid(const FGrid& GridValue) const
{
	//장애물, 높은 벽, 용암? 등의 유닛이 올라갈 수 없는 위치인지 확인함
	//TODO : 공중 유닛의 경우를 생각해야함.

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

	//Unit이 자신의 현재 위치를 보유하고 있음.
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
	//1. Enemy Unit을 전부 찾음.
	//2. Enemy Unit의 각각의 Attackable Grid를 검색함.
	//3. TSet은 중복을 허용하지 않으므로 중복을 다 걸러낸 상태로 이후에 Show.

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
