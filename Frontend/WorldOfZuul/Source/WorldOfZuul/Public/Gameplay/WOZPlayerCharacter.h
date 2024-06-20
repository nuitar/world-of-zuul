// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WOZPlayerCharacter.generated.h"

class UWOZGameplayData;
class UFloatingPawnMovement;
struct FInputActionValue;

UCLASS()
class WORLDOFZUUL_API AWOZPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AWOZPlayerCharacter();

	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Plane;
	
	UPROPERTY(EditAnywhere, Category = "WOZ")
	TObjectPtr<UWOZGameplayData> GameplayData;
	
	UPROPERTY(EditAnywhere, Category = "WOZ")
	TObjectPtr<UTexture> AvatarTexture;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MaterialInstanceDynamic;
};
