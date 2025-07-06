// WanderAgent.h
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WanderAgent.generated.h"

UCLASS()
class HYSTERIA_API AWanderAgent : public ACharacter
{
	GENERATED_BODY()

public:
	AWanderAgent();
	virtual void BeginPlay() override;
	void WanderToRandomLocation();

protected:
	FTimerHandle WanderTimerHandle;
};
