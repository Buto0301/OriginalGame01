// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
/**#include "Timermanager.h"*/
#include "adauchiCharacter.generated.h"

class AEnemyCharacter;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class AadauchiCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	/**LightPunch Input Action*/ 
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LightPunchAction;

	/**LightKick Input Action*/
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LightKickAction;


public:

	/** Constructor */
	AadauchiCharacter();	

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);


public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	/** LightPunch */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoLightPunch();

	/** LightKick */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoLightKick();

	/** Attacking hitbox activate*/
	void AttackHitCheck();


	



public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }




private:
	/** Attacking State **/
	bool bIsAttacking = false;

	/**TimeManagerを使用して、状態の終了を管理する(タイマーからDelegate型に変更したので、もう使わない)**/
	/**FTimerHandle AttackTimerHandle;*/

	/** 攻撃終了Delegateで呼び出す、実際に攻撃を終了させる関数*/
	void EndAttack();

	/** puhchconbo_montage*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> LightPunchMontage;

	/** Attack Ended state*/
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted );

	/**攻撃開始時に周囲から対称を探す */
	AEnemyCharacter* FindAttackTarget() const;

	/**現在の攻撃対象*/
	TWeakObjectPtr<AEnemyCharacter> AttackTarget();

};

