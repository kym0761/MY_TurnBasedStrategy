// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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

/**
 * 
 */

//Debug 출력 핸들 : true -> 출력 / false -> 비출력
namespace DebugChecker
{
	static bool bOK_Debugging = false;
}

namespace Debug
{

	static void Print(const FString& Msg, const FColor& Color = FColor::MakeRandomColor(), int32 InKey = -1)
	{
		if (!DebugChecker::bOK_Debugging)
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