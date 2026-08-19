// Copyright Epic Games, Inc. All Rights Reserved.

#include "adauchiCharacter.h"
#include "EnemyCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "adauchi.h"
#include "DrawDebugHelpers.h"
#include "Components/SkeletalMeshComponent.h"
#include "CollisionQueryParams.h"
#include "CollisionShape.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"




AadauchiCharacter::AadauchiCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AadauchiCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AadauchiCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AadauchiCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AadauchiCharacter::Look);

		//LightPunch
		EnhancedInputComponent->BindAction(LightPunchAction, ETriggerEvent::Started, this, &AadauchiCharacter::DoLightPunch);

		//LightKick
		EnhancedInputComponent->BindAction(LightKickAction, ETriggerEvent::Started, this, &AadauchiCharacter::DoLightKick);

	}
	else
	{
		UE_LOG(Logadauchi, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AadauchiCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AadauchiCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AadauchiCharacter::DoMove(float Right, float Forward)
{
	/** if Character is attaking, cannot move.*/
	if (bIsAttacking) {
		return;
	}
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AadauchiCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AadauchiCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AadauchiCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void AadauchiCharacter::DoLightPunch()
{
	

		if (bIsAttacking) {
			return;
		}
		/**Timer
		GetWorldTimerManager().SetTimer(
			AttackTimerHandle,
			this,
			&AadauchiCharacter::EndAttack,
			0.4f,
			false
		);
		*/
		AttackTarget = FindAttackTarget();

		if (AttackTarget.IsValid()) {
			UE_LOG(
			LogTemp, 
			Warning, 
			TEXT("AttackTarget selected: %s"),
			*AttackTarget->GetName());

			FaceAttackTarget();

		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AttackTarget was not found."));
		}
		//Attack State On
		bIsAttacking = true;

		//Montage Play
		if (LightPunchMontage) {
			

			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

			FOnMontageEnded EndDelegate;

			EndDelegate.BindUObject(
				this,
				&AadauchiCharacter::OnAttackMontageEnded
			);

			
			PlayAnimMontage(LightPunchMontage);

			AnimInstance->Montage_SetEndDelegate(
				EndDelegate,
				LightPunchMontage
			);
			/** Use for debug
			if (AnimInstance)
			{
				UE_LOG(LogTemp, Warning, TEXT("AnimInstance OK"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("AnimInstance is NULL!"));
			}

			UE_LOG(LogTemp, Warning, TEXT("Delegate Bind OK"));

			UE_LOG(LogTemp, Warning, TEXT("Montage End Delegate Set"));
			*/
				
		}

		/** Use for debug*/
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(
				-1,
				5.0f,
				FColor::Yellow,
				TEXT("LP Attack!")
				
			);
		}
}

void AadauchiCharacter::DoLightKick()
{
	UE_LOG(LogTemp, Warning, TEXT("LK Attack!"));
		
		if (bIsAttacking) {
			return ;
		}

		bIsAttacking = true;
		
		/**Delegateを使うのでもう使わない。
		GetWorldTimerManager().SetTimer(
			AttackTimerHandle,
			this,
			&AadauchiCharacter::EndAttack,
			0.4f,
			false

		);
		*/
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(
				-1,
				5.0f,
				FColor::Blue,
				TEXT("LK Attack!")
			);
		}
		
	
	
}

void AadauchiCharacter::EndAttack() {
	bIsAttacking = false;
}

void AadauchiCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted) {
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Montage Ended: %s / Interrupted = %s"),
		*GetNameSafe(Montage),
		bInterrupted ? TEXT("true") : TEXT("false")
		);


	EndAttack();

}

