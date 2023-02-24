// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageTextActor.generated.h"

class UWidgetComponent;
class UDamageTextWidget;



UCLASS()
class TURNBASEDSTRATEGY_API ADamageTextActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADamageTextActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DamageText")
		TObjectPtr<UWidgetComponent> WidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageText")
		TSubclassOf<UDamageTextWidget> DamageTextClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DamageText")
		float AppliedDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageText", Meta = (ClampMin = 0.0f, ClampMax = 200.0f))
		float Speed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
