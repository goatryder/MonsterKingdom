// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AWeapon;

UENUM(BlueprintType)
enum class EMovementState : uint8 {

	MS_Run UMETA(DislpayName = "Run"),
	MS_Sprint UMETA(DisplayName = "Sprint")

};


UCLASS()
class MONSTERKINGDOM_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UCameraComponent* FollowCamera;

	void MoveForward(float Val);
	void MoveRight(float Val);

	virtual void Jump() override;
	
	void Sprint();
	void Unsprint();

	void Attack();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool IsAlive = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EMovementState MovementState;

	UPROPERTY(BlueprintReadOnly)
		AWeapon* Weapon;

	UPROPERTY(BlueprintReadOnly)
		float MovementSpeed;

	UPROPERTY(BlueprintReadOnly)
		bool bInAir;

	void SetMovementSpeedAndAir();

	UPROPERTY(EditAnywhere)
		float RunSpeed = 600.f;

	UPROPERTY(EditAnywhere)
		float SprintSpeed = 950.f;


};
