// 干员 Sprite Sheet 动画组件
// 负责帧推进、Clip 切换、MID 参数写入。视觉载体（Plane）归 Actor 所有。

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpritesheetAnimComponent.generated.h"

// 单段动画的描述
USTRUCT(BlueprintType)
struct FSpritesheetAnimClip
{
	GENERATED_BODY()

	// 在 Sprite Sheet 中的起始帧号 (0-based)
	UPROPERTY(EditDefaultsOnly, Category = "Clip")
	int32 StartFrame = 0;

	// 这段动画一共有几帧
	UPROPERTY(EditDefaultsOnly, Category = "Clip")
	int32 FrameCount = 0;

	// 每秒播放多少帧 (推荐 30)
	UPROPERTY(EditDefaultsOnly, Category = "Clip")
	float FramesPerSecond = 30.0f;

	// 是否循环 (Idle: true, Attack/Start: false)
	UPROPERTY(EditDefaultsOnly, Category = "Clip")
	bool bLoop = false;

	// 播完后自动切到哪个 Clip (NAME_None = 不自动切)
	UPROPERTY(EditDefaultsOnly, Category = "Clip")
	FName NextClipOnFinish = NAME_None;
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ARKNIGHT_API USpriteSheetAnimComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// ===== 编辑器配置 =====

	// 指向 M_Operator 母材质
	UPROPERTY(EditDefaultsOnly, Category = "Material")
	TObjectPtr<UMaterialInterface> BaseMaterial;

	// Sprite Sheet 纹理
	UPROPERTY(EditDefaultsOnly, Category = "Material")
	TObjectPtr<UTexture2D> SpriteSheetTexture;

	// 网格列数 — Python 脚本输出
	UPROPERTY(EditDefaultsOnly, Category = "Sheet")
	int32 SheetColumns = 8;

	// 网格行数 — Python 脚本输出
	UPROPERTY(EditDefaultsOnly, Category = "Sheet")
	int32 SheetRows = 8;

	// 所有动画 Clip 的字典 (Key 如 "Idle_Front", "Attack_Front")
	UPROPERTY(EditDefaultsOnly, Category = "Clips")
	TMap<FName, FSpritesheetAnimClip> Clips;

	// ===== 运行时状态 =====

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	FName CurrentClipName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	int32 CurrentFrameIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsAttacking = false;

	// ===== 接口 =====

	// 切换到指定 Clip
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayAnimation(FName ClipName);

	// 设置攻击状态 — 仅记录，实际 Clip 切换由外部配合 PlayAnimation 完成
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetAttacking(bool bInAttacking);

	// 材质 UV 水平反转: 0 = 正常, 1 = 反转
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetFlipU(bool bFlipped);

	UFUNCTION(BlueprintPure, Category = "Animation")
	bool IsPlaying() const { return bIsPlaying; }

	// ===== 生命周期 =====
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

public:
	USpriteSheetAnimComponent();

private:
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> SpritePlaneRef;

	// 当前在播的 Clip 数据缓存
	FSpritesheetAnimClip ActiveClip;
	bool bIsPlaying = false;
	float FrameAccumulator = 0.0f;
	int32 ClipPlayHead = 0;

	// 写 MID 的 FrameIndex 参数
	void SetMaterialFrame(int32 FrameIndex);

	// 非循环 Clip 播完回调
	void OnClipFinished();
};
