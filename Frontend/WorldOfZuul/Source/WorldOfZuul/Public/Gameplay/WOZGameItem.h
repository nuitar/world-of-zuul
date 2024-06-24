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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	static AWOZGameItem* CreateItem(UObject* WorldContext, EWOZGameItem::Type ItemEnum, const FIntPoint& Position, UWOZGameplayData* GameplayData);
	void UpdateItem(EWOZGameItem::Type InItemEnum);
	
	TEnumAsByte<EWOZGameItem::Type> GetItemEnum() const { return ItemEnum; }
	const FIntPoint& GetPosition() const { return Position; }

private:
	UFUNCTION()
	void OnRep_ItemEnum();
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Plane;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(EditAnywhere, Category = WOZ)
	TObjectPtr<UWOZGameplayData> GameplayData;
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MaterialInstanceDynamic;
	
	FIntPoint Position;

	UPROPERTY(ReplicatedUsing = "OnRep_ItemEnum")
	TEnumAsByte<EWOZGameItem::Type> ItemEnum = EWOZGameItem::None;
};
