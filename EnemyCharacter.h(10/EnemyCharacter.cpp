// EnemyCharacter.cpp
#include "EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"

AEnemyCharacter::AEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
    bCanAttack = true;
}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AEnemyCharacter::TryAttack(AActor* Target)
{
    if (!Target || !bCanAttack) return;

    // 거리 체크
    const float DistSqr = FVector::DistSquared(Target->GetActorLocation(), GetActorLocation());
    if (DistSqr > FMath::Square(AttackRange)) return;

    // 공격 애니메이션 재생(있다면)
    if (AttackMontage && GetMesh())
    {
        PlayAnimMontage(AttackMontage);
        // 실제 데미지 판정은 몽타주 Notify에서 PerformAttack를 호출하거나 여기서 바로 호출해도 됨.
    }

    // 기본적으로 즉시 판정
    PerformAttack(Target);

    // 쿨다운 설정
    bCanAttack = false;
    GetWorldTimerManager().SetTimer(TimerHandle_ResetAttack, this, &AEnemyCharacter::ResetAttack, AttackCooldown, false);
}

void AEnemyCharacter::PerformAttack(AActor* Target)
{
    if (!Target) return;

    // 간단한 라인/원형 판정이 필요하면 여기서 추가. 지금은 단순 거리 기반으로 바로 데미지.
    UGameplayStatics::ApplyDamage(Target, AttackDamage, GetController(), this, nullptr);
}

void AEnemyCharacter::ResetAttack()
{
    bCanAttack = true;
}
