// Fill out your copyright notice in the Description page of Project Settings.


#include "GridManager.h"
#include "Grid/GridObject.h"
#include "Grid/PathNode.h"
#include "Grid/InstancedGridVisualComponent.h"
#include "Grid/PathFindingSystem.h"
#include "Grid/GridSystem.h"

#include "Components/BillboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UnitCore/Unit.h"

#include "UnitAction/UnitAttackActionComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "SRPG_GameMode.h"

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

}

// Called every frame
void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGridManager::Setup()
{
	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
	if (IsValid(gameMode))
	{
		SRPG_GameModeRef = gameMode;
		SRPG_GameModeRef->SetupGridManaging(this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode is Invalid."));
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

	if (!IsValid(SRPG_GameModeRef))
	{
		return;
	}

	TSet<FGrid> gridSet;

	for (int32 x = -Range; x <= Range; x++)
	{
		for (int32 y = -Range; y <= Range; y++)
		{
			FGrid temp = Grid + FGrid(x, y);

			if (!SRPG_GameModeRef->IsValidGrid(temp))
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

	if (!IsValid(SRPG_GameModeRef))
	{
		return;
	}

	GridVisual_DANGER->ClearInstances();
	
	TArray<AUnit*> unitArr = SRPG_GameModeRef->GetAllUnitInGridSystem();
	TArray<AUnit*> enemyArr;
	TSet<FGrid> resultGrids;

	for (auto unit : unitArr)
	{
		if (!IsValid(unit))
		{
			continue;
		}

		if (unit->ActorHasTag(ENEMY))
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
