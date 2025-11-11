// EnemyAIController.cpp
#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyCharacter.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"

AEnemyAIController::AEnemyAIController()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    ControlledPawn = InPawn;
    // 플레이어 찾기 (프로젝트에 플레이어가 하나라는 가정)
    // 플레이어 Pawn을 바로 캐싱 → Tick에서 매번 검색하지 않아도 됨. (성능 최적화 포인트)
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        PlayerPawn = PC->GetPawn();
    }
}

void AEnemyAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // PlayerPawn이 없으면(=플레이어 죽음 또는 초기 로딩) AI는 멈춤.
    if (!PlayerPawn || !ControlledPawn) return;

    // 거리 계산에서 제곱 거리 비교 사용. (루트 연산은 무거움)
    const float DistSqr = FVector::DistSquared(PlayerPawn->GetActorLocation(), ControlledPawn->GetActorLocation());
    const float AttackRangeSqr = FMath::Square(AttackRange);

    // 사거리 내면 정지하고 공격 시도
    if (DistSqr <= AttackRangeSqr)
    {
        StopMovement();

        // 시도해서 공격 (ControlledPawn를 AEnemyCharacter로 캐스팅)
        AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(ControlledPawn);
        if (Enemy && Enemy->CanAttack())
        {
            Enemy->TryAttack(PlayerPawn);
        }
    }
    else
    {
        // 사거리 밖이면 이동
        MoveToActor(PlayerPawn, AcceptanceRadius, true, true, true, nullptr, true);
    }
}
