// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

/**
 *
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()

public:
	AAuraCharacter();
	virtual void OnRep_PlayerState() override;
	virtual void PossessedBy(AController* NewController) override;

protected:
	void InitAbilityActorInfo();
	APlayerState* GetMyPlayerState() const { return GetPlayerState(); }

};
