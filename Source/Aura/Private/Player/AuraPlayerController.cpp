// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;

}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();


}


void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(AuraContext, TEXT("Set AuraContext in Blueprints")))
	{
		UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

		check(Subsystem);

		Subsystem->AddMappingContext(AuraContext, 0);

		bShowMouseCursor = true;
		DefaultMouseCursor = EMouseCursor::Default;

		FInputModeGameAndUI InputModeData;
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputModeData.SetHideCursorDuringCapture(false);
		SetInputMode(InputModeData);
	}
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent *EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

void AAuraPlayerController::Move(const FInputActionValue &InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn *ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	if (!CursorHit.bBlockingHit) return;
	LastActor = ThisActor;
	ThisActor = CursorHit.GetActor();


	/*
	* Line trace from cusror. There are several scenarios:
	* a. LastActor is null && This actor is null
	*	- do nothing or return -- DONE [else]
	* b. LastActor is null && ThisActor is valid
	*	- call HighlightActor on ThisActor -- DONE
	* c. LastActor is valid && ThisActor is null
	*	- Call UnHighlightActor on LastActor -- DONE
	* d. LastActor is Valid && ThisActor is valid and lastactor != thisactor
	*	- call UnhighlightActor on LastActor and HighlightActor on ThisActor -- DONE
	* e. LastActor is Valid && ThisActor is valid and lastactor == thisactor
	*	do nothing -- DONE [else]
	*/

	if (LastActor.GetInterface() == nullptr)
	{
		if (ThisActor.GetInterface())
		{
			// case b.
			ThisActor->HighlightActor();
		}
		else
		{
			// ThisActor is null
		}
	}
	else //LastActor is valid
	{
		if (ThisActor.GetInterface() == nullptr)
		{
			// case c
			LastActor->UnHighlightActor();
		}
		else
		{
			// both actors are valid
			if (LastActor.GetInterface() != ThisActor.GetInterface())
			{
				// case d
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();
			}
		}
	}

}
