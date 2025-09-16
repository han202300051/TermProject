#include "Tutorial/InputPlayer/InputCharacter.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"

// 기본값 설정
AInputCharacter::AInputCharacter()
{
	// Tick() 매 프레임 호출 가능하게 설정 (성능 위해 필요 없으면 끄기 가능)
	PrimaryActorTick.bCanEverTick = true;

	// 카메라 생성 및 루트컴포넌트에 붙이기
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(RootComponent);

	// 카메라가 컨트롤러 회전을 따라가도록 설정
	Camera->bUsePawnControlRotation = true;
}

// 게임 시작할 때 호출
void AInputCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// 매 프레임 호출 (현재는 특별한 처리 없음)
void AInputCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// 입력 바인딩 처리
void AInputCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 플레이어 컨트롤러 가져오기
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		// 로컬 플레이어의 EnhancedInputSubsystem 가져오기
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// InputMappingContext 등록 (실제 키/액션 매핑 활성화)
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}

	// EnhancedInputComponent로 캐스팅해서 액션 바인딩
	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 이동 액션 → Move 함수
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AInputCharacter::Move);

		// 시선 이동 액션 → Look 함수
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AInputCharacter::Look);

		// 점프 액션 → Jump 함수
		Input->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AInputCharacter::Jump);
	}
}

// 이동 처리
void AInputCharacter::Move(const FInputActionValue& InputValue)
{
	// 입력값 가져오기 (X: 좌우, Y: 앞뒤)
	FVector2D InputVector = InputValue.Get<FVector2D>();

	if (IsValid(Controller))
	{
		// 컨트롤러가 바라보는 방향 가져오기 (Yaw만 사용 → 상하 회전은 무시)
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 전/우 방향 벡터 구하기
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// 입력값에 맞게 이동
		AddMovementInput(ForwardDirection, InputVector.Y);
		AddMovementInput(RightDirection, InputVector.X);
	}
}

// 시선 이동 처리
void AInputCharacter::Look(const FInputActionValue& InputValue)
{
	// 입력값 가져오기 (X: 좌우, Y: 상하)
	FVector2D InputVector = InputValue.Get<FVector2D>();

	if (IsValid(Controller))
	{
		// 좌우 회전 (Yaw)
		AddControllerYawInput(InputVector.X);

		// 상하 회전 (Pitch)
		AddControllerPitchInput(InputVector.Y);
	}
}

// 점프 처리
void AInputCharacter::Jump()
{
	// ACharacter의 Jump 기능 호출
	ACharacter::Jump();
}
