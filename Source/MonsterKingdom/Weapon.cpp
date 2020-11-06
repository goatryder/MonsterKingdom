// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"

#include "GameFramework/Character.h"

#include "Kismet/GameplayStatics.h"

#include "MainCharacter.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	RootComponent = SkeletalMesh;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(RootComponent);

	AttackHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackHitBox"));
	AttackHitBox->SetupAttachment(RootComponent);

	PickFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PickFX"));
	PickFX->SetupAttachment(RootComponent);
	PickFX->SetAutoActivate(false);

	PickSound = CreateDefaultSubobject<UAudioComponent>(TEXT("PickSound"));
	PickSound->SetupAttachment(RootComponent);
	PickSound->SetAutoActivate(false);

	SpherePickFXDetector = CreateDefaultSubobject<USphereComponent>(TEXT("PickFXDetector"));
	SpherePickFXDetector->SetupAttachment(RootComponent);
	SpherePickFXDetector->SetSphereRadius(400.f);
	

}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	AActor::BeginPlay();
	
	InitialZLocation = GetActorLocation().Z;

	// Pick FX Collision Detecor
	SpherePickFXDetector->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSpherePickFXDetectorOverlap);
	SpherePickFXDetector->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSpherePickFXDetectorEndOverlap);

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereCollisionOverlapBegin);

}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);

	if (!bIsUsed) {

		PickableRotateYawMoveZ(DeltaTime);
		// PickableSoundAndFXNearPlayerHandle(); DEPRECATED

	}
		

}

void AWeapon::PickableRotateYawMoveZ(float DeltaTime)
{
	// rotate yaw

	FRotator Rotation = GetActorRotation();
	Rotation.Yaw += DeltaTime * PickableYawRotationRate;
	SetActorRotation(Rotation);

	// Calc Delta Time To Float Movement For Move Up or Down Only When We Make 180 Rotation

	float FloatMovementDelta = PickableYawRotationRate / 90.f * DeltaTime; // multiplyed by to, cos we move to -20 and to 20, so it's double distance and shoold be double speed

	// UE_LOG(LogTemp, Warning, TEXT("rotation delta %f, move delta %f"), DeltaTime, FloatMovementDelta);

	// float movement on z axis

	FVector ActorLoc = GetActorLocation();

	if (ActorLoc.Z > InitialZLocation + PickableFloatingZMax)
		bPickableMovePositiveZ = false;
	else if (ActorLoc.Z < InitialZLocation - PickableFloatingZMax)
		bPickableMovePositiveZ = true;

	if (bPickableMovePositiveZ)
		ActorLoc.Z += PickableFloatingZRate * FloatMovementDelta;
	else
		ActorLoc.Z -= PickableFloatingZRate * FloatMovementDelta;

	SetActorLocation(ActorLoc);

}

/* 
// DEPRECATED
void AWeapon::PickableSoundAndFXNearPlayerHandle()
{

	ACharacter* Char = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (!Char)
		return;

	float CharDistance = (GetActorLocation() - Char->GetActorLocation()).Size();

	UE_LOG(LogTemp, Warning, TEXT("%s (%f)"), 
		CharDistance <= PickableFXDistance ? 
		*FString("Player in PickableFXDistance range") :
		*FString("Player not in PickableFXDistance range"),
		PickableFXDistance);

	if (CharDistance <= PickableFXDistance) {

		if (!PickSound->IsPlaying())
			PickSound->Play();

		if (!PickFX->IsActive())
			PickFX->Activate();

	}

	else {

		if (PickSound->IsPlaying())
			PickSound->Stop();
		
		if (PickFX->IsActive())
			PickFX->Deactivate();

	}

	
} */

void AWeapon::OnSpherePickFXDetectorOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bSweepFrom, const FHitResult& SweepResult)
{

	// UE_LOG(LogTemp, Warning, TEXT("SpherePick Begin Overlap"));

	if (bIsUsed)
		return;
	
	if (OtherActor != UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
		return;

	if (!PickSound->IsPlaying())
		PickSound->Play();

	//if (!PickFX->IsActive())
	PickFX->Activate(true);

}

void AWeapon::OnSpherePickFXDetectorEndOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	// UE_LOG(LogTemp, Warning, TEXT("SpherePick End Overlap"));

	if (bIsUsed)
		return;

	if (OtherActor != UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
		return;

	if (PickSound->IsPlaying())
		PickSound->Stop();

	//if (PickFX->IsActive())
	PickFX->Deactivate();

}

void AWeapon::AttachItemTo(USkeletalMeshComponent* meshRoot, FName socket)
{

	SpherePickFXDetector->SetCollisionEnabled(ECollisionEnabled::NoCollision); // disable pick fx collision detection
	bIsUsed = true;

	AttachToComponent(meshRoot, FAttachmentTransformRules::KeepRelativeTransform, socket);

	SetActorRelativeLocation(FVector::ZeroVector);

	SetActorRelativeRotation(FRotator::ZeroRotator);

}

void AWeapon::OnSphereCollisionOverlapBegin(UPrimitiveComponent* OverlapComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bSweepFrom, const FHitResult& SweepResult)
{

	// UE_LOG(LogTemp, Warning, TEXT("SphereCollision Begin Overlap"));

	// if (!OtherActor->ActorHasTag("Player"))
	//	return;

	if (OtherActor != UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
		return;
	
	AMainCharacter* PlayerChar = Cast<AMainCharacter>(OtherActor);

	if (!PlayerChar)
		return;

	PlayerChar->EquipWeapon(this);

}

