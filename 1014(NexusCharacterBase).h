#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "BasicAttributeSet.h"
#include "NexusCharacterBase.generated.h"

/**
 * GAS 기반 캐릭터의 기본 클래스
 * - AbilitySystemComponent 보유
 * - AttributeSet을 통한 능력치 관리
 */
UCLASS()
class NEXUS_API ANexusCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// 생성자
	ANexusCharacterBase();

	/** -----------------------------------
	 *  Ability System 구성 요소
	 *  ----------------------------------- */

	// 캐릭터의 Ability System Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UAbilitySystemComponent* AbilitySystemComponent;

	// 이 캐릭터의 능력치 집합 (체력, 스태미나 등)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	class UBasicAttributeSet* BasicAttributes;

protected:
	// Ability System 복제 모드 (서버-클라 동기화 방식 설정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilitySystem")
	EGameplayEffectReplicationMode AscReplicationMode = EGameplayEffectReplicationMode::Mixed;

protected:
	virtual void BeginPlay() override;              // 게임 시작 시
	virtual void PossessedBy(AController* NewController) override; // 서버에서 소유권 획득 시
	virtual void OnRep_PlayerState() override;      // 클라이언트에서 PlayerState 복제 시

public:
	virtual void Tick(float DeltaTime) override;    // 매 프레임 호출
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// GAS에서 요구하는 인터페이스 함수
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
};
