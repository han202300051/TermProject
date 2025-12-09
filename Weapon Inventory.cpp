#include "MyCharacter.h"
#include "Kismet/GameplayStatics.h"

AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// WeaponSlots 기본 크기 4로 설정
	WeaponSlots.SetNum(3);
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 기본 무기 스폰
	for (int32 i = 0; i < DefaultWeapons.Num(); i++)
	{
		if (DefaultWeapons[i])
		{
			AddWeaponToSlot(DefaultWeapons[i], i);
		}
	}
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMyCharacter::StartFire);

	PlayerInputComponent->BindAction("Slot1", IE_Pressed, this, [this]() { SelectWeapon(0); });
	PlayerInputComponent->BindAction("Slot2", IE_Pressed, this, [this]() { SelectWeapon(1); });
	PlayerInputComponent->BindAction("Slot3", IE_Pressed, this, [this]() { SelectWeapon(2); });
}

void AMyCharacter::AddWeaponToSlot(TSubclassOf<AWeaponBase> NewWeapon, int32 SlotIndex)
{
	if (!NewWeapon || SlotIndex >= WeaponSlots.Num())
		return;

	UWorld* World = GetWorld();
	if (!World) return;

	AWeaponBase* SpawnedWeapon = World->SpawnActor<AWeaponBase>(NewWeapon);

	// 무기를 캐릭터 손에 붙이기
	SpawnedWeapon->AttachToComponent(GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		FName("WeaponSocket"));

	SpawnedWeapon->OnUnequip(); // 처음엔 숨김

	WeaponSlots[SlotIndex] = SpawnedWeapon;
}

void AMyCharacter::SelectWeapon(int32 Index)
{
	if (Index < 0 || Index >= WeaponSlots.Num()) return;

	// 같은 무기 선택 시 무시
	if (CurrentWeaponIndex == Index) return;

	// 이전 무기 숨기기
	if (CurrentWeaponIndex != -1 && WeaponSlots[CurrentWeaponIndex])
	{
		WeaponSlots[CurrentWeaponIndex]->OnUnequip();
	}

	// 새 무기 장착
	if (WeaponSlots[Index])
	{
		WeaponSlots[Index]->OnEquip();

		// 무기 장착 애니메이션을 넣고 싶으면 여기서 Montage 트리거
		// PlayAnimMontage(EquipMontage);

		CurrentWeaponIndex = Index;
	}
}

void AMyCharacter::StartFire()
{
	if (CurrentWeaponIndex != -1 && WeaponSlots[CurrentWeaponIndex])
	{
		WeaponSlots[CurrentWeaponIndex]->Fire();
	}
}
