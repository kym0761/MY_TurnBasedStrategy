// Fill out your copyright notice in the Description page of Project Settings.


#include "SRPG_Singleton.h"
#include "DebugHelper.h"

USRPG_Singleton::USRPG_Singleton()
{
}

USRPG_Singleton* USRPG_Singleton::GetSRPG_Singleton()
{
	// TODO: insert return statement here

	if (!IsValid(GEngine))
	{
		Debug::Print(DEBUG_TEXT("GEngine is Invalid"));
	}

	USRPG_Singleton* singleton = CastChecked<USRPG_Singleton>(GEngine->GameSingleton);

	if (!IsValid(singleton))
	{
		Debug::Print(DEBUG_TEXT("singleton is Invalid"));
		return nullptr;
	}

	return singleton;
}
