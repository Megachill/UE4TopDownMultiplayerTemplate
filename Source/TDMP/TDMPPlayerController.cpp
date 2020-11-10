/**
 * This is the original TopDown Template Player Controller modified to be as simple as possible. 
 * If you like change etc and add things like VR / Touch back into it.
 * I have added the networking bit which is the one that tells the AIPlayerPawn to move
 */

#include "TDMPPlayerController.h"
#include "TDMP.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NavigationSystem.h"
#include "Components/DecalComponent.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "TDMPCharacter.h"
#include "TDMPCharacterProxy.h"

ATDMPPlayerController::ATDMPPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ATDMPPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
}

void ATDMPPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ATDMPPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ATDMPPlayerController::OnSetDestinationReleased);

	// support touch devices 
	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ATDMPPlayerController::MoveToTouchLocation);
	//InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ATDMPPlayerController::MoveToTouchLocation);

	//InputComponent->BindAction("ResetVR", IE_Pressed, this, &ATDMPPlayerController::OnResetVR);
}

/*void ATDMPPlayerController::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}*/

/*
void ATDMPPlayerController::MoveToMouseCursor()
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		if (ATDMPCharacter* MyPawn = Cast<ATDMPCharacter>(GetPawn()))
		{
			if (MyPawn->GetCursorToWorld())
			{
				UNavigationSystem::SimpleMoveToLocation(this, MyPawn->GetCursorToWorld()->GetComponentLocation());
			}
		}
	}
	else
	{
		// Trace to see what is under the mouse cursor
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		if (Hit.bBlockingHit)
		{
			// We hit something, move there
			SetNewMoveDestination(Hit.ImpactPoint);
		}
	}
}*/

void ATDMPPlayerController::MoveToMouseCursor()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_PhysicsBody, false, Hit);

	if (Hit.bBlockingHit)
	{
		if (Hit.GetActor()->IsA(ATDMPCharacter::StaticClass()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Actor, %s"), *GetDebugName(Hit.GetActor()));
		}
		// We hit something, move there
		SetNewMoveDestination(Hit.ImpactPoint);
	}
}

/*void ATDMPPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}*/

void ATDMPPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	/*APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if (NavSys && (Distance > 120.0f))
		{
			NavSys->SimpleMoveToLocation(this, DestLocation);
		}
	}*/
	// rewired with server move below
	ServerSetNewMoveDestination(DestLocation);
}

void ATDMPPlayerController::ServerSetNewMoveDestination_Implementation(const FVector DestLocation)
{
	ATDMPCharacterProxy* const Pawn = Cast<ATDMPCharacterProxy>(GetPawn());
	if (Pawn)
	{
		UNavigationSystemV1* const NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		float const Distance = FVector::Dist(DestLocation, Pawn->GetActorLocation());

		// issue move command
		if (NavSys && (Distance > 120.f))
		{
			Pawn->MoveToLocation(this, DestLocation);
		}
	}
}

bool ATDMPPlayerController::ServerSetNewMoveDestination_Validate(const FVector DestLocation)
{
	return true;
}

void ATDMPPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void ATDMPPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}
