// Fill out your copyright notice in the Description page of Project Settings.


#include "SFAimComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "SFTurretDelegate.h"
#include "MyBlueprintFunctionLibrary.h"
#include "GameFramework/MovementComponent.h"

// Sets default values for this component's properties
USFAimComponent::USFAimComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	AimSpeed = 30.0f;
}

bool USFAimComponent::WasInitialized(bool logError)
{
	bool wasInit = BarrelSocket != NULL && MuzzleSocket != NULL && SkeletalMesh != NULL;
	if (!wasInit && logError) {
		UE_LOG(LogTemp, Error, TEXT("Unable to initialize aim component, barrel or muzzle sockets were invalid.  Check your socket names."))
	}
	return wasInit;
}

void USFAimComponent::Initialize(USkeletalMeshComponent* SKM, float projectileSpeed, FName Barrel, FName Muzzle)
{
	SkeletalMesh = SKM;
	BarrelSocket = SKM->GetSocketByName(Barrel);
	MuzzleSocket = SKM->GetSocketByName(Muzzle);
	ProjectileSpeed = projectileSpeed;
	if (!WasInitialized(true)) {
		return;
	}
	BarrelLength = FVector::Distance(BarrelSocket->GetSocketLocation(SKM), MuzzleSocket->GetSocketLocation(SKM));
}

void USFAimComponent::AimAt(FVector target) {
	if (!WasInitialized(true)) {
		return;
	}
	bWasTargetSet = true;
	Target = target;
}

void USFAimComponent::AimAtActor(AActor* actor, bool withLead) {
	if (!WasInitialized(true)) {
		return;
	}
	bWasTargetSet = actor != NULL;
	TrackedActor = actor;
	bLeadTrackedActor = withLead;
}

FRotator USFAimComponent::GetAimCallibration() {
	if (!bWasTargetSet) {
		return FRotator::ZeroRotator;
	}

	if (TrackedActor != NULL) {
		if (bLeadTrackedActor) {

			UMovementComponent* movementComp = (UMovementComponent*)TrackedActor->GetComponentByClass(UMovementComponent::StaticClass());
			FProjectilePredictionResult result = UMyBlueprintFunctionLibrary::ComputeProjectilePrediction(
				TrackedActor->GetActorLocation(), 
				movementComp != NULL ? movementComp->Velocity : TrackedActor->GetVelocity(),
				GetBarrelTransform().GetLocation(),
				ProjectileSpeed, 
				BarrelLength);
			if (result.bSuccess) {
				Target = result.predictedImpact;
			}
		} else {
			Target = TrackedActor->GetActorLocation();
		}
	}

	FTransform socketTransformWorld = BarrelSocket->GetSocketTransform(SkeletalMesh);
	FVector relativeTarget = GetOwner()->GetActorRotation().UnrotateVector(Target - socketTransformWorld.GetTranslation());
	return FRotationMatrix::MakeFromX(relativeTarget).Rotator();
}

bool USFAimComponent::IsAimingAtTarget(float tolerance) {

	if (!bWasTargetSet) {
		return false;
	}

	FTransform socketTransformWorld = BarrelSocket->GetSocketTransform(SkeletalMesh);
	FRotator barrelRotation = socketTransformWorld.Rotator();
	FRotator targetRotation = FRotationMatrix::MakeFromX(Target - socketTransformWorld.GetLocation()).Rotator();
	FRotator deltaRotator = barrelRotation - targetRotation;

	bool result = deltaRotator.Pitch <= tolerance && deltaRotator.Yaw <= tolerance;
//	UE_LOG(LogTemp, Warning, TEXT("socketTransformWorld : %s targetRotation: %s same %d"), *socketTransformWorld.Rotator().ToString(), *targetRotation.ToString(), result)
	return deltaRotator.Pitch <= tolerance && deltaRotator.Yaw <= tolerance;
}

bool USFAimComponent::Fire() {
	if (!GetOwner()->Implements<USFTurretDelegate>()) {
		return false;
	}
	return ISFTurretDelegate::Execute_Fire(GetOwner(), this);
}

FTransform USFAimComponent::GetMuzzleTransform() {
	return MuzzleSocket->GetSocketTransform(SkeletalMesh);
}

FTransform USFAimComponent::GetBarrelTransform() {
	return BarrelSocket->GetSocketTransform(SkeletalMesh);
}