#include "NexusCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Nexus/GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h"

// Sets default values
ANexusCharacterBase::ANexusCharacterBase()
{
	// 매 프레임 Tick을 호출하도록 설정.
	// 성능을 고려해 필요 없으면 false로 변경 가능.
	PrimaryActorTick.bCanEverTick = true;

	// -------------------------
	// Ability System Component
	// -------------------------
	// Ability System의 핵심 컴포넌트 생성.
	// 이 컴포넌트를 통해 GameplayAbility, GameplayEffect, Attribute를 관리한다.
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	// ASC(AbilitySystemComponent)를 네트워크로 복제하도록 설정.
	// 기본적으로 ASC는 서버/클라이언트 간에 동기화되어야 하므로 true.
	AbilitySystemComponent->SetIsReplicated(true);

	// ASC의 복제 모드 설정. (헤더의 AscReplicationMode 값을 사용)
	// ReplicationMode에 따라 GameplayEffects/Abilities의 동기화 상세 동작이 달라짐.
	AbilitySystemComponent->SetReplicationMode(AscReplicationMode);

	// -------------------------
	// Collision (Capsule)
	// -------------------------
	// 캡슐 콜리전 크기 초기화: 반지름, 높이 순서
	// 게임 플레이 여부와 충돌/물리 동작에 직접적인 영향이 있다.
	GetCapsuleComponent()->InitCapsuleSize(35.0f, 90.0f);
	
	// -------------------------
	// Rotation 및 Camera 관련
	// -------------------------
	// 컨트롤러의 회전이 Pawn의 회전에 직접 적용되지 않도록 설정.
	// 보통 3인칭 게임에서는 CharacterMovement의 방향 기준 이동을 사용하기 때문에 false로 둠.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// -------------------------
	// Movement 설정
	// -------------------------
	// 이동 방향에 맞춰 캐릭터를 회전시킴 (즉, 조작한 방향을 바라보게 함).
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// 회전 속도 (Yaw)
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// 점프 힘
	GetCharacterMovement()->JumpZVelocity = 500.0f;

	// 공중 컨트롤 (0~1)
	GetCharacterMovement()->AirControl = 0.35f;

	// 기본 이동 속도
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;

	// 아날로그 스틱을 살짝 움직였을 때의 최소 속도
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;

	// 지상에서 브레이킹(감속) 값 — 멈추는 감도가 달라짐
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

	// 떨어질 때의 감속 값
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// -------------------------
	// AttributeSet 생성
	// -------------------------
	// BasicAttributes는 UAttributeSet 기반 객체. CreateDefaultSubobject으로 생성.
	// (참고: 이 방법은 흔히 보이는 패턴이며, 런타임에 AbilitySystemComponent가 소유하도록 NewObject로 생성하는 패턴도 있음)
	BasicAttributes = CreateDefaultSubobject<UBasicAttributeSet>(TEXT("BasicAttributeSet"));
}

// Called when the game starts or when spawned
void ANexusCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// BeginPlay 시점에서는 서버/클라이언트의 소유권(Owner)과 PlayerState 복제 상태가
	// 아직 완전히 세팅되지 않았을 수 있음. 따라서 AbilityActorInfo 초기화는
	// PossessedBy(서버) / OnRep_PlayerState(클라이언트)에서 수행하는 것이 안전.
}


// Called every frame
void ANexusCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 매 프레임 처리할 로직이 있다면 여기에 추가.
	// (예: 스태미나 자동 회복 처리 등. 다만 GAS에서는 GameplayEffect로 처리하는 것이 권장)
}

// Called to bind functionality to input
void ANexusCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 입력 바인딩을 여기에 추가 (예: Move, Look, Jump 등)
	// 예:
	// PlayerInputComponent->BindAxis("MoveForward", this, &ANexusCharacterBase::MoveForward);
}

// PossessedBy는 서버에서 Pawn의 소유자가 바뀔 때 호출된다.
// 서버가 소유자(Controller)를 갖게 되는 시점에 AbilitySystem을 초기화한다.
void ANexusCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// AbilitySystemComponent가 존재하면 AbilityActorInfo를 초기화.
	// InitAbilityActorInfo(OwnerActor, AvatarActor)
	// - OwnerActor: 보통 PlayerState 또는 Controller (능력 소유자)
	// - AvatarActor: 실제 능력을 수행하는 Actor (대개 this)
	//
	// 여기서는 간단히 this, this로 초기화하고 있으나,
	// 멀티플레이/소유권을 정확히 구분하려면 OwnerActor에 PlayerState를 넣는 것이 권장됨.
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

// 클라이언트에서 PlayerState가 복제되어 들어왔을 때 호출.
// 클라이언트 쪽에서 AbilitySystem을 초기화하기 위해 사용.
void ANexusCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// PossessedBy는 서버에서 호출되므로 클라이언트에서는 OnRep_PlayerState에서 ASC 초기화.
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

// IAbilitySystemInterface 구현: GAS가 이 캐릭터의 ASC에 접근할 때 사용.
UAbilitySystemComponent* ANexusCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
