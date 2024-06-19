// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/WOZGameInstance.h"
#include "Kismet/GameplayStatics.h"

UWOZGameInstance* UWOZGameInstance::Get(const UObject* WorldContext)
{
	if (!WorldContext) return nullptr;
	return Cast<UWOZGameInstance>(UGameplayStatics::GetGameInstance(WorldContext));
}
