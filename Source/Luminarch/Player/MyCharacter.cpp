#include "MyCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "CharacterAnimInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "../Input/CharacterInput.h"




AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	mSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Arm"));
	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	mFlashLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("FlashLight"));

	mSpringArm->SetupAttachment(GetMesh(), TEXT("HeadCam"));
	mCamera->SetupAttachment(mSpringArm);
	mFlashLight->SetupAttachment(mCamera);

	mSpringArm->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	mCamera->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	mFlashLight->SetRelativeLocation(FVector(20.f, 0.f, -5.f));
	mFlashLight->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

	mSpringArm->TargetArmLength = 0.f;
	mSpringArm->bDoCollisionTest = false;
	

	// 손전등 세팅
	mFlashLight->SetIntensityUnits(ELightUnits::Lumens);
	mFlashLight->SetIntensity(3.f);
	mFlashLight->SetAttenuationRadius(400.f);
	mFlashLight->SetInnerConeAngle(10.f);
	mFlashLight->SetOuterConeAngle(35.f);
	mFlashLight->SetLightColor(FLinearColor::White);
	mFlashLight->bUseTemperature = true;
	mFlashLight->Temperature = 6500.f;
	mFlashLight->CastShadows = true;

	// 시작시 플래쉬라이트 끈 상태
	mFlashLight->SetVisibility(false);

	mCamera->bUsePawnControlRotation = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// 이동/점프 등(필요시)
	GetCharacterMovement()->MaxWalkSpeed = 200.f;

	// 4) 메시/애님BP 로드는 일단 주석 처리 (원인 분리 목적)
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(
		TEXT("/Script/Engine.SkeletalMesh'/Game/Survival_Character/Meshes/SK_Survival_Character.SK_Survival_Character'")
	);
	if (MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMeshAsset(MeshAsset.Object);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -94.f));
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimAsset(
		TEXT("/Script/Engine.AnimBlueprint'/Game/Animation/ABP_Anim.ABP_Anim_C'")
	);
	if (AnimAsset.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimAsset.Class);
	}

	// 캡슐 충돌 설정은 유지해도 OK
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Player"));
	GetCapsuleComponent()->SetCapsuleHalfHeight(94.f);

	mCamera->PostProcessSettings.bOverride_DepthOfFieldFstop = true;
	mCamera->PostProcessSettings.bOverride_DepthOfFieldMinFstop = true;
	mCamera->PostProcessSettings.bOverride_DepthOfFieldSqueezeFactor = true;
	mCamera->PostProcessSettings.bOverride_DepthOfFieldFocalDistance = true;
	mCamera->PostProcessSettings.bOverride_DepthOfFieldDepthBlurAmount = true;
	mCamera->PostProcessSettings.bOverride_DepthOfFieldDepthBlurRadius = true;

	static ConstructorHelpers::FObjectFinder<USoundBase> FootstepAsset(TEXT("/Script/Engine.SoundWave'/Game/Essential_Foosteps_SK/WAV/Concrete/Footstep_Concrete_Boots_Jog_1.Footstep_Concrete_Boots_Jog_1'"));
	if(FootstepAsset.Succeeded())
	{
		FootstepSound = FootstepAsset.Object;
	}

	FootstepInterval = 0.4f;
}





