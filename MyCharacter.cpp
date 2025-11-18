#include "MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

AMyCharacter::AMyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    bIsDodging = false;
    DodgeDistance = 600.f;   // 회피 거리
    DodgeDuration = 0.3f;    // 회피 시간
}

void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &AMyCharacter::Dodge);
}

void AMyCharacter::Dodge()
{
    if (bIsDodging) return;  // 이미 회피 중이면 무시

    bIsDodging = true;

    // 회피 방향 계산
    FVector DodgeDirection = GetLastMovementInputVector();
    if (DodgeDirection.IsNearlyZero())
    {
        DodgeDirection = GetActorForwardVector(); // 이동 안 하고 있을 때는 정면
    }
    DodgeDirection.Normalize();

    // 회피 속도 적용
    LaunchCharacter(DodgeDirection * DodgeDistance / DodgeDuration, true, true);

    // 타이머로 회피 끝내기
    GetWorldTimerManager().SetTimer(DodgeTimerHandle, this, &AMyCharacter::EndDodge, DodgeDuration, false);
}

void AMyCharacter::EndDodge()
{
    bIsDodging = false;

    // 이동 속도 초기화
    GetCharacterMovement()->StopMovementImmediately();
}
