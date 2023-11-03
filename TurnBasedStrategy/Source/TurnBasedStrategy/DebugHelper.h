// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#define LOG_NETMODEINFO (GetNetMode() == ENetMode::NM_Client) ? *FString::Printf(TEXT("Client %d"), GPlayerInEditorID) : (GetNetMode() == ENetMode::NM_Standalone) ? TEXT("Standalone") : TEXT("Server")

//예시) AActor::BeginPlay()
#define FUNCTION_NAME (TEXT(" / Function Name : ") + FString(__FUNCTION__)+TEXT(" "))

//예시) AActor::Func에서 ::Func이 빠져서 AActor만 나옴.
#define CURRENT_CLASS (TEXT(" / Class Name : ") + FString(__FUNCTION__).Left(FString(__FUNCTION__).Find(TEXT(":"))) + TEXT(" "))

//함수의 라인만 나옴
#define CURRENT_LINE  (TEXT(" / Line : ") + FString::FromInt(__LINE__) + TEXT(" "))

//예시) void __cdecl AActor::BeginPlay(void) 형식으로 출력됨.
#define CURRENT_FUNCSIG (TEXT(" / Func signature : ") +FString(__FUNCSIG__)+ TEXT(" "))
#define DEBUG_TEXT_POSITION (FUNCTION_NAME + CURRENT_LINE)

//param0 꼭 string으로 할 것.   예시) DEBUG_TEXT("abc")
#define DEBUG_TEXT(Param0) (TEXT(Param0) + DEBUG_TEXT_POSITION)

#define DTEXT_ARGS(Format, ...)  *FString::Printf(TEXT("%s / %s / Line : %d"),*FString::Printf(Format, ##__VA_ARGS__), ANSI_TO_TCHAR(__FUNCTION__), __LINE__)

#define LOG_EXAMPLE(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("%s %d %s"), ANSI_TO_TCHAR(__FUNCTION__), __LINE__, *FString::Printf(Format, ##__VA_ARGS__))

DECLARE_LOG_CATEGORY_EXTERN(LogStrategy, Log, All);

/**
 * 
 */

//Debug 출력 핸들 : true -> 출력 / false -> 비출력
//!주의 -> 게임모드의 프로퍼티로 빌드를 매번 새로하지 않고 Debug를 활성화하게 만듬.
namespace DebugControl
{
	static bool bOK_Debugging = true;
}

namespace Debug
{

	static void Print(const FString& Msg, const FColor& Color = FColor::MakeRandomColor(), int32 InKey = -1)
	{
		if (!DebugControl::bOK_Debugging)
		{
			return;
		}

		//if (GEngine)
		//{
		//	GEngine->AddOnScreenDebugMessage(InKey, 3.0f, Color, Msg);
		//}

		UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
	}

}