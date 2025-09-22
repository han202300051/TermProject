#include "Tutorial/InputPlayer/InputCharacter.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// ──────────────────────────────────────────────
// 캐릭터 클래스 기본 생성자
// ──────────────────────────────────────────────
AInputCharacter::AInputCharacter()
{
	PrimaryActorTick.bCanEverTick = true; // 매 프레임 Tick 실행 허용

	// 카메라 컴포넌트 생성 및 루트에 부착
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(RootComponent);
	Camera->bUsePawnControlRotation = true; // 컨트롤러 회전에 따라 카메라 회전

	// 1인칭 전용 팔/손 메쉬 (플레이어 본인만 보임)
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetupAttachment(Camera);           // 카메라에 붙이기
	FirstPersonMesh->bOnlyOwnerSee = true;              // 본인만 보이게
	FirstPersonMesh->bCastDynamicShadow = false;		// 그림자 안 만듦
	FirstPersonMesh->CastShadow = false;

	// 풀바디 메쉬는 본인 시점에서 안 보이게
	GetMesh()->SetOwnerNoSee(true);
}

// ──────────────────────────────────────────────
// BeginPlay: 게임 시작 시 실행
// ──────────────────────────────────────────────
void AInputCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// ──────────────────────────────────────────────
// Tick: 매 프레임 실행
// ──────────────────────────────────────────────
void AInputCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// ──────────────────────────────────────────────
// 입력 시스템 초기화 (Enhanced Input 연결)
// ──────────────────────────────────────────────
void AInputCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 로컬 플레이어의 Input Subsystem 가져오기
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMapping, 0); // 입력 매핑 컨텍스트 추가
		}
	}

	// 입력 바인딩
	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 이동, 시점, 점프
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AInputCharacter::Move);
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AInputCharacter::Look);
		Input->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AInputCharacter::Jump);

		// Dash → 블루프린트 이벤트 전달
		Input->BindAction(DashAction, ETriggerEvent::Started, this, &AInputCharacter::OnDashInput);

		// Slide → 블루프린트 이벤트 전달
		Input->BindAction(SlideAction, ETriggerEvent::Started, this, &AInputCharacter::OnSlideInput);

		// 근접 공격
		Input->BindAction(MeleeAttackAction, ETriggerEvent::Started, this, &AInputCharacter::OnMeleeAttack);
	}
}

// ──────────────────────────────────────────────
// 이동 처리
// ──────────────────────────────────────────────
void AInputCharacter::Move(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();

	if (IsValid(Controller))
	{
		// 카메라가 바라보는 방향에서 Yaw만 가져오기
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 앞/옆 방향 벡터 계산
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// 입력값을 이동에 적용
		AddMovementInput(ForwardDirection, InputVector.Y);
		AddMovementInput(RightDirection, InputVector.X);
	}
}

// ──────────────────────────────────────────────
// 마우스 Look 처리 (Yaw / Pitch)
// ──────────────────────────────────────────────
void AInputCharacter::Look(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();

	if (IsValid(Controller))
	{
		AddControllerYawInput(InputVector.X);   // 좌우 회전
		AddControllerPitchInput(InputVector.Y); // 상하 회전
	}
}

// ──────────────────────────────────────────────
// 점프 처리
// ──────────────────────────────────────────────
void AInputCharacter::Jump()
{
	ACharacter::Jump(); // 부모의 점프 기능 그대로 사용
}

// ──────────────────────────────────────────────
// Dash 입력 → 블루프린트로 이벤트 넘김
// ──────────────────────────────────────────────
void AInputCharacter::OnDashInput(const FInputActionValue& Value)
{
	HandleDashInput(); // BP에서 구현
}

// ──────────────────────────────────────────────
// Slide 입력 → 블루프린트로 이벤트 넘김
// ──────────────────────────────────────────────
void AInputCharacter::OnSlideInput(const FInputActionValue& Value)
{
	HandleSlideInput(); // BP에서 구현
}

// ──────────────────────────────────────────────
// 근접 공격 입력 처리
// ──────────────────────────────────────────────
void AInputCharacter::OnMeleeAttack(const FInputActionValue& Value)
{
	PerformMeleeAttack();
}

// ──────────────────────────────────────────────
// 실제 공격 판정
// ──────────────────────────────────────────────
void AInputCharacter::PerformMeleeAttack()
{
	// 카메라 위치 → 전방으로 일정 거리
	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + (Camera->GetForwardVector() * MeleeRange);

	// 구체 충돌체 생성
	FCollisionShape Sphere = FCollisionShape::MakeSphere(MeleeRadius);

	// 충돌 파라미터 (자기 자신 제외)
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	// 구체 스윕 (Start → End)
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,   // Pawn 채널만 감지
		Sphere,
		Params
	);

	// Debug: 공격 범위를 빨간 구체로 표시
	DrawDebugSphere(GetWorld(), End, MeleeRadius, 16, FColor::Red, false, 1.0f);

	// 맞은 액터가 있다면 로그 출력 + 데미지 적용
	if (bHit && HitResult.GetActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName());

		// 데미지 적용 (BP 또는 C++ Damage 시스템과 연동 가능)
		UGameplayStatics::ApplyDamage(
			HitResult.GetActor(),
			MeleeDamage,     // 데미지 값
			GetController(), // 공격자 컨트롤러
			this,            // 데미지 유발한 액터 (본인)
			nullptr          // DamageType (기본)
		);
	}
}
