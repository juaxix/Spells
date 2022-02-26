// Spells - xixgames - juaxix - 2021/2022

#include "SGameInstance.h"

// Photon includes
#include "PhotonCloudConfig.h"
#include "PhotonCloudSubsystem.h"

// Spells includes
#include "Online/SPhotonCloudObject.h"

void USGameInstance::Init()
{
#if WITH_EDITOR
	UObject* CloudBPAsset = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL,
		TEXT("/Game/SpellsGame/Blueprints/Online/BP_PhotonCloudObject.BP_PhotonCloudObject")));
#else
	UClass* CloudBP = StaticLoadClass(UObject::StaticClass(), NULL,
		TEXT("/Game/SpellsGame/Blueprints/Online/BP_PhotonCloudObject.BP_PhotonCloudObject_C"));
#endif

	if (UPhotonCloudConfig* PhotonConfig = GetMutableDefault<UPhotonCloudConfig>())
	{
#if WITH_EDITOR
		if (const UBlueprint* CloudBP = Cast<UBlueprint>(CloudBPAsset))
		{
			PhotonConfig->PhotonCloudAPIObjectClass = CloudBP->GeneratedClass;
		}
#else
		UPhotonCloudConfig::PhotonCloudAPIObjectClassOverride = CloudBP;
#endif
	}
	
	Super::Init();
}

void USGameInstance::OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld)
{
	Super::OnWorldChanged(OldWorld, NewWorld);
	if (IsValid(NewWorld))
	{
		if (const UPhotonCloudSubsystem* PhotonCloudSubsystem = NewWorld->GetGameInstance()->GetSubsystem<UPhotonCloudSubsystem>())
		{
			if (USPhotonCloudObject* PhotonCloudObject = Cast<USPhotonCloudObject>(PhotonCloudSubsystem->GetPhotonCloudAPI()))
			{
				PhotonCloudObject->OnWorldChanged(OldWorld, NewWorld);
			}
		}
	}
}
