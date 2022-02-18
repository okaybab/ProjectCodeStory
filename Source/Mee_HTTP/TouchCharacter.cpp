// Fill out your copyright notice in the Description page of Project Settings.


#include "TouchCharacter.h"

// Sets default values
ATouchCharacter::ATouchCharacter()
{
	
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	IsTouchPress = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));


	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Chr_Gym_Male(TEXT("SkeletalMesh'/Game/PolygonShops/Meshes/Characters/SK_Chr_Gym_Male_01.SK_Chr_Gym_Male_01'"));

	if (SK_Chr_Gym_Male.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_Chr_Gym_Male.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> TOUCHMAN_ANIM(TEXT("AnimBlueprint'/Game/PolygonShops/Animation/Poly_AnimBP.Poly_AnimBP_C'"));

	if (TOUCHMAN_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(TOUCHMAN_ANIM.Class);
	}

	SetControlMode(0);
}

// Called when the game starts or when spawned
void ATouchCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATouchCharacter::SetControlMode(int32 ControlMode)
{
	
	if (ControlMode == 0)
	{
		SpringArm->TargetArmLength = 450.0f;
		SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;
		SpringArm->bDoCollisionTest = true;
		bUseControllerRotationYaw = false;
	}

}

// Called every frame
void ATouchCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsTouchPress) 
	{
		FVector2D VectorMinusResult = TouchEnd - TouchStart;
		float InputRotationYaw, InputRotationPitch;
		InputRotationYaw = VectorMinusResult.X * DeltaTime * 0.5;
		InputRotationPitch = VectorMinusResult.Y * DeltaTime * -0.5;

		FRotator AddRelRotator = FRotator(InputRotationPitch, InputRotationYaw, 0.0f);
		FRotator SetRelRotator = FRotator(SpringArm->GetRelativeRotation().Pitch, SpringArm->GetRelativeRotation().Yaw, 0.0f);

		if (SpringArm->GetRelativeRotation().Pitch > -85.0f && SpringArm->GetRelativeRotation().Pitch < 22.0f )
		{
			SpringArm->AddRelativeRotation(AddRelRotator);
		}

		else if (SpringArm->GetRelativeRotation().Pitch > 0.0f)
		{
			SetRelRotator = FRotator(SetRelRotator.Pitch - 1.0f, SetRelRotator.Yaw, 0.0f);
			SpringArm->SetRelativeRotation(SetRelRotator);
			
		}
		else
		{
			SetRelRotator = FRotator(SetRelRotator.Pitch + 1.0f, SetRelRotator.Yaw, 0.0f);
			SpringArm->SetRelativeRotation(SetRelRotator);
		}
		
		
	}

}

// Called to bind functionality to input
void ATouchCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATouchCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ATouchCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ATouchCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATouchCharacter::Turn);
	//PlayerInputComponent->BindTouch(IE_Pressed, this, &ATouchCharacter::TouchPressed);
	//PlayerInputComponent->BindTouch(IE_Repeat, this, &ATouchCharacter::TouchMoved);
	//PlayerInputComponent->BindTouch(IE_Released, this, &ATouchCharacter::TouchReleased);
}

void ATouchCharacter::MoveForward(float NewAxisValue)
{
	//AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), NewAxisValue);
	
	APlayerCameraManager* Cameramanager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (Cameramanager)
	{
		AddMovementInput(Cameramanager->GetActorForwardVector(), NewAxisValue);
	}
	
}

void ATouchCharacter::MoveRight(float NewAxisValue)
{
	//ABLOG(Warning, TEXT("%f"), NewAxisValue);
	//AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), NewAxisValue);

	APlayerCameraManager* Cameramanager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (Cameramanager)
	{
		AddMovementInput(Cameramanager->GetActorRightVector(), NewAxisValue);
	}

}

void ATouchCharacter::LookUp(float NewAxisValue)
{
	//ABLOG(Warning, TEXT("%f"), NewAxisValue);
	AddControllerPitchInput(NewAxisValue);
}

void ATouchCharacter::Turn(float NewAxisValue)
{
	AddControllerYawInput(NewAxisValue);
}

/*
void ATouchCharacter::TouchPressed(ETouchIndex::Type FingerIndex, FVector Location)
{
	//ABLOG(Warning);
	if (FingerIndex == ETouchIndex::Touch1 || FingerIndex == ETouchIndex::Touch2)
	{
		TouchStart = GetTouchScreenLocation();
		//ABLOG(Warning, TEXT("%f%f"), TouchStart.X, TouchStart.Y);
	}
}

void ATouchCharacter::TouchMoved(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (FingerIndex == ETouchIndex::Touch1 || FingerIndex == ETouchIndex::Touch2)
	{
		TouchEnd = GetTouchScreenLocation();
		IsTouchPress = true;
		SpringArm->bUsePawnControlRotation = false;
		//ABLOG(Warning, TEXT("%d"), IsTouchPress);
	}
}

void ATouchCharacter::TouchReleased(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (FingerIndex == ETouchIndex::Touch1 || FingerIndex == ETouchIndex::Touch2)
	{
		IsTouchPress = false;
		SpringArm->bUsePawnControlRotation = true;
		//ABLOG(Warning, TEXT("%d"), IsTouchPress);
	}
}
*/

FVector2D ATouchCharacter::GetTouchScreenLocation()
{
	FVector2D returnVector2D;
	bool IsCurrentlyPressed_Touch1;
	APlayerController* playertouchcontrol = UGameplayStatics::GetPlayerController(this, 0);
	playertouchcontrol->APlayerController::GetInputTouchState(ETouchIndex::Touch1, returnVector2D.X, returnVector2D.Y, IsCurrentlyPressed_Touch1);
	
	return returnVector2D;
	
}

