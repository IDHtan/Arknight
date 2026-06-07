// 干员 Sprite Sheet 动画组件 — 实现

#include "SpritesheetAnimComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

USpriteSheetAnimComponent::USpriteSheetAnimComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USpriteSheetAnimComponent::BeginPlay()
{
	Super::BeginPlay();

	// 从 Owner 找 SpritePlane
	AActor* Owner = GetOwner();
	if (!Owner) return;

	SpritePlaneRef = Owner->FindComponentByClass<UStaticMeshComponent>();
	if (!SpritePlaneRef) return;

	// 创建动态材质实例
	if (BaseMaterial)
	{
		DynamicMaterial = SpritePlaneRef->CreateDynamicMaterialInstance(0, BaseMaterial);
	}

	if (!DynamicMaterial) return;

	// 初始参数
	if (SpriteSheetTexture)
	{
		DynamicMaterial->SetTextureParameterValue(TEXT("SpriteTexture"), SpriteSheetTexture);
	}
	DynamicMaterial->SetScalarParameterValue(TEXT("Columns"), (float)SheetColumns);
	DynamicMaterial->SetScalarParameterValue(TEXT("Rows"), (float)SheetRows);
	DynamicMaterial->SetScalarParameterValue(TEXT("FrameIndex"), 0.0f);
	DynamicMaterial->SetScalarParameterValue(TEXT("FlipU"), 0.0f);
}

void USpriteSheetAnimComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsPlaying) return;
	if (!DynamicMaterial) return;
	if (ActiveClip.FrameCount <= 0) return;

	// 帧推进
	const float FrameDuration = 1.0f / FMath::Max(ActiveClip.FramesPerSecond, 1.0f);
	FrameAccumulator += DeltaTime;

	if (FrameAccumulator >= FrameDuration)
	{
		FrameAccumulator -= FrameDuration;
		ClipPlayHead++;

		if (ClipPlayHead >= ActiveClip.FrameCount)
		{
			OnClipFinished();
		}
		else
		{
			CurrentFrameIndex = ActiveClip.StartFrame + ClipPlayHead;
			SetMaterialFrame(CurrentFrameIndex);
		}
	}
}

void USpriteSheetAnimComponent::PlayAnimation(FName ClipName)
{
	if (!Clips.Contains(ClipName)) return;

	// 如果是同一个 Clip 且已经在播，不重播（避免闪烁）
	if (CurrentClipName == ClipName && bIsPlaying) return;

	ActiveClip = Clips[ClipName];
	CurrentClipName = ClipName;
	ClipPlayHead = 0;
	FrameAccumulator = 0.0f;
	bIsPlaying = true;

	CurrentFrameIndex = ActiveClip.StartFrame;
	SetMaterialFrame(CurrentFrameIndex);
}

void USpriteSheetAnimComponent::SetAttacking(bool bInAttacking)
{
	bIsAttacking = bInAttacking;
}

void USpriteSheetAnimComponent::SetFlipU(bool bFlipped)
{
	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(TEXT("FlipU"), bFlipped ? 1.0f : 0.0f);
	}
}

void USpriteSheetAnimComponent::SetMaterialFrame(int32 FrameIndex)
{
	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(TEXT("FrameIndex"), (float)FrameIndex);
	}
}

void USpriteSheetAnimComponent::OnClipFinished()
{
	// 不循环 + 仍在攻击状态 → 重播当前 Attack Clip
	if (!ActiveClip.bLoop && bIsAttacking)
	{
		ClipPlayHead = 0;
		FrameAccumulator = 0.0f;
		CurrentFrameIndex = ActiveClip.StartFrame;
		SetMaterialFrame(CurrentFrameIndex);
		return;
	}

	// 循环 Clip → 回到开头继续
	if (ActiveClip.bLoop)
	{
		ClipPlayHead = 0;
		FrameAccumulator = 0.0f;
		CurrentFrameIndex = ActiveClip.StartFrame;
		SetMaterialFrame(CurrentFrameIndex);
		return;
	}

	// 有后续 Clip → 自动切
	if (!ActiveClip.NextClipOnFinish.IsNone() && Clips.Contains(ActiveClip.NextClipOnFinish))
	{
		PlayAnimation(ActiveClip.NextClipOnFinish);
		return;
	}

	// 停在最后一帧
	bIsPlaying = false;
}
