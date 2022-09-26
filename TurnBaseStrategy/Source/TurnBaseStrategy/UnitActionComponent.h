// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Grid.h"
#include "UnitActionComponent.generated.h"

class AUnitCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActionStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActionEnd);

UCLASS(abstract ,ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TURNBASESTRATEGY_API UUnitActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUnitActionComponent();

	FOnActionStart OnActionStart;
	FOnActionEnd OnActionEnd;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	AUnitCharacter* Unit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	int32 MaxActionRange;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bCanAction;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	virtual FString GetActionName() const;
	virtual void TakeAction(FGrid Grid);
	virtual bool IsValidActionGrid(FGrid Grid) const;
	virtual TArray<FGridVisualData> GetValidActionGridVisualDataArray() const;
	virtual TArray<FGrid> GetValidActionGridArray() const;
	AUnitCharacter* GetUnit() const;

};
