// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitSelectPawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/KismetMathLibrary.h" //UKismetMathLibrary
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UnitCharacter.h"

#include "UnitMoveActionComponent.h"
#include "GridManager.h"
#include "Grid.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AUnitSelectPawn::AUnitSelectPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>("PawnMovement");
}

// Called when the game starts or when spawned
void AUnitSelectPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUnitSelectPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AUnitSelectPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AUnitSelectPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AUnitSelectPawn::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AUnitSelectPawn::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AUnitSelectPawn::LookUp);

	PlayerInputComponent->BindAction("UnitSelect", IE_Pressed, this, &AUnitSelectPawn::HandleSelectAction);

}

void AUnitSelectPawn::MoveForward(float Value)
{
	FVector cameraLocation;
	FRotator cameraRotation;
	GetController()->GetPlayerViewPoint(cameraLocation, cameraRotation);

	FRotator controlRot = cameraRotation;
	FVector temp = UKismetMathLibrary::GetForwardVector(controlRot);

	AddMovementInput(temp, Value);
}

void AUnitSelectPawn::MoveRight(float Value)
{
	FVector cameraLocation;
	FRotator cameraRotation;
	GetController()->GetPlayerViewPoint(cameraLocation, cameraRotation);

	FRotator controlRot = cameraRotation;
	FVector temp = UKismetMathLibrary::GetRightVector(controlRot);

	AddMovementInput(temp, Value);
}

void AUnitSelectPawn::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AUnitSelectPawn::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AUnitSelectPawn::HandleSelectAction()
{
	bool unitSelected = TryUnitSelect();

	if (unitSelected) // 유닛이 설정되었을 때
	{
		if (!IsValid(SelectedUnit))
		{
			UE_LOG(LogTemp, Warning, TEXT("true.. Selected Unit Not Valid"));
			return;
		}

		if (!IsValid(SelectedAction))
		{
			UE_LOG(LogTemp, Warning, TEXT("true.. Selected Action Not Valid"));
			return;
		}

		auto gridarray = SelectedAction->GetValidActionGridArray();
		AGridManager* gridManager = AGridManager::GetGridManager();
		if (IsValid(gridManager))
		{
			gridManager->RemoveAllGridVisual();
			//gridManager->ShowFromGridArray(gridarray, EGridVisualType::Warning);
			gridManager->ShowFromGridVisualDataArray(SelectedAction->GetValidActionGridVisualDataArray());
		}

	}
	else // 유닛 설정이 아니라 어떤 Action을 취하고 있을 때
	{
		if (!IsValid(SelectedUnit))
		{
			UE_LOG(LogTemp, Warning, TEXT("false.. Selected Unit Not Valid"));
			return;
		}

		if (!IsValid(SelectedAction))
		{
			UE_LOG(LogTemp, Warning, TEXT("false.. Selected Action Not Valid"));
			return;
		}

		APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (!IsValid(playerController))
		{
			UE_LOG(LogTemp, Warning, TEXT("false.. Player Controller Not Valid"));
			return;
		}

		FVector loc;
		FVector rot;
		playerController->DeprojectMousePositionToWorld(loc, rot);

		TArray<TEnumAsByte<EObjectTypeQuery>> objects;

		objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
		//objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
		TArray<AActor*> ignores;
		//ignores.Add(this);

		FHitResult hit;
		GetWorld()->LineTraceSingleByChannel(hit, loc, loc + rot * 1000, ECollisionChannel::ECC_Visibility);
		bool result = UKismetSystemLibrary::LineTraceSingleForObjects(
			GetWorld(),
			loc,
			loc + rot * 10000,
			objects,
			true,
			ignores,
			//EDrawDebugTrace::None,
			EDrawDebugTrace::ForDuration,
			hit,
			true,
			FLinearColor::Red,
			FLinearColor::Blue,
			5.0f
		);

		if (result)
		{
			FVector hitLocation = hit.Location;

			AGridManager* gridManager = AGridManager::GetGridManager();
			if (!IsValid(gridManager))
			{
				UE_LOG(LogTemp, Warning, TEXT("Action Failed Cause GridManager Can't be found"));
				return;
			}

			FGrid grid = gridManager->WorldToGrid(hitLocation);

			if (!gridManager->IsValidGrid(grid))
			{
				return;
			}

			SelectedAction->TakeAction(grid);
		}

	}


}

bool AUnitSelectPawn::TryUnitSelect()
{
	//UE_LOG(LogTemp, Warning, TEXT("UnitSelect()"));

	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!IsValid(playerController))
	{
		return false;
	}

	FVector loc;
	FVector rot;
	playerController->DeprojectMousePositionToWorld(loc, rot);

	TArray<TEnumAsByte<EObjectTypeQuery>> objects;

	objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

	//////objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	//////objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	////objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	////objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));


	TArray<AActor*> ignores;
	//ignores.Add(this);

	FHitResult hit;

	bool result = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		loc,
		loc + rot * 10000,
		objects,
		true,
		ignores,
		//EDrawDebugTrace::None,
		EDrawDebugTrace::ForDuration,
		hit,
		true,
		FLinearColor::Red,
		FLinearColor::Blue,
		5.0f
	);

	if (result)
	{
		FVector hitLocation = hit.Location;
		AGridManager* gridManager = AGridManager::GetGridManager();
		if (!IsValid(gridManager))
		{
			UE_LOG(LogTemp, Warning, TEXT("GridManager Not Valid"));
			return false;
		}

		FGrid hitGrid = gridManager->WorldToGrid(hitLocation);
		if (gridManager->IsValidGrid(hitGrid))
		{
			auto gridUnitArr = gridManager->GetUnitArrayAtGrid(hitGrid);
			if (gridUnitArr.Num() > 0 && gridUnitArr[0]->ActorHasTag(FName("MyUnit")))
			{
				UE_LOG(LogTemp, Warning, TEXT("Unit Set!"));
				SetSelectUnit(gridUnitArr[0]);
				return true;
			}
		}
		
	}

	UE_LOG(LogTemp, Warning, TEXT("Nothing Unit Detected"));
	return false;
}

void AUnitSelectPawn::SetSelectUnit(AUnitCharacter* Selected)
{
	if (IsValid(Selected) && SelectedUnit != Selected)
	{
		SelectedUnit = Selected;
		if (OnSelectedUnitChanged.IsBound())
		{
			OnSelectedUnitChanged.Broadcast();
		}
		
		if (IsValid(SelectedUnit->GetUnitActionComponent(EUnitActionType::Move)))
		{
			SelectedAction = SelectedUnit->GetUnitActionComponent(EUnitActionType::Move);
			if (OnSelectedActionChanged.IsBound())
			{
				OnSelectedActionChanged.Broadcast();
			}
			
			SelectedAction->OnActionStart;
			SelectedAction->OnActionEnd;
		}
	}
}

AUnitCharacter* AUnitSelectPawn::GetSelectedUnit()
{
	return SelectedUnit;
}

UUnitActionComponent* AUnitSelectPawn::GetSelectedAction()
{
	return SelectedAction;
}

AUnitSelectPawn* AUnitSelectPawn::GetUnitSelectPawn()
{
	if (!IsValid(GEngine) || GEngine->GameViewport.IsNull())
	{
		return nullptr;
	}

	UWorld* world = GEngine->GameViewport->GetWorld();
	AUnitSelectPawn* unitSelectPawn = Cast<AUnitSelectPawn>(UGameplayStatics::GetActorOfClass(world, AUnitSelectPawn::StaticClass()));

	return unitSelectPawn;
}

