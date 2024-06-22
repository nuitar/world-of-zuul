// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WOZGameplayData.h"
#include "Engine/GameInstance.h"
#include "WOZGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFZUUL_API UWOZGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	static UWOZGameInstance* Get(const UObject* WorldContext);

public:
	int32 UserID = 0;
	FText Username;

	bool bIsNewGame = true;
	FWOZSaveGameData SinglePlayerSaveGameData;
};
