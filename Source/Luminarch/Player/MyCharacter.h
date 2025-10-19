
#pragma once

#include "EngineMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "CharacterAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Sound/SoundCue.h"
#include "MyCharacter.generated.h"

UCLASS()
class LUMINARCH_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyCharacter();

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent>		mSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent>		mCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCharacterAnimInstance>	mAnimInst;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpotLightComponent>			mFlashLight;

	UPROPERTY(EditAnywhere, Category = "Zoom")
	float DefaultFOV = 90.f;

	UPROPERTY(EditAnywhere, Category = "Zoom")
	float ZoomFOV = 45.f;

	UPROPERTY(EditAnywhere, Category = "Zoom")
	float ZoomInterpSpeed = 3.f;

public:
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FlashLight")
	bool bCanUseFlashLight = false;

	
private:
	bool bIsZoom = false;
	bool bIsPause = false;

public:
	float AutoFocusTraceDistance = 5000.f;

	float FocusLocation = 0.f;

	float AutoFocusSpeed = 10.f;

	UFUNCTION(BlueprintCallable, Category = "FlashLight")
	void UnlockFlashLight();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


public:
	void MoveKey(const FInputActionValue& Value);
	void CameraRoatationKey(const FInputActionValue& Value);
	void RotationKey(const FInputActionValue& Value);
	void ActionZoomKey(const FInputActionValue& Value);
	void FlashLightKey(const FInputActionValue& Value);
	void PauseKey(const FInputActionValue& Value);
	void PickKey(const FInputActionValue& Value);
	void OpenKey(const FInputActionValue& Value);

	void StopMove(const FInputActionValue& Value);

private:
	void PlayFoootstep();

	FVector CurrentMoveDir;
	bool bIsMoving = false;

	float FootstepTimer = 0.0f;
	float FootstepInterval = 0.4f;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* FootstepSound;
};
