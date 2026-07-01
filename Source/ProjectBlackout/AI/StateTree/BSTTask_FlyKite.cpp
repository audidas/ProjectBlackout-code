#include "AI/StateTree/BSTTask_FlyKite.h"
#include "StateTreeExecutionContext.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "BlackoutGameplayTags.h"

EStateTreeRunStatus FBSTTask_FlyKite::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);
	const UWorld* World = Context.GetWorld();
	const float Now = World ? World->GetTimeSeconds() : 0.0f;

	Data.bOrbitRight = FMath::RandBool();
	Data.NextReverseTime = Now + FMath::RandRange(Data.ReverseIntervalMin, Data.ReverseIntervalMax);
	Data.CurrentHeightOffset = FMath::RandRange(Data.HeightMin, Data.HeightMax);

	// 선회 각도를 현재 보스->타겟 방향으로 초기화
	const APawn* Pawn = Data.Controller ? Data.Controller->GetPawn() : nullptr;
	if (Pawn && Data.Target)
	{
		const FVector Offset = Pawn->GetActorLocation() - Data.Target->GetActorLocation();
		Data.OrbitAngle = FMath::Atan2(Offset.Y, Offset.X);
	}
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FBSTTask_FlyKite::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);

	APawn* Pawn = Data.Controller ? Data.Controller->GetPawn() : nullptr;
	const UWorld* World = Context.GetWorld();
	if (!Pawn || !Data.Target || !World)
	{
		return EStateTreeRunStatus::Running; // 호버 대기
	}

	
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn))
	{
		if (ASC->HasMatchingGameplayTag(BlackoutGameplayTags::State_MovementLocked))
		{
			return EStateTreeRunStatus::Running;
		}
	}

	const float Now = World->GetTimeSeconds();

	// 선회 방향 반전
	if (Now >= Data.NextReverseTime)
	{
		Data.bOrbitRight = !Data.bOrbitRight;
		Data.NextReverseTime = Now + FMath::RandRange(Data.ReverseIntervalMin, Data.ReverseIntervalMax);
		Data.CurrentHeightOffset = FMath::RandRange(Data.HeightMin, Data.HeightMax);
	}

	// 선회 각도 진행
	const float Dir = Data.bOrbitRight ? 1.0f : -1.0f;
	Data.OrbitAngle += Dir * FMath::DegreesToRadians(Data.OrbitAngularSpeed) * DeltaTime;

	// 가변 반경
	const float Radius = Data.DesireRange + Data.RadiusVarAmplitude * FMath::Sin(Now * Data.RadiusVarFrequency);

	// 목표점 = 타겟 주위 궤도회전 + 부유 
	const FVector TargetLoc = Data.Target->GetActorLocation();
	FVector Desired = TargetLoc;
	Desired.X += FMath::Cos(Data.OrbitAngle) * Radius;
	Desired.Y += FMath::Sin(Data.OrbitAngle) * Radius;
	Desired.Z = TargetLoc.Z + Data.CurrentHeightOffset + Data.BobAmplitude * FMath::Sin(Now * Data.BobFrequency);

	// 단일 방향 스티어링 (반경/선회/고도가 하나로)
	const FVector ToDesired = Desired - Pawn->GetActorLocation();
	Pawn->AddMovementInput(ToDesired.GetSafeNormal());

	return EStateTreeRunStatus::Running;
}
