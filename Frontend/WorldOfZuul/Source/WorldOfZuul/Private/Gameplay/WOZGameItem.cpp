// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/WOZGameItem.h"
#include "Components/SphereComponent.h"
#include "Gameplay/WOZGameplayData.h"

AWOZGameItem::AWOZGameItem()
{
	PrimaryActorTick.bCanEverTick = false;

	Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	SetRootComponent(Plane);
	Plane->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());
}



AWOZGameItem* AWOZGameItem::CreateItem(UObject* WorldContext, EWOZGameItem::Type ItemEnum, const FIntPoint& Position, UWOZGameplayData* GameplayData)
{
	check(WorldContext && GameplayData && GameplayData->ItemClass);
	const FVector& SpawnLocation = FVector(0.0, 0.0, 1000.0);
	AWOZGameItem* Item = Cast<AWOZGameItem>(WorldContext->GetWorld()->SpawnActor(GameplayData->ItemClass, &SpawnLocation));
	check(Item);
	
	UMaterialInterface* Material = Item->Plane->GetMaterial(0);
	if (Material)
	{
		Item->MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(Material, Item);
		Item->Plane->SetMaterial(0,Item->MaterialInstanceDynamic);
	}

	Item->SetActorScale3D(FVector(GameplayData->ItemSize / 100.f));
	Item->Position = Position;
	Item->UpdateItem(ItemEnum, GameplayData);

	return Item;
}

void AWOZGameItem::UpdateItem(EWOZGameItem::Type ItemEnum, UWOZGameplayData* GameplayData)
{
	check(GameplayData && MaterialInstanceDynamic);
	Item = ItemEnum;

	const FWOZGameItemInfo& ItemInfo = GameplayData->Items.FindRef(Item);
	MaterialInstanceDynamic->SetTextureParameterValue(FName("Texture"), ItemInfo.Icon);
}