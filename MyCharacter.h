#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

UCLASS()
class YOURGAME_API AMyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMyCharacter();

protected:
    virtual void BeginPlay() override;

    void Dodge();                     // 회피 시작
    void EndDodge();                  // 회피 종료

    bool bIsDodging;                  // 회피 중인지
    bool bIsInvincible;               // 무적 중인지

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dodge")
    float DodgeDistance; // 회피 거리

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dodge")
    float DodgeDuration; // 회피 지속 시간

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dodge")
    float InvincibleTime; // 무적 지속 시간

    FTimerHandle DodgeTimerHandle;
    FTimerHandle InvincibleTimerHandle; // 타이머 시스템 변수 -> 일정 시간 지나면 함수 호

    // 애니메이션 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dodge")
    UAnimMontage* DodgeMontage;

    // 이펙트 (파티클)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dodge")
    UParticleSystem* DodgeEffect;

public: 
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // 무적 타이머 종료
    void EndInvincibility();
};
