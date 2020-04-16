// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SFSpringFlightMovementComponent.generated.h"

USTRUCT(BlueprintType)
struct SPACEFORCE_API FSpringConfig {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Stiffness;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CriticalDampingCoefficient;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxExtension;

	FSpringConfig() : Stiffness(0.0f), CriticalDampingCoefficient(0.0f), MaxExtension(0.0f) {}

	FSpringConfig(float stiffness, float criticalDamping, float maxExtension) : Stiffness(stiffness), CriticalDampingCoefficient(criticalDamping), MaxExtension(maxExtension) {}

};

class AActor;

UCLASS(ClassGroup = Movement, meta = (BlueprintSpawnableComponent), ShowCategories = (Velocity))
class SPACEFORCE_API USFSpringFlightMovementComponent : public UPawnMovementComponent
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(EditAnywhere)
	FSpringConfig SpringConfig;

	UPROPERTY(EditAnywhere)
	bool bDebug;

	UPROPERTY(EditAnywhere)
	bool bDebugRotation;

// Begin target
	UPROPERTY(BlueprintGetter = GetTarget, BlueprintSetter = SetTarget)
	FVector Target;

	UPROPERTY(BlueprintGetter = GetTargetActor, BlueprintSetter = SetTargetActor)
	AActor* TargetActor;

	UFUNCTION(BlueprintGetter)
	FVector GetTarget();

	UFUNCTION(BlueprintSetter)
	void SetTarget(FVector value);

	UFUNCTION(BlueprintGetter)
	AActor* GetTargetActor();

	UFUNCTION(BlueprintSetter)
	void SetTargetActor(AActor* value);

	UFUNCTION(BlueprintCallable)
	void ClearTarget();

	UPROPERTY(EditAnywhere)
	bool bCalculateVelocityOfTarget = true;

private:
	UPROPERTY()
	bool bHasTarget;

	void UpdateTarget(float DeltaTime);
// End target
	
protected:
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

	void BeginPlay() override;

private:
	FVector CalculateForces();

	FVector CalculateTorque(FVector forward);

	bool IsValid(bool logError = false);

	float ComputeDampingCoefficient(FSpringConfig config, float mass);
	float DampingCoefficient;

	class UPrimitiveComponent* GetUpdatedPrimitiveComp();

	FVector DefaultForward = FVector::ForwardVector;

	FVector TargetVelocity = FVector::ZeroVector;

	FVector LastTargetPosition;

//	float GetMaxSpeed();

};
