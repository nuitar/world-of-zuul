// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WOZGameplayData.h"
#include "GameFramework/Actor.h"
#include "WOZGameItem.generated.h"

class USphereComponent;
class AWOZPlayerCharacter;

UCLASS()
class WORLDOFZUUL_API AWOZGameItem : public AActor
{
	GENERATED_BODY()

public:
	AWOZGameItem();

	static AWOZGameItem* CreateItem(UObject* WorldContext, EWOZGameItem::Type ItemEnum, const FIntPoint& Position, UWOZGameplayData* GameplayData);
	void UpdateItem(EWOZGameItem::Type ItemEnum, UWOZGameplayData* GameplayData);
	
	TEnumAsByte<EWOZGameItem::Type> GetItemEnum() const { return Item; }
	const FIntPoint& GetPosition() const { return Position; }

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Plane;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MaterialInstanceDynamic;
	
	FIntPoint Position;
	TEnumAsByte<EWOZGameItem::Type> Item = EWOZGameItem::None;
};
