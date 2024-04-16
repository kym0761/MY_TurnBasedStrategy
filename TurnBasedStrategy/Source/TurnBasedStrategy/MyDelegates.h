// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyDelegates.generated.h"
/**
 * 
 */


//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnyUnitMoved);
//
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTurnManagingDelegate);
//
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGridManagingDelegate);
//
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTurnDelegate);
//
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonClickedCompleted);
//
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnitActionDelegate);
//
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnitControlDelegate);
//
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnitDelegateSignature);
//
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNotifyWorksSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicMulticastVoid);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynamicMulticastVoid_float, float, Val);

UCLASS()
class TURNBASEDSTRATEGY_API UMyDelegates : public UObject // dummyclass
{
	GENERATED_BODY()
};