// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Grid.h"
#include "UnitCharacter.generated.h"

class UStatComponent;
class UUnitMoveActionComponent;
UCLASS()
class TURNBASESTRATEGY_API AUnitCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AUnitCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		UStatComponent* StatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		UUnitMoveActionComponent* UnitMoveActionComponent;

private:

	//현재 Grid 위치 값
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	FGrid Grid;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	int32 ActionPoints = 1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser);

	FGrid GetGrid();
	void SetGrid(FGrid GridValue);
};
