#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WeaponBase.h"
#include "MyCharacter.generated.h"

UCLASS()
class YOURGAME_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/** 최대 무기 슬롯 3개 */
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TArray<TSubclassOf<AWeaponBase>> DefaultWeapons;

	/** 실제 스폰된 무기 오브젝트 */
	UPROPERTY()
	TArray<AWeaponBase*> WeaponSlots;

	/** 현재 장착된 무기 */
	int32 CurrentWeaponIndex = -1;

public:
	/** 무기 선택 */
	void SelectWeapon(int32 Index);

	/** 무기 추가 함수 (상자 루팅 등 적용 가능) */
	void AddWeaponToSlot(TSubclassOf<AWeaponBase> NewWeapon, int32 SlotIndex);

	/** 공격 함수 → 현재 무기 Fire 호출 */
	void StartFire();
};
