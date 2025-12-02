// RecoilComponent.cpp

#include "RecoilComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

URecoilComponent::URecoilComponent()
{
	// Tick은 필요 없으니 꺼두기 (성능 절약)
	PrimaryComponentTick.bCanEverTick = false;

	// 시작 시 반동 인덱스 0
	RecoilIndex = 0;
}

void URecoilComponent::BeginPlay()
{
	Super::BeginPlay();
}

/** 
 * 커브를 사용해서 Pitch 반동 값을 가져오는 함수
 * - Curve가 없으면 0 리턴 (반동 없음)
 * - X축 기반: RecoilIndex를 float로 캐스팅해서 사용
 */
float URecoilComponent::GetPitchRecoilValue() const
{
	if (!RecoilPitchCurve) return 0.f;

	const float XValue = static_cast<float>(RecoilIndex);
	return RecoilPitchCurve->GetFloatValue(XValue);
}

/**
 * 커브를 사용해서 Yaw 반동 값을 가져오는 함수
 */
float URecoilComponent::GetYawRecoilValue() const
{
	if (!RecoilYawCurve) return 0.f;

	const float XValue = static_cast<float>(RecoilIndex);
	return RecoilYawCurve->GetFloatValue(XValue);
}

/**
 * 실제 반동 적용 함수
 * - 총 발사 시 Weapon이나 Character에서 호출
 * - 카메라를 직접 회전시키는 것이 아니라
 *   PlayerController의 AddPitchInput / AddYawInput을 통해 입력처럼 처리
 */
void URecoilComponent::ApplyRecoil(bool bIsADS /*= false*/)
{
	// 이 컴포넌트가 붙어있는 액터의 컨트롤러를 가져옴
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;

	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PC) return;

	// 커브에서 반동 값 가져오기
	float PitchValue = GetPitchRecoilValue();
	float YawValue   = GetYawRecoilValue();

	// ADS 상태라면 반동을 줄여주기 (조준 시 더 안정적)
	if (bIsADS)
	{
		PitchValue *= ADSRecoilMultiplier;
		YawValue   *= ADSRecoilMultiplier;
	}

	/**
	 * 언리얼 기준:
	 * - AddPitchInput(+): 보통 위/아래 카메라 회전 입력
	 * - 총기 반동은 보통 "위로 튀는" 느낌이므로
	 *   카메라가 위로 올라가게 하려면 Pitch를 음수(-)로 주는게 일반적 (프로젝트 세팅에 따라 다름)
	 *   여기선 "위로 튄다" = 화면이 위를 보게 된다 = Pitch 감소 -> -PitchValue 사용
	 */
	PC->AddPitchInput(-PitchValue * 0.5f); // 0.5f는 튜닝용 스케일
	PC->AddYawInput(YawValue * 0.5f);

	// 연속 사격을 위해 반동 인덱스 증가
	RecoilIndex++;

	// 일정 시간 동안 사격이 없으면 반동 패턴을 리셋
	// -> 연사 중에는 계속 타이머 리셋되면서 유지됨
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(RecoilResetTimer);
		GetWorld()->GetTimerManager().SetTimer(
			RecoilResetTimer,
			this,
			&URecoilComponent::ResetRecoil,
			0.2f,        // 0.2초 동안 사격이 없으면 리셋
			false
		);
	}
}

/** 
 * RecoilIndex를 0으로 돌려 초기 상태로 만드는 함수
 * - 일정 시간 동안 사격이 없을 때 호출됨
 * - 이로 인해 다시 첫발 반동 패턴부터 시작
 */
void URecoilComponent::ResetRecoil()
{
	RecoilIndex = 0;
}
