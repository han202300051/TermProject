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

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* AttackAction;

	// 1인칭 전용 팔/손 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* FirstPersonMesh;

	// Sword Mesh
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UStaticMeshComponent* SwordMesh;

public:
	AInputCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// 입력 바인딩 처리
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// 입력 처리 함수들
	void Move(const FInputActionValue& InputValue);
	void Look(const FInputActionValue& InputValue);
	void Jump();
	void Attack();

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackAnimation;

	// 블루프린트에서 구현 가능하게 이벤트로 선언
	UFUNCTION(BlueprintImplementableEvent, Category = "Movement")
	void HandleDashInput();

	UFUNCTION(BlueprintImplementableEvent, Category = "Movement")
	void HandleSlideInput();

	// C++에서 Input을 받아 BP 이벤트로 넘겨주는 함수
	void OnDashInput(const FInputActionValue& Value);
	void OnSlideInput(const FInputActionValue& Value);

	// Sword 충돌 확인
	UFUNCTION()
	void OnSwordOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(BlueprintReadWrite)
	bool bIsAttacking;

	UPROPERTY(BlueprintReadWrite)
	bool bShouldDealDamage = true; // melee 관련 제거 후 남김
};
