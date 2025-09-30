#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InputCharacter.generated.h"

UCLASS()
class TUTORIAL_API AInputCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	// 카메라 컴포넌트 (1인칭 시점)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	// Enhanced Input 시스템에서 사용할 입력 매핑 컨텍스트
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputMappingContext* InputMapping;

	// 기본 입력 액션들
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* MoveAction; // 이동
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* JumpAction; // 점프
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* LookAction; // 시점 이동

	// Dash / Slide 액션
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* DashAction;  // 대시
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* SlideAction; // 슬라이드
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* AttackAction; // 공격

	// 1인칭 전용 팔/손 메쉬 (본인만 보임)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* FirstPersonMesh;

	// Sword Mesh (충돌 감지용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UStaticMeshComponent* SwordMesh;

public:
	// 생성자
	AInputCharacter();

protected:
	// 게임 시작 시 호출
	virtual void BeginPlay() override;

public:
	// 매 프레임 호출
	virtual void Tick(float DeltaTime) override;

	// 입력 바인딩 처리
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// 입력 처리 함수들
	void Move(const FInputActionValue& InputValue); // 이동
	void Look(const FInputActionValue& InputValue); // 시점 이동
	void Jump();                                    // 점프
	void Attack();                                  // 공격

	// 공격 애니메이션
	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackAnimation;

	// Dash / Slide 블루프린트 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Movement")
	void HandleDashInput();  // BP에서 Dash 구현
	UFUNCTION(BlueprintImplementableEvent, Category = "Movement")
	void HandleSlideInput(); // BP에서 Slide 구현

	// C++에서 Input을 받아 BP 이벤트로 넘겨주는 함수
	void OnDashInput(const FInputActionValue& Value);
	void OnSlideInput(const FInputActionValue& Value);

	// Sword 충돌 처리
	UFUNCTION()
	void OnSwordOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	// 공격 상태 플래그
	UPROPERTY(BlueprintReadWrite)
	bool bIsAttacking;

	// 검 공격 데미지 적용 여부
	UPROPERTY(BlueprintReadWrite)
	bool bShouldDealDamage = true; //
};
