#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

UCLASS()
class YOURGAME_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;

private:
	APawn* PlayerPawn;

public:
	/** 추적 시작 시야 거리 */
	UPROPERTY(EditAnywhere, Category = "AI")
	float SightRadius = 1500.0f;

	/** 시야각 (도 단위) */
	UPROPERTY(EditAnywhere, Category = "AI")
	float SightAngle = 60.0f;

	/** 추격 중 멈춰야 하는 플레이어와의 최소 거리 */
	UPROPERTY(EditAnywhere, Category = "AI")
	float StopDistance = 200.0f;

	/** 총 발사 거리 */
	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireRange = 800.0f;

	/** 총 쏘는 쿨타임 */
	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireCooldown = 1.0f;

	bool bCanFire = true;

private:
	bool IsPlayerInSight();
	void ShootPlayer();
};
