// Fill out your copyright notice in the Description page of Project Settings.


#include "GridManager.h"
#include "Grid/GridObject.h"
#include "Grid/PathObject.h"
#include "Grid/InstancedGridVisualComponent.h"
#include "Grid/PathFindingSystem.h"
#include "Grid/GridSystem.h"

#include "Components/BillboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UnitCore/Unit.h"

#include "UnitAction/UnitAttackActionComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "GridCostModifier.h"

#include "DebugHelper.h"


// Sets default values
AGridManager::AGridManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//드래그 중에 OnConstruction이 동작하는 것을 방지
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

	SetupGridSystem();

	SetupUnitsOnGrid();

}

// Called every frame
void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGridManager::SetupGridSystem()
{
	GridSystem = NewObject<UGridSystem>(this);

	GridSystem->SetGridSystem(
		X_Length,
		Y_Length,
		[](UGridSystem* gs, FGrid grid) {
			UGridObject* gridObj = NewObject<UGridObject>();
			gridObj->SetGrid(grid);
			gridObj->SetGridSystem(gs);
			return gridObj;
		});

	PathFindingSystem = NewObject<UPathFindingSystem>(this);

	PathFindingSystem->SetPathFindingSystem(
		X_Length,
		Y_Length,
		[](UPathFindingSystem* pfs, FGrid grid)
		{
			UPathObject* pathObject = NewObject<UPathObject>();
			pathObject->SetGrid(grid);
			return pathObject;
		}
	);

	//Grid 맵에 장애물 적용. 통과 불가.
	{
		auto pathFindingMap = PathFindingSystem->GetPathObjectMap();
		for (int32 i = 0; i < X_Length; i++)
		{
			for (int32 j = 0; j < Y_Length; j++)
			{
				FGrid grid(i, j);
				FVector pos = GridToWorld(grid);

				FVector startPos = pos + FVector(0.0f, 0.0f, 5000.0f);
				FVector endPos = FVector(startPos.X, startPos.Y, -5000.0f);

				TArray<TEnumAsByte<EObjectTypeQuery>> objects;

				objects.Add(UEngineTypes::ConvertToObjectType(
					ECollisionChannel::ECC_GameTraceChannel2)); // ObjectType : Obstacle Obj Type을 감지함. DefaultEngine.ini 참고

				TArray<AActor*> ignores;
				TArray<FHitResult> outHits;

				UKismetSystemLibrary::LineTraceMultiForObjects(
					GetWorld(),
					startPos,
					endPos,
					objects,
					true,
					ignores,
					EDrawDebugTrace::None,
					//EDrawDebugTrace::ForDuration,
					outHits,
					true,
					FLinearColor::Red,
					FLinearColor::Blue,
					5.0f
				);

				//outhits이 1개 이상의 값을 가지고 있다면, 해당 위치에 장애물이 존재하고 있는 것임.
				if (outHits.Num() > 0)
				{
					UPathObject* pathObject = pathFindingMap[grid];
					if (IsValid(pathObject))
					{
						pathObject->SetIsWalkable(false);
					}
				}
			}
		}
	}

	//GridModifier 체크
	{
		TArray<AActor*> outActors;
		UGameplayStatics::GetAllActorsOfClass(this, AGridCostModifier::StaticClass(), outActors);
		auto gridObjMap = GridSystem->GetGridObjectMap();

		for (auto i : outActors)
		{
			AGridCostModifier* gridCostModifier = CastChecked<AGridCostModifier>(i);
			if (!IsValid(gridCostModifier))
			{
				continue;
			}
			FGrid grid = WorldToGrid(gridCostModifier->GetActorLocation());
			UGridObject* gridObj = gridObjMap[grid];
			if (IsValid(gridObj))
			{
				gridObj->SetGridCost(gridCostModifier->CostValue);
			}
		}
	}

}

TArray<AUnit*> AGridManager::GetUnitArrayAtGrid(const FGrid& GridValue) const
{
	//Grid 위에 있는 Unit을 얻음.
	//이 Function은 Unit이 여러개일 때를 가정함.

	UGridObject* gridObject = GridSystem->GetValidGridObject(GridValue);
	if (!IsValid(gridObject))
	{
		return TArray<AUnit*>();
	}

	return gridObject->GetUnitArray();
}

