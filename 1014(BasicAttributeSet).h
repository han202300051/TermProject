// Fill out your copyright notice...
#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"                    // UAttributeSet 기반 클래스
#include "AbilitySystemComponent.h"           // GAS의 핵심 컴포넌트
#include "BasicAttributeSet.generated.h"

/**
 * Gameplay Ability System의 속성을 정의하는 클래스
 * 이 클래스는 체력(Health), 스태미나(Stamina) 등 플레이어의 능력치 데이터를 관리함.
 * 모든 Attribute는 클라이언트-서버 간 동기화(Replication)를 지원함.
 */
UCLASS()
class NEXUS_API UBasicAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	// 생성자
	UBasicAttributeSet();

	/** -----------------------------------
	 *  Health 관련 속성
	 *  ----------------------------------- */
	
	// 현재 체력
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing=OnRep_Health)
	FGameplayAttributeData Health;
	// Health 속성 접근 매크로 (Getter, Setter, Init 등 생성)
	ATTRIBUTE_ACCESSORS_BASIC(UBasicAttributeSet, Health);

	// 최대 체력
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing=OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS_BASIC(UBasicAttributeSet, MaxHealth);

	/** -----------------------------------
	 *  Stamina 관련 속성
	 *  ----------------------------------- */

	// 현재 스태미나
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing=OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS_BASIC(UBasicAttributeSet, Stamina);

	// 최대 스태미나
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing=OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS_BASIC(UBasicAttributeSet, MaxStamina);


public:
	/** -----------------------------------
	 *  OnRep Functions (서버 → 클라이언트 동기화)
	 *  속성 값이 변경될 때 클라이언트에 반영
	 *  ----------------------------------- */

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, Health, OldValue);
	}

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, MaxHealth, OldValue);
	}

	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, Stamina, OldValue);
	}

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, MaxStamina, OldValue);
	}

	/** -----------------------------------
	 *  Unreal Networking: 속성 복제 등록
	 *  ----------------------------------- */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** -----------------------------------
	 *  속성 값이 변경되기 전에 호출
	 *  예: 체력 값이 MaxHealth보다 커지지 않도록 제한
	 *  ----------------------------------- */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/** -----------------------------------
	 *  GameplayEffect 실행 후 호출
	 *  예: 체력 감소, 회복 등의 효과 반영
	 *  ----------------------------------- */
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
};
