// EnemyCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class YOURGAME_API AEnemyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyCharacter();

    // 공격 시 호출
    UFUNCTION(BlueprintCallable)
    void TryAttack(AActor* Target);

    // 실제 공격 판정(애니 이벤트나 타이밍에 맞춰 호출해도 됌)
    UFUNCTION(BlueprintCallable)
    void PerformAttack(AActor* Target);

    // 공격 가능 여부
    bool CanAttack() const { return bCanAttack; }

protected:
    virtual void BeginPlay() override;

    // 공격이 성공했을 때 피해 적용에 쓰는 값 (블루프린트에서 값 재지정 가능)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    float AttackDamage = 20.f;

    // 공격 범위 (사거리)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    float AttackRange = 200.f;

    // 공격 쿨다운(초)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    float AttackCooldown = 1.5f;

    // 타격에 사용할 몽타주(선택)
    UPROPERTY(EditAnywhere, Category="Combat|Animation")
    UAnimMontage* AttackMontage;

private:
    void ResetAttack();

    bool bCanAttack;

    FTimerHandle TimerHandle_ResetAttack;
};
