// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Grid/Grid.h"
#include "UnitActionComponent.generated.h"

class AUnitCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActionStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActionEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActionSelected);

UCLASS(abstract ,ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TURNBASEDSTRATEGY_API UUnitActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUnitActionComponent();

	FOnActionStart OnActionStart;
	FOnActionEnd OnActionEnd;
	FOnActionSelected OnActionSelected;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	AUnitCharacter* Unit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	int32 MaxActionRange;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bCanDoActionThisTurn;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	FString ActionName;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	FString GetActionName() const;

	virtual void DealWithGridBeforeAction(const FGrid& Grid);

	virtual void TakeAction(const FGrid& Grid);
	virtual bool IsValidActionGrid(const FGrid& Grid) const;
	virtual TArray<FGridVisualData> GetValidActionGridVisualDataArray() const;
	virtual TArray<FGrid> GetValidActionGridArray() const;
	AUnitCharacter* GetUnit() const;

	bool IsCanDoActionThisTurn() const;
	void SetCanDoActionThisTurn(bool InputBool);
	bool IsCurrentlyAvailableAction() const;

	UFUNCTION()
	virtual void OnActionStartFunc();
	UFUNCTION()
	virtual void OnActionEndFunc();
	UFUNCTION()
	virtual void OnActionSelectedFunc();
};
