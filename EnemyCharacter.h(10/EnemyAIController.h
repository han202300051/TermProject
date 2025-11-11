// EnemyAIController.h
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

    virtual void OnPossess(APawn* InPawn) override;
    virtual void Tick(float DeltaSeconds) override;

protected:
    // 플레이어에게 다가가는 최소 거리(이 거리 밖이면 플레이어를 쫓음)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
    float AcceptanceRadius = 50.f;

    // 공격을 시작할 거리 (EnemyCharacter의 AttackRange와 매칭되도록 설정)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
    float AttackRange = 200.f;

private:
    // 타겟
    APawn* PlayerPawn;
    APawn* ControlledPawn;
};
