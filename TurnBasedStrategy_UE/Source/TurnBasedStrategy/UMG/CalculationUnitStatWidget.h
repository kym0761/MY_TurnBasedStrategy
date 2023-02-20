// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CalculationUnitStatWidget.generated.h"

class UProgressBar;
class UTextBlock;

class UStatComponent;

/**
 * 
 */
UCLASS()
class TURNBASEDSTRATEGY_API UCalculationUnitStatWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		UProgressBar* ProgressBar_HP;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		UTextBlock* TextBlock_HP;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		UTextBlock* TextBlock_DMG;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		UTextBlock* TextBlock_Hit;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		UTextBlock* TextBlock_Crit;

	UPROPERTY()
		UStatComponent* CurrentStatComponent;
public:

	void InitCalculationUnitStat(AActor* ActorHasStatComponent);
	void UpdateUnitStatWidget();
};
