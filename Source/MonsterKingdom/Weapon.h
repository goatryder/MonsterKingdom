// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class USphereComponent;
class USkeletalMeshComponent;
class UBoxComponent;
class UParticleSystemComponent;
class UAudioComponent;


UCLASS()
class MONSTERKINGDOM_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float InitialZLocation;
	bool bPickableMovePositiveZ;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Pickable State Effects
	void PickableRotateYawMoveZ(float DeltaTime);
	
	// DEPRECATED
	// void PickableSoundAndFXNearPlayerHandle();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float PickableYawRotationRate = 180.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float PickableFloatingZMax = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float PickableFloatingZRate = 20.f;

	// DEPRECATED
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//	float PickableFXDistance = 200.f;

	bool bIsUsed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USphereComponent* SphereCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* AttackHitBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UParticleSystemComponent* PickFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAudioComponent* PickSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USphereComponent* SpherePickFXDetector;
	
	void AttachItemTo(USkeletalMeshComponent* meshRoot, FName socket);

	UFUNCTION()
		virtual void OnSpherePickFXDetectorOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
			bool bSweepFrom, const FHitResult& SweepResult);

	UFUNCTION()
		virtual void OnSpherePickFXDetectorEndOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		virtual void OnSphereCollisionOverlapBegin(UPrimitiveComponent* OverlapComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
			bool bSweepFrom, const FHitResult& SweepResult);


};