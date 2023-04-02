// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "UnitAction/UnitAction.h"
#include "Grid/Grid.h"
#include "UnitControlPawn.h"
#include "UnitSelectPawn.generated.h"

class UFloatingPawnMovement;
class AUnitCharacter;
class UUnitActionComponent;
class UUnitActionListWidget;
class UMainCanvasWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectedUnitChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectedActionChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBusyChanged, bool, bIsBusy);

/*이 Pawn은 wasd로 3D 공간을 이동하고 마우스 클릭으로 유닛을 선택하는 테스트 Pawn
현재 사용하지 않는다. UnitControlPawn으로 완전히 대체할 예정.
*/

UCLASS(abstract)
class TURNBASEDSTRATEGY_API AUnitSelectPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AUnitSelectPawn();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		UFloatingPawnMovement* PawnMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UMG")
		TSubclassOf<UUnitActionListWidget> UnitActionListWidgetClass;

	UPROPERTY()
	UUnitActionListWidget* UnitActionsWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UMG")
		TSubclassOf<UMainCanvasWidget> UMainCanvasWidgetClass;

	UPROPERTY()
	UMainCanvasWidget* MainCanvasWidget;

	UPROPERTY()
		bool bIsBusy;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="SRPG")
		EPawnMode PawnMode = EPawnMode::Selection;

	FTimerHandle TraceTimer;
	FGrid CurrentTracedGrid;

	FOnSelectedActionChanged OnSelectedActionChanged;
	FOnSelectedUnitChanged OnSelectedUnitChanged;
	FOnBusyChanged OnBusyChanged;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	AUnitCharacter* SelectedUnit;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	UUnitActionComponent* SelectedAction;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	//Movement Functions
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);

	void HandleSelectAction();

	bool TraceToGrid(FHitResult &OutHit);
	void OnTracedGridChanged();

	void DoSelection();
	void DoAction();

	void SetPawnMode(EPawnMode InputMode);


	bool TryUnitSelect();

	void SetSelectUnit(AUnitCharacter* InputUnit);
	void SetSelectedAction(UUnitActionComponent* InputUnitAction);

	UFUNCTION(BlueprintCallable)
		AUnitCharacter* GetSelectedUnit();
	UFUNCTION(BlueprintCallable)
		UUnitActionComponent* GetSelectedAction();

	static AUnitSelectPawn* GetUnitSelectPawn();

	bool GetIsBusy() const;
	void SetIsBusy(bool InputBool);

	void DeSelect();

	UFUNCTION()
		void OnSelectedActionChangedFunc();
	UFUNCTION()
		void OnSelectedUnitChangedFunc();
	UFUNCTION()
		void OnBusyChangedFunc(bool InputBool);

	UFUNCTION()
		void UnitLook();

};
