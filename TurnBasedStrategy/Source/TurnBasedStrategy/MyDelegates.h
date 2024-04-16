// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyDelegates.generated.h"
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicMulticastVoid);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynamicMulticastVoid_float, float, Val);

UCLASS()
class TURNBASEDSTRATEGY_API UMyDelegates : public UObject // dummyclass
{
	GENERATED_BODY()
};