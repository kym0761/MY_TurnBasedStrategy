// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainCanvasWidget.generated.h"

class UCanvasPanel;

/**
 * 
 */
UCLASS(abstract)
class TURNBASEDSTRATEGY_API UMainCanvasWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		UCanvasPanel* MainCanvas;

};
