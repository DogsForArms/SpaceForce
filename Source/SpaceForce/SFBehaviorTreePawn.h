// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SFBehaviorTreePawn.generated.h"

UCLASS()
class SPACEFORCE_API ASFBehaviorTreePawn : public APawn
{
	GENERATED_BODY()

public:
	ASFBehaviorTreePawn();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere)
	class UBehaviorTree* BehaviorTree;
};
