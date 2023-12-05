// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SRPG_GameMode.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGridManagingDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTestDele, int32, a, float, b);

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

	static ASRPG_GameMode* GetSRPG_GameMode();

#pragma region DebugControl
public:

	//true : 디버그 Log 출력 / false : 비출력
	//필요하면 GameMode의 블루프린트에서 해당 값을 바꿔서 디버그 로그 출력 조절.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Debug")
	bool bUseCustomDebug = true;
#pragma endregion


#pragma region WinLose

	UFUNCTION()
	void CheckWinOrLose();

	void WinGame();
	void LoseGame();


	/*이겼을 때와 졌을 때 UI가 나오는 임시 UI. 추후에 다른 것으로 변경될 수 있음.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WinLose")
	TSubclassOf<class UUserWidget> WBP_WinGame;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WinLose")
	TSubclassOf<class UUserWidget> WBP_LoseGame;

#pragma endregion

	FTestDele TestDelegate;

};
