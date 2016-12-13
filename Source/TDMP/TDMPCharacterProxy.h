// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "TDMPCharacterProxy.generated.h"

UCLASS()
class TDMP_API ATDMPCharacterProxy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATDMPCharacterProxy(const FObjectInitializer& ObjectInitializer);

	// Needed so we can pick up the class in the constructor and spawn it elsewhere 
	TSubclassOf<AActor> CharacterClass;

	// pointer to actual character
	UPROPERTY(Replicated)
	class ATDMPCharacter* Character;

	// AIController we will use
	class AAIController* PlayerAIController;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// used by controller to get the moving to work
	void MoveToLocation(const class ATDMPPlayerController* controller, const FVector& DestLocation);

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// top down camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* TopDownCameraComponent;

	// camera boom
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;
	
	// decal
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UDecalComponent* CursorToWorld;

	
};
