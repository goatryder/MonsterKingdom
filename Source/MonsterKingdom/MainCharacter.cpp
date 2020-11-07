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

#include "Weapon.h"
#include "Enemy.h"


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

	if (bIsAttacking)
		return;

	if (GetCharacterMovement()->Velocity.Size() > 0.5f)
		return;

	if (Weapon) {

		UAnimInstance* MyAnim = GetMesh()->GetAnimInstance();

		if (MyAnim && CombatMontage) {

			bIsAttacking = true; // disabled in anim bp

			MyAnim->Montage_Play(CombatMontage, FMath::FRandRange(1.4f, 2.4f));

			int min = 0;
			int max = 2;

			int RandAnim = std::rand() % (max - min) + min;

			switch (RandAnim) {

				case 0:
					MyAnim->Montage_JumpToSection(TEXT("Attack_1"));
					break;

				case 1:
					MyAnim->Montage_JumpToSection(TEXT("Attack_2"));
					break;

			}

			// CanDetectDamageCollision = true; // resolved in anim bp with anim notify
		}

	}

}

void AMainCharacter::SetMovementSpeedAndAir()
{

	FVector CharSpeed = GetVelocity();

	MovementSpeed = FVector(CharSpeed.X, CharSpeed.Y, 0.f).Size();

	bInAir = GetMovementComponent()->IsFalling();
}

void AMainCharacter::EquipWeapon(AWeapon* WeaponActor)
{

	// if we have weapon already do nothing
	if (Weapon != nullptr)
		return;

	WeaponActor->AttachItemTo(GetMesh(), TEXT("RightHandSocket"));

	Weapon = WeaponActor;

	// add on component begin overlap for hit box
	Weapon->AttackHitBox->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::DamageBoxOnBeginOverlap);

}

void AMainCharacter::DamageBoxOnBeginOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult)
{

	if (CanDetectDamageCollision) {

		AEnemy* Enemy = Cast<AEnemy>(OtherActor);

		if (Enemy) {
			
			CanDetectDamageCollision = false; // block multipy event fire on one attack

			Enemy->ApplyDamage(Weapon->CalcDamage());

		}

	}

}

bool AMainCharacter::ApplyDamage(float AppliedDamage)
{

	Health -= AppliedDamage;

	// UE_LOG(LogTemp, Warning, TEXT("Applied %f Damage To Main Character, Health: %f"), AppliedDamage, Health);

	if (Health <= 0.f) {

		IsAlive = false;

		// UE_LOG(LogTemp, Warning, TEXT("void AMainCharacter::ApplyDamage PLAYER IsAlive=false"));

		// todo restart game
	
		return true;
	
	}

	return false;

}

