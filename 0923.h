
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

	// 카메라 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	// 입력 매핑 컨텍스트 (Enhanced Input)
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputMappingContext* InputMapping;

	// 기본 액션
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* LookAction;

	// Dash / Slide 액션
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* DashAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* SlideAction;


	// --- 1인칭 전용 팔/손 메쉬 ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* FirstPersonMesh;

	// 공격 애니메이션 (Montage)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	UAnimMontage* MeleeAttackMontage;


public:
	// 기본 생성자
	AInputCharacter();

protected:
	// 게임 시작시 호출
	virtual void BeginPlay() override;

public:	
	// 매 프레임마다 호출
	virtual void Tick(float DeltaTime) override;

	// 입력 바인딩 처리
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	// 입력 처리 함수들
	void Move(const FInputActionValue& InputValue);
	void Look(const FInputActionValue& InputValue);
	void Jump();

	// 블루프린트에서 구현 가능하게 이벤트로 선언
	UFUNCTION(BlueprintImplementableEvent, Category = "Movement")
	void HandleDashInput();

	UFUNCTION(BlueprintImplementableEvent, Category = "Movement")
	void HandleSlideInput();


	// C++에서 Input을 받아 BP 이벤트로 넘겨주는 함수
	void OnDashInput(const FInputActionValue& Value);
	void OnSlideInput(const FInputActionValue& Value);

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// --- 공격 액션 ---
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* MeleeAttackAction;

	// --- 공격 함수 ---
	void OnMeleeAttack(const FInputActionValue& Value);

	// --- 공격 판정 ---
	void PerformMeleeAttack();

	// --- 공격 범위/거리 ---
	UPROPERTY(EditAnywhere, Category = "Combat")
	float MeleeRange = 200.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float MeleeRadius = 50.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float MeleeDamage = 20.f;
};
