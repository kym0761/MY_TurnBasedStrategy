// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitActionComponent.h"
#include "UnitMoveActionComponent.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartMoving);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndMoving);

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TURNBASESTRATEGY_API UUnitMoveActionComponent : public UUnitActionComponent
{
	GENERATED_BODY()

public:
		UUnitMoveActionComponent();

		//FOnStartMoving OnStartMoving;
		//FOnEndMoving OnEndMoving;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		FGrid Destination;
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		bool bMoveActivate = false;
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		TArray<FGrid> Path;

		bool bIsMoving = false;
protected:

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

private: 

public:

	virtual TArray<FGrid> GetValidActionGridArray() const override;
	virtual TArray<FGridVisualData> GetValidActionGridVisualDataArray() const;
	virtual void TakeAction(FGrid Grid) override;
};
