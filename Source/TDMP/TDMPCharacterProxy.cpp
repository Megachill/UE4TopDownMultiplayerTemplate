// Fill out your copyright notice in the Description page of Project Settings.

#include "TDMP.h"
#include "TDMPCharacterProxy.h"
#include "TDMPCharacter.h"
#include "TDMPPlayerController.h"
#include "AIController.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"


// Sets default values
ATDMPCharacterProxy::ATDMPCharacterProxy(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComponent = GetCapsuleComponent();

	// don't rotate character tp camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// config character movement
	/*GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;*/

	// create camera boom
	CameraBoom = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 1000.f;
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// create camera
	TopDownCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // camera does not rotate relative to arm

	if (Role == ROLE_Authority)
	{
		static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/BP_TDCharacter"));
		//if (PlayerPawnBPClass.Succeeded()) 
		if (PlayerPawnBPClass.Class != NULL)
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayerPawnBPClass set"));
			CharacterClass = PlayerPawnBPClass.Class;
		}
	}

	// cursor to where the mouse pointer is -- NOTE COMMENTED OUT AS IT CURRENTLY CRASHES THE EDITOR!
	/*CursorToWorld = ObjectInitializer.CreateDefaultSubobject<UDecalComponent>(this, TEXT("CursorToWorld"));
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
		UE_LOG(LogTemp, Warning, TEXT("Cursor to World found"));
	}
	CursorToWorld->DecalSize = FVector(16.f, 32.f, 32.f);
	CursorToWorld->SetRelativeRotation(FRotator(90.f, 0.f, 0.f).Quaternion());*/

 	// activate ticking in order to update the cursor every frame
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

// Called when the game starts or when spawned
void ATDMPCharacterProxy::BeginPlay()
{
	Super::BeginPlay();
	
	if (Role == ROLE_Authority)
	{
		// get current location of player proxy
		FVector Location = GetActorLocation();
		FRotator Rotation = GetActorRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;
		SpawnParams.bNoFail = true;

		// spawn actual player
		Character = Cast<ATDMPCharacter>(GetWorld()->SpawnActor(CharacterClass, &Location, &Rotation, SpawnParams));

		// we use AI to control the player character for navigation
		PlayerAIController = GetWorld()->SpawnActor<AAIController>(GetActorLocation(), GetActorRotation());
		PlayerAIController->Possess(Character);
	}
}

// Called every frame
void ATDMPCharacterProxy::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (Character)
	{
		// keep proxy in sync with real character
		FTransform CharTransform = Character->GetTransform();
		FTransform MyTransform = GetTransform();

		FTransform Transform;
		// NOTE:: ScalarRegister(0.5f) is causing camera jitter / vibrations like crazy, hence this low value
		Transform.LerpTranslationScale3D(CharTransform, MyTransform, ScalarRegister(0.01f));
		SetActorTransform(Transform);
	}
}

void ATDMPCharacterProxy::MoveToLocation(const class ATDMPPlayerController* controller, const FVector& DestLocation)
{
	/**
	*  Engage AI to path find
	*  The AI will then "route" the character correctly.
	*  The Proxy (and with it the camera), on each tick, moves to the location of the real character
	*
	*  And thus, we get the illusion of moving with the Player Character
	*/
	PlayerAIController->MoveToLocation(DestLocation);
}

void ATDMPCharacterProxy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// replicated to everyone
	DOREPLIFETIME(ATDMPCharacterProxy, Character);
}
