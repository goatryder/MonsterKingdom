// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

class USphereComponent;
class UBoxComponent;
class AMainCharacter;
class UAnimMontage;
class AAIController;
class UParticleSystemComponent;
class UAudioComponent;


UCLASS()
class MONSTERKINGDOM_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
		USphereComponent* DetectSphere;

	UPROPERTY(EditAnywhere)
		USphereComponent* CombatSphere;

	UPROPERTY(EditAnywhere)
		UBoxComponent* AttackHitBox;

	UFUNCTION()
		virtual void DetectSphereOnBeginOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		virtual void DetectSphereOnEndOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		virtual void CombatSphereOnBeginOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		virtual void CombatSphereOnEndOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		virtual void AttackHitBoxOnBeginOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		virtual void AttackHitBoxOnEndOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(BlueprintReadOnly)
		bool IsAlive = true;

	AMainCharacter* TargetChar;
	AAIController* AI_Controller;

	void MoveToTarget();

	UPROPERTY(BlueprintReadOnly)
		bool bIsAttacking;

	UPROPERTY(EditAnywhere)
		float MoveToTargetRadius = 10.f;

	UPROPERTY(EditAnywhere)
		bool bTargetInAttackRange;

	float AttackDelayMin = 0.2f;
	float AttackDelayMax = 0.7f;

	UPROPERTY(BlueprintReadWrite)
		bool bCanDetectDamageCollision;

	void Attack();

	UFUNCTION(BlueprintCallable)
		void AttackEnded();

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere)
		UAnimMontage* CombatMontage;

	UPROPERTY(EditAnywhere)
		float Health = 100.f;

	UPROPERTY(EditAnywhere)
		float Damage = 20.f;

	UFUNCTION(BlueprintCallable)
		bool ApplyDamage(float AppliedDamage);
	
	UFUNCTION(BlueprintCallable)
		void DisposeEnemy();

	UPROPERTY(EditAnywhere)
		UAudioComponent* HitSound;

	UPROPERTY(EditAnywhere)
		UParticleSystemComponent* HitFX;

	UFUNCTION(BlueprintCallable)
		void PlayHitEffects();

};
