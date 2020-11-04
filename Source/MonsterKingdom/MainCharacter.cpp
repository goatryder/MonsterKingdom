// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

#include "Camera/CameraComponent.h"

#include "Animation/AnimInstance.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"

#include "Kismet/GameplayStatics.h"

//#include Weapon.h
//#include Enemy.h


// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// add character movement settings
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 550.f;
	GetCharacterMovement()->AirControl = 0.2f;


}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	

}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetMovementSpeedAndAir();

}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAxis("CameraPitch", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("CameraYaw", this, &APawn::AddControllerYawInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::Jump);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainCharacter::Unsprint);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AMainCharacter::Attack);
}

void AMainCharacter::MoveForward(float Val)
{

	if (!IsAlive)
		return;

	if (Val != 0.f && !bIsAttacking) {

		auto YawRotation = FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);
		auto Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, Val);

	}


}

void AMainCharacter::MoveRight(float Val)
{

	if (!IsAlive)
		return;

	if (Val != 0.f && !bIsAttacking) {

		auto YawRotation = FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);
		auto Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction, Val);

	}


}

void AMainCharacter::Jump()
{

	if (!IsAlive)
		return;

	if (bIsAttacking)
		return;

	ACharacter::Jump();

}

void AMainCharacter::Sprint()
{

	if (!IsAlive)
		return;

	if (GetCharacterMovement()->Velocity.Size() == 0.f)
		return;

	MovementState = EMovementState::MS_Sprint;
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

}

void AMainCharacter::Unsprint()
{

	if (!IsAlive)
		return;

	MovementState = EMovementState::MS_Run;
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;

}

void AMainCharacter::Attack()
{

	if (!IsAlive)
		return;



}

void AMainCharacter::SetMovementSpeedAndAir()
{

	FVector CharSpeed = GetVelocity();

	MovementSpeed = FVector(CharSpeed.X, CharSpeed.Y, 0.f).Size();

	bInAir = GetMovementComponent()->IsFalling();
}

