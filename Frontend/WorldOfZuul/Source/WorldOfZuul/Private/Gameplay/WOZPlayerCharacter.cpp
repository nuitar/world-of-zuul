// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/WOZPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Gameplay/WOZGameplayData.h"

AWOZPlayerCharacter::AWOZPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	Plane->SetupAttachment(GetRootComponent());
	Plane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWOZPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	check(GameplayData);

	GetCapsuleComponent()->SetCapsuleRadius(GameplayData->PlayerSize / 2.f);
	Plane->SetWorldScale3D(FVector(GameplayData->PlayerSize / 100.f));

	UMaterialInterface* Material = Plane->GetMaterial(0);
	if (Material)
	{
		MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(Material, this);
		Plane->SetMaterial(0,MaterialInstanceDynamic);
	}

	MaterialInstanceDynamic->SetTextureParameterValue(FName("Texture"), AvatarTexture);
}
