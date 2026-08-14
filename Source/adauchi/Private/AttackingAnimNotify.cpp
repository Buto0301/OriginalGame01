// Fill out your copyright notice in the Description page of Project Settings.

#include "AttackingAnimNotify.h"
#include "adauchiCharacter.h"

void UAttackingAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) {
	UE_LOG(LogTemp, Warning, TEXT("AttackingAnimNotify called"));
	Super::Notify(MeshComp, Animation);

	if (!MeshComp) {
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	if (AadauchiCharacter* Character = Cast<AadauchiCharacter>(Owner)){
		Character->AttackHitCheck();
	}
}