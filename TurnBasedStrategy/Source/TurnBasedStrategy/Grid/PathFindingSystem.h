// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Grid.h"
#include "PathFindingSystem.generated.h"

class UPathObject;

/**
 * 
 */
UCLASS()
class TURNBASEDSTRATEGY_API UPathFindingSystem : public UObject
{
	GENERATED_BODY()

private:

	UPROPERTY()
	int32 X_Length;
	UPROPERTY()
	int32 Y_Length;

	//PathFinding Object
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PathFinding", Meta = (AllowPrivateAccess = true))
		TMap<FGrid, UPathObject*> PathObjectMap;
public:

	UPathFindingSystem();


	//Call Order in GridManager : NewObject<>(); -> SetPathFindingSystem();
	void SetPathFindingSystem(int _X_Length, int _Y_Length,
		TFunctionRef<UPathObject* (UPathFindingSystem*, FGrid)> CreateObjectFunction);

	TMap<FGrid, UPathObject*> GetPathObjectMap() const;
	UPathObject* GetValidPathObject(const FGrid& Grid) const;

};
