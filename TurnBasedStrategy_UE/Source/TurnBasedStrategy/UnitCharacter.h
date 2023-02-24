// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Grid/Grid.h"
#include "UnitAction/UnitAction.h"
#include "UnitCharacter.generated.h"

class UStatComponent;
class UUnitMoveActionComponent;
class UUnitActionComponent;
class UUnitAttackActionComponent;
class UUnitInteractActionComponent;
class UWaitActionComponent;
class ADamageTextActor;
//class UDamageTextWidget;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnitSpawned);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnitDead);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActionPlayed); //??


//Tag MyUnit , Enemy ??

UCLASS()
class TURNBASEDSTRATEGY_API AUnitCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AUnitCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		UStatComponent* StatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		UUnitMoveActionComponent* UnitMoveActionComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		UUnitAttackActionComponent* UnitAttackActionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		UUnitInteractActionComponent* UnitInteractActionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		UWaitActionComponent* WaitActionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TSubclassOf<ADamageTextActor> DamageTextActorBP;

	FOnUnitSpawned OnUnitSpawned;
	FOnUnitDead OnUnitDead;
	FOnActionPlayed OnActionPlayed;

private:

	//현재 Grid 위치 값
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	FGrid Grid;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	int32 ActionPoints = 1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay
	(
		const EEndPlayReason::Type EndPlayReason
	) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

	FGrid GetGrid();
	void SetGrid(FGrid GridValue);

	bool HasActionComponent(EUnitActionType UnitActionType);
	UUnitActionComponent* GetUnitActionComponent(EUnitActionType UnitActionType);

	UFUNCTION()
	void OnTurnChanged();

	UFUNCTION()
		void OnSelectedUnitChanged();


	bool IsThisUnitCanAction() const;


};
