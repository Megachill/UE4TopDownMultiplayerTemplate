// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "TDMPGameMode.h"
#include "TDMP.h"
#include "TDMPPlayerController.h"
#include "TDMPCharacter.h"

ATDMPGameMode::ATDMPGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ATDMPPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/BP_TDCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}