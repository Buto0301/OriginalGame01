// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	UE_LOG(LogTemp, Warning, TEXT("Enemy Health: %.1f / %.1f"), CurrentHealth, MaxHealth);
	
}

/**void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
*/


// Called to bind functionality to input
/**void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	vfr
}
*/
float AEnemyCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {
	const float ActualDamage = Super::TakeDamage(
		DamageAmount,
		DamageEvent,
		EventInstigator,
		DamageCauser
	);

	if (ActualDamage <= 0.0f) {
		return 0.0f;
	}

	CurrentHealth = FMath::Clamp(
		CurrentHealth - ActualDamage,
		0.0f,
		MaxHealth
	);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("%s took %.1f damage. Health: %.1f/%.1f"),
		*GetName(),
		ActualDamage,
		CurrentHealth,
		MaxHealth
	);

	if (CurrentHealth <= 0) {
		Die();
	}

	return ActualDamage;
}

void AEnemyCharacter::Die() {
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("%s died"),
		*GetName()
	);

	Destroy();

}