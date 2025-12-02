// WeaponBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class URecoilComponent;

UCLASS()
class YOURGAME_API AWeaponBase : public AActor
{
	GENERATED_BODY()

public:	
	// 생성자
	AWeaponBase();

protected:
	virtual void BeginPlay() override;

public:	

	// ====================== [Spread 관련 프로퍼티] ======================

	/**
	 * 기본 탄 퍼짐 값 (도 단위)
	 * - 0에 가까울수록 레이저총, 값이 클수록 샷건 느낌
	 * - 예: 1.0f ~ 3.0f 정도면 일반 AR 느낌
	 */
	UPROPERTY(EditAnywhere, Category = "Weapon|Spread")
	float BaseSpread = 1.0f;

	/**
	 * 한 발 쏠 때마다 얼마나 퍼짐이 증가할지
	 * - 연사 시 점점 퍼지는 느낌을 주기 위함
	 */
	UPROPERTY(EditAnywhere, Category = "Weapon|Spread")
	float SpreadIncreasePerShot = 0.2f;

	/**
	 * 퍼짐의 최대 한계값
	 * - 아무리 오래 쏴도 이 값 이상으로는 안 퍼지게 제한
	 */
	UPROPERTY(EditAnywhere, Category = "Weapon|Spread")
	float MaxSpread = 4.0f;

	/**
	 * 현재 사용 중인 퍼짐 값
	 * - BaseSpread에서 시작해서 사격할 때마다 증가
	 */
	UPROPERTY(VisibleAnywhere, Category = "Weapon|Spread")
	float CurrentSpread;

	// ====================== [Recoil 관련 컴포넌트] ======================

	/**
	 * 반동을 담당하는 컴포넌트
	 * - 이 무기에 붙어있지만, 실제로는 Owner Character의 컨트롤러에 반동을 줌
	 */
	UPROPERTY(VisibleAnywhere, Category = "Weapon|Recoil")
	URecoilComponent* RecoilComp;

	// ====================== [함수들] ======================

	/**
	 * 발사 함수
	 * - 실제 데미지/총알 스폰은 Character 쪽에서 처리
	 *   이 함수에서는 "반동 / 퍼짐 증가" 역할만 담당
	 */
	virtual void Fire(bool bIsADS);

	/**
	 * Spread(확산)를 적용한 방향 벡터 계산
	 * @param ForwardVector - 카메라나 총구의 정방향 벡터
	 * @return 퍼짐이 적용된 새로운 방향 벡터
	 */
	FVector ApplyBulletSpread(const FVector& ForwardVector) const;
};
