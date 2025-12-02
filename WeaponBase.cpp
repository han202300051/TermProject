// WeaponBase.cpp

#include "WeaponBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "RecoilComponent.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// 반동 컴포넌트 생성 (무기 액터에 붙음)
	RecoilComp = CreateDefaultSubobject<URecoilComponent>(TEXT("RecoilComponent"));

	// 시작 시 현재 퍼짐을 기본 퍼짐값으로 설정
	CurrentSpread = BaseSpread;
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
}

/**
 * Fire
 * - 캐릭터가 발사 버튼을 눌렀을 때 호출되는 함수
 * - 여기서는 "반동 + 퍼짐 증가"만 처리
 * - 실제 LineTrace나 Projectile 발사는 Character에서 별도로 처리
 */
void AWeaponBase::Fire(bool bIsADS)
{
	// 1) 반동 적용
	if (RecoilComp)
	{
		RecoilComp->ApplyRecoil(bIsADS);
	}

	// 2) 퍼짐 값 증가
	//    - BaseSpread ~ MaxSpread 범위 내에서만 증가하도록 Clamp
	CurrentSpread = FMath::Clamp(
		CurrentSpread + SpreadIncreasePerShot,
		BaseSpread,
		MaxSpread
	);

}

/**
 * ApplyBulletSpread
 * - 카메라(또는 총구)의 정방향 벡터에
 *   "원뿔 형태의 랜덤 방향"을 적용해서 퍼진 사격 방향을 만들어주는 함수
 */
FVector AWeaponBase::ApplyBulletSpread(const FVector& ForwardVector) const
{
	/**
	 * RandomUnitVectorInConeInDegrees
	 * - 기준 벡터(ForwardVector)를 중심으로
	 *   ConeHalfAngleDegree(여기선 CurrentSpread) 만큼의 각도 안에서
	 *   랜덤한 방향 벡터를 리턴
	 * - 즉, CurrentSpread가 클수록 원뿔이 넓어지고 탄이 더 퍼짐
	 */
	const float ConeHalfAngleDegree = CurrentSpread;

	// ForwardVector를 기준으로 ConeHalfAngleDegree 내의 랜덤 방향 생성
	FVector NewDir = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(
		ForwardVector,
		ConeHalfAngleDegree
	);

	NewDir.Normalize();
	return NewDir;
}