TArray<AUnit*> AGridManager::GetAllUnitInGridSystem() const
{
	//현재 Grid 위에 존재하는 모든 Unit을 얻어냄.

	TMap<FGrid, UGridObject*> gridObjs = GetAllGridObjectsThatHasUnit();

	TArray<AUnit*> unitArray;

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

AUnit* AGridManager::GetUnitAtGrid(const FGrid& GridValue) const
{
	//Grid 위에 있는 Unit을 얻음.
	//이 Function은 Unit이 하나일 때를 가정함.

	TArray<AUnit*> gridArray = GetUnitArrayAtGrid(GridValue);

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

	UPathObject* pathObject = PathFindingSystem->GetValidPathObject(GridValue);
	if (IsValid(pathObject))
	{
		return pathObject->GetIsWalkable();
	}

	return false;
}

int32 AGridManager::GetPathLength(const FGrid& Start, const FGrid& End, const int32 MaxMoveCost)
{
	int32 pathLength = 0;
	FindPath(Start, End, pathLength, MaxMoveCost);
	return pathLength;
}

bool AGridManager::IsValidGrid(const FGrid& Grid) const
{
	return (Grid.X >= 0) && (Grid.Y >= 0) && (Grid.X < X_Length) && (Grid.Y < Y_Length);
}

FGrid AGridManager::WorldToGrid(const FVector& WorldPosition) const
{
	FVector managerLocation = GetActorLocation();

	FGrid grid;
	grid.X = FMath::RoundToInt32((WorldPosition.X - managerLocation.X) / CellSize);
	grid.Y = FMath::RoundToInt32((WorldPosition.Y - managerLocation.Y) / CellSize);

	//Debug::Print(DEBUG_TEXT("Grid Pos : ") + grid.ToString());

	return grid;
}

FVector AGridManager::GridToWorld(const FGrid& Grid) const
{
	FVector managerLocation = GetActorLocation();

	FVector worldPosition = managerLocation;
	worldPosition.X += Grid.X * CellSize;
	worldPosition.Y += Grid.Y * CellSize;

	//Debug::Print(DEBUG_TEXT("World Pos : ") + worldPosition.ToString());

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

void AGridManager::AddUnitAtGrid(AUnit* Unit, const FGrid& GridValue)
{
	UGridObject* gridObject = GridSystem->GetValidGridObject(GridValue);
	if (!IsValid(gridObject))
	{
		Debug::Print(DEBUG_TEXT("AddUnitAtGrid Failed."));
		return;
	}

	gridObject->AddUnit(Unit);
}

void AGridManager::RemoveUnitAtGrid(AUnit* Unit, const FGrid& GridValue)
{
	UGridObject* gridObject = GridSystem->GetValidGridObject(GridValue);
	if (!IsValid(gridObject))
	{
		Debug::Print(DEBUG_TEXT("RemoveUnitAtGrid Failed."));
		return;
	}

	gridObject->RemoveUnit(Unit);
}

void AGridManager::MoveUnitGrid(AUnit* Unit, const FGrid& From, const FGrid& to)
{
	RemoveUnitAtGrid(Unit, From);
	AddUnitAtGrid(Unit, to);

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

void AGridManager::SetupUnitsOnGrid()
{
	//게임 시작할 때 비어있는 Grid에
	//모든 유닛을 Grid에 매칭시킨다.
	TArray<AActor*> units;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnit::StaticClass(), units);

	for (auto i : units)
	{
		auto unit = Cast<AUnit>(i);

		if (IsValid(unit))
		{
			AddUnitAtGrid(unit, WorldToGrid(unit->GetActorLocation()));
		}

	}
}

TArray<FGrid> AGridManager::FindPath(const FGrid& Start, const FGrid& End, int32& PathLength, const int32 MaxMoveCost, bool bCanIgnoreUnit, bool bCalculateToTarget)
{
	//!주의! return 하기 전에 PathLength를 변경시켜야함.

	//PathLength의 존재 이유는, 장애물이나 다른 기타 이유로 우회할 때
	//이동력에 의해 닿을 수 있는 위치인지 확인해야함.
	//예시) 이동력이 5인 유닛은 해당 위치까지 가는 PathLength가 5 이하일 때만 Valid.

	FGrid checkGrid = Start - End;
	if (checkGrid.Size() > MaxMoveCost)
	{
		Debug::Print(DEBUG_TEXT("Too Far Distance of StartGrid from EndGrid."));
		PathLength = -1;
		return TArray<FGrid>();
	}

	TArray<UPathObject*> openList; 	//openList = 이동 가능할 위치. Heap으로 사용함.
	TSet<UPathObject*> closeSet; 	//closeSet = 이동 불가능한 위치. 빠른 검색용 TSet

	//Heap화 == PriorityQueue
	openList.Heapify(UPathObject::PathFindingPredicated);

	//시작 위치
	UPathObject* startObject = PathFindingSystem->GetValidPathObject(Start);
	if (!IsValid(startObject))
	{
		Debug::Print(DEBUG_TEXT("startObject is Invalid."));
		PathLength = -1;
		return TArray<FGrid>();
	}

	//목표 위치
	UPathObject* endObject = PathFindingSystem->GetValidPathObject(End);
	if (!IsValid(endObject))
	{
		Debug::Print(DEBUG_TEXT("endObject is Invalid."));
		PathLength = -1;
		return TArray<FGrid>();
	}

	//모든 PathObject를 초기화 한 뒤에 시작함.
	InitPathFindingObjects(Start, MaxMoveCost);

	//startObject 상태
	//G = 0 , H = 예상되는 직선 길이(X+Y), F = G+H
	startObject->SetGCost(0);
	startObject->SetHCost(CalculateGridDistance(Start, End));
	startObject->CalculateFCost();

	openList.Add(startObject);

	while (openList.Num() > 0)
	{
		UPathObject* currentObject = GetLowestFCostObject(openList);

		openList.Remove(currentObject); // 현재 위치를 Openlist에서 제거

		if (currentObject->GetGCost() > MaxMoveCost)
		{
			//이 위치로는 이동할 수 없음. 다른 길을 찾아야함.
			continue;
		}

		if (currentObject == endObject) //Break Point.
		{
			FGrid currentGrid = currentObject->GetGrid();
			AUnit* currentUnit = GetUnitAtGrid(currentGrid);
			AUnit* startUnit = GetUnitAtGrid(Start);

			//적군이든 아군이든 누군가가 존재한다면, 해당 Grid를 점유할 수 없으므로 경로가 존재할 수 없게됨.
			//다만, 자기 자신이라면 가능함.
			if (!bCalculateToTarget && IsValid(currentUnit) && currentUnit != startUnit)
			{
				PathLength = -1;
				return TArray<FGrid>();
			}

			PathLength = endObject->GetFCost();
			return CalculatePath(endObject);
		}


		TArray<UPathObject*> nearObjectArray = GetNearObjectArray(currentObject);
		for (UPathObject* nearObject : nearObjectArray)
		{
			//close Set 안에 있는 노드는 무시.
			if (closeSet.Contains(nearObject))
			{
				continue;
			}

			//걸을 수 없는 위치 무시.
			if (nearObject->GetIsWalkable() == false)
			{
				closeSet.Add(nearObject);
				continue;
			}

			//bCanIgnoreUnit이 true일 때, 적 유닛을 통과할 수 있음.
			//GridVisual은 여기가 아니라 MoveActionComponent에서 ValidGridVisual을 체크함.
			//유닛 정보는 GridSystem에 접근이 필요함.
			UGridObject* gridObj = GridSystem->GetValidGridObject(nearObject->GetGrid());
			if (!bCanIgnoreUnit && IsValid(gridObj) && gridObj->HasAnyUnit())
			{
				AUnit* currentUnit = gridObj->GetUnit();
				AUnit* startUnit = GetUnitAtGrid(Start);
				if (IsValid(currentUnit) && IsValid(startUnit))
				{
					if (currentUnit->GetTeamType() == startUnit->GetTeamType())
					{
						//아군이면 일단 통과가 가능함.
					}
					else
					{
						//아군이 아니라면 통과가 불가능함.
						closeSet.Add(nearObject);
						continue;
					}
				}
			}


			//F = G + H;
			//G : 현재까지의 Cost
			//H : 앞으로 예상되는 Cost

			//G = 현재까지의 GCost + 다음노드 진입에 필요한 Cost
			int tempGCost = currentObject->GetGCost() + nearObject->GetGridCost();

			if (tempGCost < nearObject->GetGCost())
			{
				nearObject->SetParentObject(currentObject);
				nearObject->SetGCost(tempGCost);
				nearObject->SetHCost(CalculateGridDistance(nearObject->GetGrid(), End));
				nearObject->CalculateFCost();

				if (!openList.Contains(nearObject))
				{
					openList.Add(nearObject);
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

UPathObject* AGridManager::GetLowestFCostObject(TArray<UPathObject*>& PathObjectList)
{
	if (PathObjectList.Num() == 0)
	{
		return nullptr;
	}

	//PathObjectList는 FindPath에서 Heapify가 됐다. Heap의 첫번째 원소가 가장 F 값이 낮은 Object다.
	UPathObject* lowestObject = PathObjectList[0];

	return lowestObject;
}

TArray<FGrid> AGridManager::CalculatePath(UPathObject* EndObject) const
{
	if (!IsValid(EndObject))
	{
		return TArray<FGrid>();
	}

	//Path를 얻는 법
	//PathFinding을 한 후, 목표 지점 노드부터 Parent를 계속 올라가면서 Grid 값을 얻어낸다.
	//그 Grid 결과를 뒤집으면 Start -> End 까지의 Path다.
	TArray<FGrid> gridArray;

	gridArray.Add(EndObject->GetGrid());
	UPathObject* current = EndObject;
	while (IsValid(current))
	{
		gridArray.Add(current->GetGrid());
		current = current->GetParentObject();
	}

	//결과를 reverse하면 올바른 방향의 경로가 나옴.
	Algo::Reverse(gridArray);

	return gridArray;
}

TArray<UPathObject*> AGridManager::GetNearObjectArray(UPathObject* CurrentObject) const
{
	//PathFinding에서 사용할 현재 위치의 상하좌우 Object를 찾는 Function
	//만약, 존재하지 않는 노드라면 NearObject로 추가하지 않음.

	TArray<UPathObject*> nearObjectList;

	FGrid grid = CurrentObject->GetGrid();

	TArray<int32> dx{ 1,-1,0,0 };
	TArray<int32> dy{ 0,0,1,-1 };

	for (int32 i = 0; i < dx.Num(); i++)
	{
		FGrid near = grid;
		near.X += dx[i];
		near.Y += dy[i];

		if (IsValidGrid(near))
		{
			UPathObject* nearObject = PathFindingSystem->GetValidPathObject(near);

			if (!IsValid(nearObject))
			{
				continue;
			}

			nearObjectList.Add(nearObject);
		}
	}

	return nearObjectList;
}

bool AGridManager::HasPath(const FGrid& Start, const FGrid& End, int32 MaxMoveCost, bool bCanIgnoreUnit)
{
	//FindPath의 결과 중 PathLength가 -1이면 위치까지의 경로가 존재하지 않음.

	int32 pathLength = 0;
	FindPath(Start, End, pathLength, MaxMoveCost, bCanIgnoreUnit);

	return pathLength != -1;
}

void AGridManager::InitAllPathFindingObjects()
{
	//PathFindingGridSystem의 Grid 값을 PathFinding에 이용할 수 있도록 전부 초기화.
	//G = 무한대(int32 최대값) , H = 0 , F = G + H = 무한대

	//100*100 정도의 큰 맵에서는 전체 초기화를 하기 때문에 랙이 발생할 가능성이 높음
	//새로 만든 로직이 별 문제가 없으면 삭제할 함수임.
	
	TMap<FGrid, UPathObject*> pathObjects = PathFindingSystem->GetPathObjectMap();
	TMap<FGrid, UGridObject*> gridObjs = GridSystem->GetGridObjectMap();

	for (int x = 0; x < X_Length; x++)
	{
		for (int y = 0; y < Y_Length; y++)
		{
			FGrid grid(x, y);
			UPathObject* pathObject = pathObjects[grid];
			UGridObject* gridObj = gridObjs[grid];

			if (!IsValid(pathObject) || !IsValid(gridObj))
			{
				Debug::Print(DEBUG_TEXT("Invalid Position."));
				continue;
			}

			int32 tempG_Cost = TNumericLimits<int32>::Max();
			int32 tempH_Cost = 0;
			int32 tempGridCost = gridObj->GetGridCost();

			pathObject->SetGCost(tempG_Cost);
			pathObject->SetHCost(tempH_Cost);
			pathObject->CalculateFCost();
			pathObject->SetGridCost(tempGridCost);
			pathObject->SetParentObject(nullptr);
		}
	}
}

void AGridManager::InitPathFindingObjects(const FGrid& StartGrid, const int32 MaxMoveCost)
{
	//PathFindingGridSystem의 Grid 값을 PathFinding에 이용할 수 있도록 전부 초기화.
	//G = 무한대(int32 최대값) , H = 0 , F = G + H = 무한대
	

	TMap<FGrid, UPathObject*> pathObjects = PathFindingSystem->GetPathObjectMap();
	TMap<FGrid, UGridObject*> gridObjs = GridSystem->GetGridObjectMap();

	//상하좌우 Near 계산을 감안하고 1칸 정도의 padding(?)을 준다. 
	int max = MaxMoveCost + 1;

	//모든 값을 초기화하는 것보다, 필요한 만큼만 초기화한다.
	//전체 초기화보다 확연히 빠르다!
	for (int32 y = -max; y <= max; y++)
	{
		for (int32 x = -max; x <= max; x++)
		{
			if (FMath::Abs(x) + FMath::Abs(y) > MaxMoveCost)
			{
				continue;
			}

			FGrid grid = StartGrid + FGrid(x, y);

			if (!pathObjects.Contains(grid) || !gridObjs.Contains(grid))
			{
				continue;
			}

			UPathObject* pathObject = pathObjects[grid];
			UGridObject* gridObj = gridObjs[grid];

			int32 tempG_Cost = TNumericLimits<int32>::Max();
			int32 tempH_Cost = 0;
			int32 tempGridCost = gridObj->GetGridCost(); // 이 그리드를 지나기 위한 Cost

			pathObject->SetGCost(tempG_Cost);
			pathObject->SetHCost(tempH_Cost);
			pathObject->CalculateFCost();
			pathObject->SetGridCost(tempGridCost);
			pathObject->SetParentObject(nullptr);

		}
	}

}

void AGridManager::RemoveAllGridVisual()
{
	GridVisual_Move->RemoveGridVisuals();
	GridVisual_OK->RemoveGridVisuals();
	GridVisual_NO->RemoveGridVisuals();
	GridVisual_Warning->RemoveGridVisuals();
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
		toDraw->DrawGridVisualswithGridSet(GridSet, Height);
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

	//최적화 이슈?
	//만약 100X100이면 10000개에 대한 Grid를 조사해야함.
	//여기서 ActorsOfClass로 Unit만 추수리는 동작을 한다해도,
	//길찾기 로직에서 모든 Grid 값을 초기화한 뒤에 로직이 동작하기 때문에
	//Grid의 갯수가 많아질 수록 이 계산이 오래걸림.

	//TArray<AUnit*> unitArr = GetAllUnitInGridSystem();
	
	TArray<AActor*> units;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnit::StaticClass(), units);
	TArray<AUnit*> enemyArr;
	TSet<FGrid> resultGrids;

	//적 유닛 조사
	for (auto i : units)
	{
		auto unit = CastChecked<AUnit>(i);
		if (!IsValid(unit))
		{
			continue;
		}

		if (unit->GetTeamType() == ETeamType::Team02)
		{
			enemyArr.Add(unit);
		}
	}

	//적 유닛의 AttackComponent를 통해 공격 가능한 위치를 전부 도출해낸다.
	//AttackComponent는 MoveComponent와 연동해서 이동 가능한 범위 전부 파악하고 공격 가능한 위치를 도출한다.
	for (auto enemy : enemyArr)
	{
		auto unitAttackComp = enemy->FindComponentByClass<UUnitAttackActionComponent>();
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