void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	mAnimInst = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (IsValid(PlayerController))
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

		const UCharacterInput* InputCDO = GetDefault<UCharacterInput>();

		Subsystem->AddMappingContext(InputCDO->mContext, 0);
	}

	if (mFlashLight)
	{
		// 시작은 꺼두되 숨김은 풀어두기
		mFlashLight->SetHiddenInGame(false);
		mFlashLight->SetVisibility(false, true);
		mFlashLight->SetMobility(EComponentMobility::Movable); // 이동/회전 가능 조명 권장
	}
	
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 줌 할때 FOV 변경
	float FOV = bIsZoom ? ZoomFOV : DefaultFOV;
	float NewFOV = FMath::FInterpTo(mCamera->FieldOfView, FOV, DeltaTime, ZoomInterpSpeed);
	mCamera->SetFieldOfView(NewFOV);

	if (bIsZoom)
	{
		FVector Start = mCamera->GetComponentLocation();

		FVector End = (mCamera->GetForwardVector() * AutoFocusTraceDistance) + Start;

		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this);

		FHitResult HitResult;

		UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, ETraceTypeQuery::TraceTypeQuery1, true, ActorsToIgnore,
			EDrawDebugTrace::None, HitResult, true, FLinearColor::Red, FLinearColor::Green, 5.f);

		FVector HitLocation = HitResult.Location - mCamera->GetComponentLocation();
	
		float HitLocationLength = HitLocation.Size();

		FocusLocation = FMath::FInterpTo(FocusLocation, HitLocationLength, DeltaTime, AutoFocusSpeed);
	}
	else 
	{
		FocusLocation = 0.f;
	}


	mCamera->PostProcessSettings.DepthOfFieldFstop = 3.0f;
	mCamera->PostProcessSettings.DepthOfFieldMinFstop = 11.f;
	mCamera->PostProcessSettings.DepthOfFieldSqueezeFactor = 2.f;
	mCamera->PostProcessSettings.DepthOfFieldFocalDistance = FocusLocation;
	mCamera->PostProcessSettings.DepthOfFieldDepthBlurAmount = 10.f;
	mCamera->PostProcessSettings.DepthOfFieldDepthBlurRadius = 1.f;



	// 이동 중일 때만 발소리 타이머 진행
	if (bIsMoving)
	{
		FootstepTimer += DeltaTime;

		if (FootstepTimer >= FootstepInterval)
		{
			PlayFoootstep();
			FootstepTimer = 0.f;
		}
	}

	else
	{
		FootstepTimer = 0.f;
	}
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (IsValid(EnhancedInput))
	{
		const UCharacterInput* InputCDO = GetDefault<UCharacterInput>();

		EnhancedInput->BindAction(InputCDO->mMove, ETriggerEvent::Triggered, this, &AMyCharacter::MoveKey);

		EnhancedInput->BindAction(InputCDO->mCameraRotation, ETriggerEvent::Triggered, this, &AMyCharacter::CameraRoatationKey);

		EnhancedInput->BindAction(InputCDO->mRotation, ETriggerEvent::Triggered, this, &AMyCharacter::RotationKey);

		EnhancedInput->BindAction(InputCDO->mActionZoom, ETriggerEvent::Triggered, this, &AMyCharacter::ActionZoomKey);

		EnhancedInput->BindAction(InputCDO->mFlashLight, ETriggerEvent::Started, this, & AMyCharacter::FlashLightKey);

		EnhancedInput->BindAction(InputCDO->mPause, ETriggerEvent::Started, this, &AMyCharacter::PauseKey);

		EnhancedInput->BindAction(InputCDO->mPick, ETriggerEvent::Started, this, &AMyCharacter::PickKey);

		EnhancedInput->BindAction(InputCDO->mOpen, ETriggerEvent::Triggered, this, &AMyCharacter::OpenKey);

		EnhancedInput->BindAction(InputCDO->mMove, ETriggerEvent::Completed, this, &AMyCharacter::StopMove);
	}
}


void AMyCharacter::MoveKey(const FInputActionValue& Value)
{
	FVector Dir = Value.Get<FVector>();

	AddMovementInput(GetActorForwardVector(), Dir.X);
	AddMovementInput(GetActorRightVector(), Dir.Y);

	// 이동중인지 체크
	bIsMoving = !Dir.IsNearlyZero();
}

void AMyCharacter::StopMove(const FInputActionValue& Value)
{
	bIsMoving = false;
	FootstepTimer = 0.f; // 잔여 타이밍 제거
}

void AMyCharacter::CameraRoatationKey(const FInputActionValue& Value)
{
	FVector Axis = Value.Get<FVector>();

	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y*-1.f);
}

void AMyCharacter::RotationKey(const FInputActionValue& Value)
{
	float Angle = Value.Get<float>();

	AddControllerYawInput(Angle);
}



void AMyCharacter::ActionZoomKey(const FInputActionValue& Value)
{
	bIsZoom = Value.Get<bool>();
	UE_LOG(LogTemp, Warning,
		TEXT("Pressed: %d"), Value.Get<bool>());
}

void AMyCharacter::FlashLightKey(const FInputActionValue& Value)
{
	if (!bCanUseFlashLight)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Use FlashLight."));
		return;
	}

	if (mFlashLight)
	{
		// 숨김이 걸려있을 수 있으니 항상 해제 보증
		mFlashLight->SetHiddenInGame(false);

		// 토글 대신 상태를 명시적으로 전환하면 디버깅이 쉬워요
		const bool bNewOn = !mFlashLight->IsVisible();
		mFlashLight->SetVisibility(bNewOn, true);
		UE_LOG(LogTemp, Warning, TEXT("FlashLight %s"), bNewOn ? TEXT("ON") : TEXT("OFF"));

		// 소리가 왜 안들릴까?
		USoundBase* Sound = LoadObject<USoundBase>(nullptr,	TEXT("/Game/Free_Sounds_Pack/wav/Wood_Chop_1-4.Wood_Chop_1-4"));
	}
}

void AMyCharacter::UnlockFlashLight()
{
	if (bCanUseFlashLight)
	{
		UE_LOG(LogTemp, Warning, TEXT("FlashLight Already UnLocked"));
		return;
	}

	bCanUseFlashLight = true;

	if (mFlashLight)
	{
		// 언락 시 숨김 풀어두면 이후 F 토글이 바로 보임
		mFlashLight->SetHiddenInGame(false);
		mFlashLight->SetVisibility(false, true); // 언락은 '사용 가능'만 의미, 기본은 꺼짐
	}
}

void AMyCharacter::PauseKey(const FInputActionValue& Value)
{
	bIsPause = Value.Get<bool>();
	UE_LOG(LogTemp, Warning, TEXT("Pause"));
}

void AMyCharacter::PickKey(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Pick] V pressed"));
		UnlockFlashLight();
	}
}

void AMyCharacter::OpenKey(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Open] R pressed"));
	}
}

void AMyCharacter::PlayFoootstep()
{
	if (FootstepSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), FootstepSound, GetActorLocation());
	}
}

