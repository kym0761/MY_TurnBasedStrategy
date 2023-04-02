// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Grid/Grid.h"
#include "InputActionValue.h"
#include "Turn/Turn.h"
#include "UnitControlPawn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnitControlDelegate);

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;
class UFloatingPawnMovement;
class USceneComponent;
class UUnitActionComponent;
class UUnitActionListWidget;
class UMainCanvasWidget;
class AUnitCharacter;

/*
* Player가 사용할 UnitControlPawn.
*/

UENUM(BlueprintType)
enum class EPawnMode : uint8
{
	Selection UMETA(DisplayName = "Selection"),
	Action UMETA(DisplayName = "Action"),
	UI UMETA(DisplayName = "UI"),
	Busy UMETA(DisplayName = "Busy")
};

UENUM(BlueprintType)
enum class EPawnDoing : uint8
{
	Nothing UMETA(DisplayName = "Nothing"),
	Moving UMETA(DisplayName = "Moving"),
	Attacking UMETA(DisplayName = "Attacking")
};


UCLASS(abstract)
class TURNBASEDSTRATEGY_API AUnitControlPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AUnitControlPawn();

	FUnitControlDelegate OnSelectedActionChanged;
	FUnitControlDelegate OnSelectedUnitChanged;

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UMG")
		TSubclassOf<UUnitActionListWidget> UnitActionListWidgetClass;

	UPROPERTY()
		UUnitActionListWidget* UnitActionsWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UMG")
		TSubclassOf<UMainCanvasWidget> UMainCanvasWidgetClass;

	UPROPERTY()
		UMainCanvasWidget* MainCanvasWidget;

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputMappingContext* InputMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
		UInputAction* InputGridMove;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
		UInputAction* ControlEnter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USceneComponent* PawnRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component|Camera")
		UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component|Camera")
		USpringArmComponent* CameraBoom;

protected:

	//Start Position
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SRPG", Meta = (AllowPrivateAccess = true))
		FGrid StartGrid;

	//Current Position
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SRPG", Meta = (AllowPrivateAccess = true))
		FGrid PivotGrid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SRPG", Meta = (AllowPrivateAccess = true))
		float MoveInterval = 0.25f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SRPG", Meta = (AllowPrivateAccess = true))
		float MoveAccumulate = 0.0f;

	UPROPERTY()
		bool bIsBusy;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		AUnitCharacter* SelectedUnit;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		UUnitActionComponent* SelectedAction;

	//플레이어 Pawn이 현재 어떤 모드인지 따라서 선택할 때의 행동이 달라짐
	//Selection : Enter로 UnitSelect가능
	//Action : 선택된 유닛과 Action에 대한 행동을 함.
	//UI : Enter로 UI 조작
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SRPG", Meta = (AllowPrivateAccess = true))
		EPawnMode PawnMode = EPawnMode::Selection;

	//어떤 턴에 반응할 것인지 나타냄.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		ETurnType PawnTurnType = ETurnType::PlayerTurn;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void InitGridPosition(const FGrid& Grid);

	UFUNCTION()
	void GridMove(const FInputActionValue& Val);
	UFUNCTION()
	void GridMoveStart(const FInputActionValue& Val);
	UFUNCTION()
	void GridMoveEnd(const FInputActionValue& Val);
	UFUNCTION()
	void HandleControlEnter(const bool& Val);

	bool TryUnitSelect();
	void SetSelectedUnit(AUnitCharacter* InputUnit);
	void SetSelectedAction(UUnitActionComponent* InputUnitAction);

	void DoSelection();
	void DoDeselection();
	void DoAction();

	void SetControlPawnMode(EPawnMode ModeInput);
	void SetBusyOrNot(bool BusyInput);

	UFUNCTION()
		virtual void OnUnitActionCompleted();

	void FindAllPlayerUnits();

};
