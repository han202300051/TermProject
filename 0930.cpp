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
	PrimaryActorTick.bCanEverTick = true;

	// 카메라 컴포넌트 생성 및 루트에 부착
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);
	Camera->bUsePawnControlRotation = true;

	// 1인칭 전용 팔/손 메쉬
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetupAttachment(Camera);
	FirstPersonMesh->bOnlyOwnerSee = true;
	FirstPersonMesh->bCastDynamicShadow = false;
	FirstPersonMesh->CastShadow = false;

	GetMesh()->SetOwnerNoSee(true);

	// Sword Mesh
	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sword"));
	SwordMesh->SetupAttachment(GetMesh(), TEXT("Sword_Socket"));
}

// BeginPlay
void AInputCharacter::BeginPlay()
{
	Super::BeginPlay();

	SwordMesh->OnComponentBeginOverlap.AddDynamic(this, &AInputCharacter::OnSwordOverlapBegin);
}

// Tick
void AInputCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// 입력 바인딩
void AInputCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}

	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 이동, 시점, 점프
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AInputCharacter::Move);
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AInputCharacter::Look);
		Input->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AInputCharacter::Jump);

		// Dash / Slide → BP 이벤트 전달
		Input->BindAction(DashAction, ETriggerEvent::Started, this, &AInputCharacter::OnDashInput);
		Input->BindAction(SlideAction, ETriggerEvent::Started, this, &AInputCharacter::OnSlideInput);

		// 공격
		Input->BindAction(AttackAction, ETriggerEvent::Started, this, &AInputCharacter::Attack);
	}
}


// 이동 처리
void AInputCharacter::Move(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();

	if (IsValid(Controller))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, InputVector.Y);
		AddMovementInput(RightDirection, InputVector.X);
	}
}

// 마우스 Look 처리
void AInputCharacter::Look(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();
	if (IsValid(Controller))
	{
		AddControllerYawInput(InputVector.X);
		AddControllerPitchInput(InputVector.Y);
	}
}

// 점프 처리
void AInputCharacter::Jump()
{
	ACharacter::Jump();
}

// Dash 입력 → 블루프린트 이벤트
void AInputCharacter::OnDashInput(const FInputActionValue& Value)
{
	HandleDashInput();
}

// Slide 입력 → 블루프린트 이벤트
void AInputCharacter::OnSlideInput(const FInputActionValue& Value)
{
	HandleSlideInput();
}

// 공격 처리
void AInputCharacter::Attack()
{
	if (!bIsAttacking)
	{
		bIsAttacking = true;
		GetMesh()->GetAnimInstance()->Montage_Play(AttackAnimation);
	}
}

void AInputCharacter::OnSwordOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	if (bShouldDealDamage)
	{
		if (IsValid(OtherActor) && OtherActor != this)
		{
			OtherActor->Destroy();
		}
	}
}
