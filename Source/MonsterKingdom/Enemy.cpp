// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"

#include "Particles/ParticleSystemComponent.h"

#include "AIModule/Classes/AIController.h"
#include "MainCharacter.h"
#include "Animation/AnimInstance.h"


// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DetectSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectSphere"));
	DetectSphere->SetupAttachment(RootComponent);
	DetectSphere->InitSphereRadius(600.f);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(RootComponent);
	CombatSphere->InitSphereRadius(75.f);

	AttackHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackHitBox"));
	AttackHitBox->SetupAttachment(GetMesh(), TEXT("HitboxSocket"));

	HitSound = CreateDefaultSubobject<UAudioComponent>(TEXT("HitSound"));
	HitSound->SetupAttachment(RootComponent);
	HitSound->SetAutoActivate(false);

	HitFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("HitFX"));
	HitFX->SetupAttachment(GetMesh(), TEXT("headSocket"));
	HitFX->SetAutoActivate(false);

;}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AI_Controller = Cast<AAIController>(GetController());
	
	DetectSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::DetectSphereOnBeginOverlap);
	DetectSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::DetectSphereOnEndOverlap);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnBeginOverlap);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnEndOverlap);

	AttackHitBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AttackHitBoxOnBeginOverlap);
	AttackHitBox->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AttackHitBoxOnEndOverlap);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::MoveToTarget()
{

	if (bIsAttacking)
		return;

	if (AI_Controller) {

		FNavPathSharedPtr NavPath;

		FAIMoveRequest MoveRequest;

		MoveRequest.SetGoalActor(TargetChar);
		MoveRequest.SetAcceptanceRadius(MoveToTargetRadius);

		AI_Controller->MoveTo(MoveRequest, &NavPath);

	}

}

void AEnemy::Attack()
{

	if (!IsAlive)
		return;

	if (bIsAttacking)
		return;

	bIsAttacking = true;
	// bCanDetectDamageCollision = true; // resolved in anim bp on attack activate animNotify

	if (AI_Controller)
		AI_Controller->StopMovement();

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && CombatMontage) {

		AnimInstance->Montage_Play(CombatMontage, FMath::FRandRange(0.9f, 1.9f));
		AnimInstance->Montage_JumpToSection(TEXT("Attack"), CombatMontage);
	}
}

void AEnemy::AttackEnded()
{

	bIsAttacking = false;

	bCanDetectDamageCollision = false;

	if (TargetChar) {

		if (bTargetInAttackRange) {

			float AttackDelay = FMath::RandRange(AttackDelayMin, AttackDelayMax);
			GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackDelay);

		}
		else {

			MoveToTarget();
		}


	}
	else {

		bTargetInAttackRange = false;

	}

}

bool AEnemy::ApplyDamage(float AppliedDamage, bool Crit)
{

	EnemyHitDelegate.Broadcast(AppliedDamage, Crit);

	CurrentHealth -= AppliedDamage;

	//bIsCritted = Crit;

	UE_LOG(LogTemp, Warning, TEXT("Applied %f Damage To Enemy, IsCrit: %s"), AppliedDamage, Crit ? *FString("True") : *FString("False"));

	PlayHitEffects();


	if (CurrentHealth <= 0.f) {

		if (AI_Controller)
			AI_Controller->StopMovement();

		IsAlive = false;

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance)
			AnimInstance->Montage_Stop(0.2f);

		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::DisposeEnemy, 2.f);

		

		return true;
	}

	return false;

}

void AEnemy::DisposeEnemy()
{

	Destroy();

}

void AEnemy::PlayHitEffects()
{

	HitSound->Play();
	HitFX->Activate(true);

}

void AEnemy::DetectSphereOnBeginOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult)
{

	if (!IsAlive)
		return;

	AMainCharacter* MyChar = Cast<AMainCharacter>(OtherActor);

	if (MyChar) {

		TargetChar = MyChar;
		MoveToTarget();

	}


}

void AEnemy::DetectSphereOnEndOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	if (!IsAlive)
		return;

	AMainCharacter* MyChar = Cast<AMainCharacter>(OtherActor);

	if (MyChar) {

		TargetChar = nullptr;

		if (AI_Controller)
			AI_Controller->StopMovement();

	}

}

void AEnemy::CombatSphereOnBeginOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult)
{

	if (!IsAlive)
		return;

	AMainCharacter* MyChar = Cast<AMainCharacter>(OtherActor);

	if (MyChar) {

		bTargetInAttackRange = true;

		float AttackDelay = FMath::RandRange(AttackDelayMin, AttackDelayMax);
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackDelay);

	}

}

void AEnemy::CombatSphereOnEndOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	if (!IsAlive)
		return;

	AMainCharacter* MyChar = Cast<AMainCharacter>(OtherActor);

	if (MyChar) {

		bTargetInAttackRange = false;

	}

}

void AEnemy::AttackHitBoxOnBeginOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult)
{

	if (!IsAlive)
		return;

	if (bCanDetectDamageCollision) {

		AMainCharacter* MyChar = Cast<AMainCharacter>(OtherActor);

		if (MyChar) {

			bCanDetectDamageCollision = false;

			if (MyChar->ApplyDamage(Damage))
				TargetChar = nullptr;

		}

	}
}

void AEnemy::AttackHitBoxOnEndOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	if (!IsAlive)
		return;

}
