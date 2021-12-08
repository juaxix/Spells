// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/Character.h"
#include "SAICharacter.generated.h"

UCLASS()
class SPELLS_API ASAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASAICharacter();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintPure, Category = "Spells|AI|Attack")
	FORCEINLINE FVector GetMuzzleLocation() const
	{
		return MuzzleSocket->GetSocketLocation(GetMesh());
	}

protected:
	UPROPERTY(Transient)
	USkeletalMeshSocket const* MuzzleSocket = nullptr;
};
