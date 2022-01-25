// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SGameplayBlueprintFunctions.generated.h"

/**
 * Gameplay functions
 */
UCLASS()
class SPELLS_API USGameplayBlueprintFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Spells|Gameplay|Attack")
	static bool ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount, const FHitResult& HitResult);

	UFUNCTION(BlueprintPure, Category = "Spells|Player Controller")
	static APlayerController* GetFirstLocalPlayerController(const UObject* Context)
	{
		if (!IsValid(Context))
		{
			return nullptr;
		}

		if (const UWorld* World = Context->GetWorld())
		{
			return World->GetFirstPlayerController();
		}

		return nullptr;
	}
};
