#include "MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "Animation/AnimInstance.h"

AMyCharacter::AMyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    bIsDodging = false; // 처음엔 회피 중 X
    bIsInvincible = false; // 무적 X
    DodgeDistance = 600.f; // 회피 거리
    DodgeDuration = 0.3f; // 회피하는 데 걸리는 시간
    InvincibleTime = 0.3f; // 무적 지속 시간
}

void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &AMyCharacter::Dodge);
}  // "Dodge" (ex_spacebar) 입력을 누르면 → Dodge() 함수 실행

void AMyCharacter::Dodge()
{
    if (bIsDodging) return;

    bIsDodging = true; // 회피 시작
    bIsInvincible = true; // 무적 시작

    // 이동 방향 계산 ( 마지막 입력 방향을 가져옴 (WASD) )
    FVector DodgeDirection = GetLastMovementInputVector();
    if (DodgeDirection.IsNearlyZero())
    {
        DodgeDirection = GetActorForwardVector();
    }
    DodgeDirection.Normalize();

    // 회피 이동 ( 캐릭터를 순간적으로 튕기듯 힘을 가하는 함수 )
    LaunchCharacter(DodgeDirection * DodgeDistance / DodgeDuration, true, true);

    // 회피 애니메이션 재생
    if (DodgeMontage)
    {
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            AnimInstance->Montage_Play(DodgeMontage);  // DodgeMontage가 연결되어 있다면 회피 애니 재생
        }
    }

    // 회피 이펙트 ( 재생 캐릭터 위치에 파티클 생성 (잔상, 연기, 발광 등) )
    if (DodgeEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DodgeEffect, GetActorLocation(), GetActorRotation());
    }

    // 타이머로 회피 종료 ( 일정 시간 뒤 자동으로 동작하는 구조 )
    GetWorldTimerManager().SetTimer(DodgeTimerHandle, this, &AMyCharacter::EndDodge, DodgeDuration, false);
    GetWorldTimerManager().SetTimer(InvincibleTimerHandle, this, &AMyCharacter::EndInvincibility, InvincibleTime, false);
}

// 회피 상태 종료 ( 순간속도 제거 (미끄러지는 현상 방지) )
void AMyCharacter::EndDodge()
{
    bIsDodging = false;
    GetCharacterMovement()->StopMovementImmediately();
}

// 무적 해제 -> 공격 판정이 다시 들어오게 됨
void AMyCharacter::EndInvincibility()
{
    bIsInvincible = false;
}