/** Attacking Hit Check*/
void AadauchiCharacter::AttackHitCheck() {
	UE_LOG(LogTemp, Warning, TEXT("AttackHitCheck called."));

	const FName AttackSocketName(TEXT("hand_l"));

	if (!GetMesh() || !GetMesh()->DoesSocketExist(AttackSocketName)) {
		UE_LOG(
			LogTemp, 
			Error, 
			TEXT("AttackSocket was not found")
			);

		return;
	}

	const FVector Start = GetMesh()->GetSocketLocation(AttackSocketName);
	const float AttackDistance = 40.0f;
	const float AttackRadius = 25.0f;
	
	const FVector End = Start + GetActorForwardVector() * AttackDistance;

	FHitResult HitResult;
	FCollisionQueryParams  QueryParams;
	QueryParams.AddIgnoredActor(this);

	const FCollisionShape AttackShape = FCollisionShape::MakeSphere(AttackRadius);

	const FCollisionObjectQueryParams ObjectQueryParams(ECC_Pawn);

	const bool bHit = GetWorld()->SweepSingleByObjectType(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		ObjectQueryParams,
		AttackShape,
		QueryParams
	);

	const FColor DebugColor = bHit ? FColor::Green : FColor::Red;

	/**Display Start Sphere*/
	DrawDebugSphere(
		GetWorld(),
		Start,
		8.0f,
		12,
		FColor::Blue,
		false,
		2.0f
	);
	/**Display End Sphere*/
	DrawDebugSphere(
		GetWorld(),
		End,
		AttackRadius,
		16,
		DebugColor,
		false,
		2.0f
	);

	/**Display Line (Start to End)*/
	DrawDebugLine(
		GetWorld(),
		Start,
		End,
		DebugColor,
		false,
		2.0f,
		0,
		3.0f

	);

	if (bHit && HitResult.GetActor()) {
		AActor* HitActor = HitResult.GetActor();

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Attack hit: %s"),
			*HitResult.GetActor()->GetName()
		);

		const float DamageAmount = 10.0f;
		
		const float AppliedDamage = UGameplayStatics::ApplyDamage(
			HitActor,
			DamageAmount,
			GetController(),
			this,
			UDamageType::StaticClass()

		);

		UE_LOG(LogTemp, Warning, TEXT("Applied Damage: %.1f"), AppliedDamage);
	} 
	else {
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Attack missed.")
			);
	}

}

AEnemyCharacter* AadauchiCharacter::FindAttackTarget() const{
	constexpr float SearchRadius = 200.0f;

	TArray<FOverlapResult> OverlapResults;

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	UWorld* World = GetWorld();

	if (!World) {
		return nullptr;
	}

	const bool bFoundOverlap = World->OverlapMultiByObjectType(
		OverlapResults,
		GetActorLocation(),
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(SearchRadius),
		QueryParams
	);

	DrawDebugSphere(
		World,
		GetActorLocation(),
		SearchRadius,
		32,
		FColor::Blue,
		false,
		2.f
	);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Found Overlap %s / Overlap count: %d"),
		bFoundOverlap ? TEXT("True") : TEXT("False"),
		OverlapResults.Num()
		);

	TSet<AEnemyCharacter*> ProcessedEnemies;
	AEnemyCharacter* ClosestEnemy = nullptr;
	float ClosestDistanceSquared = TNumericLimits<float>::Max();
	for (const FOverlapResult& OverlapResult : OverlapResults) {
		AActor* OverlappedActor = OverlapResult.GetActor();

		AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(OverlappedActor);

		if (!EnemyCharacter) {
			continue;
		}

		if (ProcessedEnemies.Contains(EnemyCharacter)) {
			continue;
		}

		ProcessedEnemies.Add(EnemyCharacter);

		const FVector ForwardVector = GetActorForwardVector().GetSafeNormal2D();

		const FVector DirectionToEnemy = (EnemyCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();

		const float Dot = FVector::DotProduct(ForwardVector, DirectionToEnemy);

		if (Dot < 0.707f) {
			continue;
		}

		const float DistanceSquared = FVector::DistSquared2D(
			GetActorLocation(),
			EnemyCharacter->GetActorLocation()
		);

		if (DistanceSquared < ClosestDistanceSquared) {
			ClosestDistanceSquared = DistanceSquared;
			ClosestEnemy = EnemyCharacter;
		}



		UE_LOG(
			LogTemp, 
			Warning, 
			TEXT("Enemy Candidate found: %s"),
			*EnemyCharacter->GetName());

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Unique enemy count: %d"),
			ProcessedEnemies.Num()
		);
	}

	

	return ClosestEnemy;
}

void AadauchiCharacter::FaceAttackTarget() {
	if (!AttackTarget.IsValid()) {
		return;
	}

	FVector DirectionToTarget = AttackTarget->GetActorLocation() - GetActorLocation();

	//上下ではなく左右のみ調整する
	DirectionToTarget.Z = 0.0f;

	if (DirectionToTarget.IsNearlyZero()) {
		return;
	}

	const FRotator TargetRotation = DirectionToTarget.Rotation();

	SetActorRotation(
		FRotator(0.0f, TargetRotation.Yaw, 0.0f)
	);



}