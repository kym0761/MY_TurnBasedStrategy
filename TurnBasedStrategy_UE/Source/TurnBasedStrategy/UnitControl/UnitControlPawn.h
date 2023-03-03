// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Grid/Grid.h"
#include "InputActionValue.h"
#include "UnitControlPawn.generated.h"

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

UENUM(BlueprintType)
enum class EPawnMode : uint8
{
	Selection UMETA(DisplayName = "Selection"),
	Action UMETA(DisplayName = "Action"),
	UI UMETA(DisplayName = "UI"),
	Busy UMETA(DisplayName = "Busy")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnitControlDelegate);


UCLASS(abstract)
class TURNBASEDSTRATEGY_API AUnitControlPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AUnitControlPawn();

	//Start Position
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SRPG")
		FGrid StartGrid;

	//Current Position
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SRPG")
		FGrid PivotGrid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SRPG")
	float MoveInterval = 0.25f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SRPG")
	float MoveAccumulate = 0.0f;

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SRPG")
	//	EPawnMode PawnMode = EPawnMode::Selection;

	UPROPERTY()
		bool bIsBusy;

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

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SRPG", Meta = (AllowPrivateAccess = true))
		EPawnMode PawnMode = EPawnMode::Selection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		AUnitCharacter* SelectedUnit;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		UUnitActionComponent* SelectedAction;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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
	void DoAction();

	void SetControlPawnMode(EPawnMode ModeInput);

};
