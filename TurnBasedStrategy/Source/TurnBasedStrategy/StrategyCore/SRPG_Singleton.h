// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SRPG_Singleton.generated.h"

/**
 * 
 */
UCLASS()
class TURNBASEDSTRATEGY_API USRPG_Singleton : public UObject
{
	GENERATED_BODY()
	
public:

	USRPG_Singleton();

	static USRPG_Singleton* GetSRPG_Singleton();

};
