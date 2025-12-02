// RecoilComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Curves/CurveFloat.h"      // UCurveFloat 사용을 위한 헤더
#include "RecoilComponent.generated.h"

/**
 *  URecoilComponent
 *  - 총기 발사 시 카메라/컨트롤러에 반동(Pitch/Yaw)을 적용하는 컴포넌트
 *  - 캐릭터(또는 무기)에 붙여서 사용
 *  - 연속 사격 시 RecoilIndex를 증가시키고, 이를 CurveFloat의 인덱스로 사용
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class YOURGAME_API URecoilComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// 생성자
	URecoilComponent();

protected:
	// 게임 시작 시 한 번 호출
	virtual void BeginPlay() override;

	// 반동 인덱스를 일정 시간 뒤에 초기화하기 위한 타이머 핸들
	FTimerHandle RecoilResetTimer;

public:

	/** 
	 * 현재 반동 인덱스 (총을 몇 번 연속으로 쐈는지에 대한 카운트 개념)
	 * - Curve의 X축 값으로 사용
	 * - 예: 0발째, 1발째, 2발째… 이런 식으로 커브 상에서 다른 값 추출
	 */
	int32 RecoilIndex;

	/**
	 * Pitch(상하) 반동 곡선
	 * - X축: RecoilIndex (샷 횟수)
	 * - Y축: Shot마다 적용할 Pitch 값 크기
	 * - 예: 첫발은 1도, 두번째는 1.5도, 세번째는 2도 이런 식으로 커브로 제어
	 */
	UPROPERTY(EditAnywhere, Category = "Recoil")
	UCurveFloat* RecoilPitchCurve;

	/**
	 * Yaw(좌우) 반동 곡선
	 * - X축: RecoilIndex
	 * - Y축: 좌우로 틀어질 값 (좌/우 랜덤이 아니라 패턴을 만들고 싶을 때 사용)
	 */
	UPROPERTY(EditAnywhere, Category = "Recoil")
	UCurveFloat* RecoilYawCurve;

	/**
	 * ADS(조준 모드)일 때 반동을 줄여주는 배율
	 * - Character 쪽에서 조준 여부에 따라 이 값을 곱해주도록 확장 가능
	 * - 기본값 0.6 -> 40% 감소
	 */
	UPROPERTY(EditAnywhere, Category = "Recoil")
	float ADSRecoilMultiplier = 0.6f;

	/** 실제 반동을 적용하는 함수 (총 발사 시 호출) */
	void ApplyRecoil(bool bIsADS = false);

	/** 일정 시간 연사하지 않을 경우 RecoilIndex를 0으로 초기화 */
	void ResetRecoil();

	/** 현재 RecoilIndex에 해당하는 Pitch 반동 값을 커브에서 가져오기 */
	float GetPitchRecoilValue() const;

	/** 현재 RecoilIndex에 해당하는 Yaw 반동 값을 커브에서 가져오기 */
	float GetYawRecoilValue() const;
};
