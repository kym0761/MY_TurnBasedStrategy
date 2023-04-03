// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatComponent.generated.h"

/*
* STR : 물리 공격력
* INT : 마법 공격력
* SPD : 스피드
* SKL : 기술
* LUK : 행운
* DEF : 물리 방어력
* RES : 마법 방어력
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TURNBASEDSTRATEGY_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatComponent();

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		int32 HP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		int32 MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		int32 STR;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		int32 INT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		int32 SPD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		int32 SKL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		int32 LUK;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		int32 DEF;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		int32 RES;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	float DealDamage(float DamageAmount);

	float GetHP() const;
	float GetMaxHP() const;
	int32 GetSTR() const;
	int32 GetINT() const;
	int32 GetSPD() const;
	int32 GetSKL() const;
	int32 GetLUK() const;
	int32 GetDEF() const;
	int32 GetRES() const;
};
