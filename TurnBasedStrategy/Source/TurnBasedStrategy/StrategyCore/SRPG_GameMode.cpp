// Fill out your copyright notice in the Description page of Project Settings.


#include "SRPG_GameMode.h"

#include "Kismet/GameplayStatics.h"

#include "DebugHelper.h"

ASRPG_GameMode::ASRPG_GameMode()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ASRPG_GameMode::BeginPlay()
{
	Super::BeginPlay();

	//DebugHelper.h에 존재하는 디버그 활성/ 비활성 Flag다.
	DebugControl::bOK_Debugging = bUseCustomDebug;
	Debug::Print(DEBUG_TEXT("한글 출력 테스트."));
}

void ASRPG_GameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

}

void ASRPG_GameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

ASRPG_GameMode* ASRPG_GameMode::GetSRPG_GameMode()
{

	if (!IsValid(GEngine) || !IsValid(GEngine->GameViewport))
	{
		return nullptr;
	}

	UWorld* world = GEngine->GameViewport->GetWorld();

	ASRPG_GameMode* gameMode = Cast<ASRPG_GameMode>(UGameplayStatics::GetActorOfClass(world, ASRPG_GameMode::StaticClass()));

	return gameMode;
}

void ASRPG_GameMode::WinGame()
{
	Debug::Print(DEBUG_TEXT("WIN GAME"));
}

void ASRPG_GameMode::LoseGame()
{
	Debug::Print(DEBUG_TEXT("LOSE GAME"));
}