// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/WOZGameItem.h"
#include "Components/SphereComponent.h"
#include "Gameplay/WOZGameplayData.h"
#include "Net/UnrealNetwork.h"

AWOZGameItem::AWOZGameItem()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicatingMovement(true);

	Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	SetRootComponent(Plane);
	Plane->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());
}

void AWOZGameItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWOZGameItem, ItemEnum);
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
	Item->UpdateItem(ItemEnum);

	return Item;
}

void AWOZGameItem::UpdateItem(EWOZGameItem::Type InItemEnum)
{
	if (!MaterialInstanceDynamic)
	{
		UMaterialInterface* Material = Plane->GetMaterial(0);
		if (Material)
		{
			MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(Material, this);
			Plane->SetMaterial(0,MaterialInstanceDynamic);
		}
	}
	
	check(GameplayData && MaterialInstanceDynamic);
	ItemEnum = InItemEnum;

	const FWOZGameItemInfo& ItemInfo = GameplayData->Items.FindRef(InItemEnum);
	MaterialInstanceDynamic->SetTextureParameterValue(FName("Texture"), ItemInfo.Icon);
}

void AWOZGameItem::OnRep_ItemEnum()
{
	UpdateItem(ItemEnum);
}
