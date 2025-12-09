#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

AEnemyAIController::AEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 플레이어 Pawn 캐싱
	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void AEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!PlayerPawn || !GetPawn())
		return;

	APawn* AIPawn = GetPawn();
	float Distance = FVector::Dist(AIPawn->GetActorLocation(), PlayerPawn->GetActorLocation());

	// ======================
	// 1) 시야 체크 → 보이면 추격 OR 공격
	// ======================
	if (IsPlayerInSight())
	{
		// 플레이어가 너무 멀면 → 추격
		if (Distance > FireRange)
		{
			MoveToActor(PlayerPawn, StopDistance);
		}
		else
		{
			// 공격 거리 안 → 정지 + 사격
			StopMovement();
			ShootPlayer();
		}
	}
	else
	{
		// 시야에서 놓침 → 추격 중지
		StopMovement();
	}
}

/**
 * 시야각 + 시야거리 + 장애물 체크
 */
bool AEnemyAIController::IsPlayerInSight()
{
	if (!PlayerPawn || !GetPawn()) return false;

	const FVector MyLoc = GetPawn()->GetActorLocation() + FVector(0,0,50);
	const FVector PlayerLoc = PlayerPawn->GetActorLocation() + FVector(0,0,50);

	FVector DirToPlayer = (PlayerLoc - MyLoc).GetSafeNormal();
	FVector Forward = GetPawn()->GetActorForwardVector();

	// 1) 시야각 체크
	float Dot = FVector::DotProduct(Forward, DirToPlayer);
	float Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));

	if (Angle > SightAngle)
		return false;

	// 2) 시야 거리 체크
	if (FVector::Dist(MyLoc, PlayerLoc) > SightRadius)
		return false;

	// 3) 장애물 체크 (LineTrace)
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetPawn());

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit, MyLoc, PlayerLoc, ECC_Visibility, Params);

	if (bHit && Hit.GetActor() != PlayerPawn)
	{
		return false; // 벽에 가려짐
	}

	// 디버그 라인 (초록색)
	DrawDebugLine(GetWorld(), MyLoc, PlayerLoc, FColor::Green, false, 0.05f);

	return true;
}

/**
 * 총 발사 (라인트레이스 기반)
 */
void AEnemyAIController::ShootPlayer()
{
	if (!bCanFire) return;
	bCanFire = false;

	FVector Start = GetPawn()->GetActorLocation() + FVector(0,0,50);
	FVector End = PlayerPawn->GetActorLocation();

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetPawn());

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit, Start, End, ECC_Visibility, Params);

	if (bHit)
	{
		// 맞은 곳까지 빨간 레이 그리기
		DrawDebugLine(GetWorld(), Start, Hit.ImpactPoint, FColor::Red, false, 1.0f, 0, 2.0f);

		// 여기서 데미지 적용 가능
		// UGameplayStatics::ApplyDamage(Hit.GetActor(), Damage, GetPawn()->GetController(), GetPawn(), nullptr);
	}

	// 쿨타임
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(
		Timer,
		[this]() { bCanFire = true; },
		FireCooldown,
		false
	);
}
