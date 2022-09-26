// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "UnitAction.h"
#include "UnitSelectPawn.generated.h"

class UFloatingPawnMovement;
class AUnitCharacter;
class UUnitActionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectedUnitChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectedActionChanged);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActionStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBusyChanged, bool, bIsBusy);

UCLASS()
class TURNBASESTRATEGY_API AUnitSelectPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AUnitSelectPawn();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		UFloatingPawnMovement* PawnMovement;

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

	bool TryUnitSelect();

	void SetSelectUnit(AUnitCharacter* Selected);

	UFUNCTION(BlueprintCallable)
	AUnitCharacter* GetSelectedUnit();
	UFUNCTION(BlueprintCallable)
	UUnitActionComponent* GetSelectedAction();

	static AUnitSelectPawn* GetUnitSelectPawn();
};
