#include "Tutorial/InputPlayer/InputCharacter.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

// 캐릭터 클래스 기본 생성자
AInputCharacter::AInputCharacter()
{
	PrimaryActorTick.bCanEverTick = true; // 매 프레임 Tick() 호출 허용

	// 카메라 컴포넌트 생성 및 루트에 부착
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent); // 루트에 부착
	Camera->bUsePawnControlRotation = true; // 컨트롤러 회전에 따라 카메라 회전

	// 1인칭 전용 팔/손 메쉬 생성 및 카메라에 부착
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetupAttachment(Camera);
	FirstPersonMesh->bOnlyOwnerSee = true; // 본인만 보이도록 설정
	FirstPersonMesh->bCastDynamicShadow = false; // 그림자 비활성화
	FirstPersonMesh->CastShadow = false; // 그림자 비활성화

	GetMesh()->SetOwnerNoSee(true); // 캐릭터 본체는 본인에게 안보이도록 설정 (1인칭 전용)

	// Sword Mesh 생성 및 캐릭터 메쉬 소켓에 부착
	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sword"));
	SwordMesh->SetupAttachment(GetMesh(), TEXT("Sword_Socket")); // "Sword_Socket" 위치에 부착
}

// BeginPlay: 게임 시작 시 호출
void AInputCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 검 충돌 이벤트 바인딩
	SwordMesh->OnComponentBeginOverlap.AddDynamic(this, &AInputCharacter::OnSwordOverlapBegin);
}

// Tick: 매 프레임 호출
void AInputCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// 입력 바인딩
void AInputCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// PlayerController와 Enhanced Input 서브시스템 가져오기
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMapping, 0); // 입력 매핑 컨텍스트 등록
		}
	}

	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 이동, 시점, 점프 입력 바인딩
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AInputCharacter::Move);
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AInputCharacter::Look);
		Input->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AInputCharacter::Jump);

		// Dash / Slide → 블루프린트 이벤트로 전달
		Input->BindAction(DashAction, ETriggerEvent::Started, this, &AInputCharacter::OnDashInput);
		Input->BindAction(SlideAction, ETriggerEvent::Started, this, &AInputCharacter::OnSlideInput);

		// 공격 입력 바인딩
		Input->BindAction(AttackAction, ETriggerEvent::Started, this, &AInputCharacter::Attack);
	}
}

// 이동 처리
void AInputCharacter::Move(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>(); // 입력값(X: 좌/우, Y: 앞/뒤)

	if (IsValid(Controller))
	{
		const FRotator Rotation = Controller->GetControlRotation(); // 컨트롤러 회전
		const FRotator YawRotation(0, Rotation.Yaw, 0); // Yaw 회전만 사용

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); // 전방 방향
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y); // 우측 방향

		AddMovementInput(ForwardDirection, InputVector.Y); // 앞/뒤 이동
		AddMovementInput(RightDirection, InputVector.X);   // 좌/우 이동
	}
}

// 마우스 또는 스틱 Look 처리
void AInputCharacter::Look(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();
	if (IsValid(Controller))
	{
		AddControllerYawInput(InputVector.X);   // 좌우 회전
		AddControllerPitchInput(InputVector.Y); // 상하 회전
	}
}

// 점프 처리
void AInputCharacter::Jump()
{
	ACharacter::Jump(); // 기본 점프 함수 호출
}

// Dash 입력 → 블루프린트 이벤트 전달
void AInputCharacter::OnDashInput(const FInputActionValue& Value)
{
	HandleDashInput(); // BP에서 구현
}

// Slide 입력 → 블루프린트 이벤트 전달
void AInputCharacter::OnSlideInput(const FInputActionValue& Value)
{
	HandleSlideInput(); // BP에서 구현
}

// 공격 처리
void AInputCharacter::Attack()
{
	if (!bIsAttacking) // 이미 공격 중이 아니면
	{
		bIsAttacking = true; // 공격 상태로 변경
		GetMesh()->GetAnimInstance()->Montage_Play(AttackAnimation); // 공격 애니메이션 재생
	}
}

//공격
공격 중이 아닐 때만 애니메이션 재생.
bIsAttacking → 공격 중 상태 플래그.


// 검 충돌 처리
void AInputCharacter::OnSwordOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	if (bShouldDealDamage) // 데미지 적용 가능 상태일 때
	{
		if (IsValid(OtherActor) && OtherActor != this) // 다른 액터가 자신이 아니면
		{
			OtherActor->Destroy(); // 액터 제거(데미지 처리)
		}
	}
}

//검이 다른 액터와 겹치면 호출.
bShouldDealDamage가 true일 때만 적용.
다른 액터가 자신이 아닌 경우 → 해당 액터 삭제(데미지 처리)
