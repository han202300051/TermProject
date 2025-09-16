#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InputCharacter.generated.h"

UCLASS()
class TUTORIAL_API AInputCharacter : public ACharacter
{
	GENERATED_BODY()

	// 플레이어가 보는 카메라
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

protected:
	// 에디터에서 연결할 InputMappingContext (키/액션 바인딩 정보 들어있음)
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputMappingContext* InputMapping;

	// 이동 입력 (예: WASD, 스틱)
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* MoveAction;

	// 점프 입력 (예: Space)
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* JumpAction;

	// 시선 이동 입력 (예: 마우스, 패드 오른쪽 스틱)
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* LookAction;

public:
	// 생성자
	AInputCharacter();

protected:
	// 게임 시작 시 호출
	virtual void BeginPlay() override;

public:	
	// 매 프레임 호출
	virtual void Tick(float DeltaTime) override;

	// 입력 바인딩 (키/액션 연결) 처리
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// 이동 처리 함수
	void Move(const FInputActionValue& InputValue);

	// 시선 이동 처리 함수
	void Look(const FInputActionValue& InputValue);

	// 점프 처리 함수
	void Jump();
};
