// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Turn.h"
#include "Grid/Grid.h"
#include "GameFramework/GameModeBase.h"
#include "SRPG_GameMode.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGridManagingDelegate);

/**
 * 
 */
UCLASS()
class TURNBASEDSTRATEGY_API ASRPG_GameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	ASRPG_GameMode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	static ASRPG_GameMode* GetSRPG_GameMode(const UObject* WorldContextObject);

#pragma region DebugControl
public:

	//true : 디버그 Log 출력 / false : 비출력
	//필요하면 GameMode의 블루프린트에서 해당 값을 바꿔서 디버그 로그 출력 조절.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Debug")
	bool bUseCustomDebug = true;
#pragma endregion


#pragma region WinLose

	void WinGame();
	void LoseGame();


#pragma endregion

};
