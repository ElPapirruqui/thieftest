// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ThiefCharacter.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AThiefCharacter::AThiefCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("LightDetectorCapture"));
	SceneCapture->SetupAttachment(RootComponent);
	SceneCapture->bCaptureEveryFrame = false;
	SceneCapture->bCaptureOnMovement = false;
	SceneCapture->bAlwaysPersistRenderingState = true;

	LightReceptor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightReceptor"));
	LightReceptor->SetupAttachment(RootComponent);
	//LightReceptor->SetStaticMesh(...); // load the cube mesh
	//LightReceptor->SetVisibility(false);
	//LightReceptor->SetHiddenInGame(true);

	LightReceptor->bOwnerNoSee = true;       // Main camera (player) won’t see it
	LightReceptor->bOnlyOwnerSee = false;    // Everyone else (including SceneCapture) will
}

// Called when the game starts or when spawned
void AThiefCharacter::BeginPlay()
{
	Super::BeginPlay();
	RenderTarget = SceneCapture->TextureTarget;
	if (!RenderTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("SceneCapture has no TextureTarget assigned!"));
		return;
	}
}

// Called every frame
void AThiefCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AThiefCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AThiefCharacter::RenderLightDetection() {
	// Capture scene manually (optional if already set to capture every frame)
	if (!RenderTarget) return;
	if (!SceneCapture) return;

	auto mesh = GetMesh();

	mesh->bOwnerNoSee = false;       // Main camera (player) won’t see it
	mesh->bOnlyOwnerSee = true;    // Everyone else (including SceneCapture) will

	SceneCapture->CaptureScene();

	// Read pixels
	TArray<FColor> PixelData;
	FRenderTarget* RenderResource = RenderTarget->GameThread_GetRenderTargetResource();
	RenderResource->ReadPixels(PixelData);

	// Center pixel logic
	int32 SizeX = RenderTarget->SizeX;
	int32 SizeY = RenderTarget->SizeY;
	int32 CenterIndex = (SizeY / 2) * SizeX + (SizeX / 2);

	if (PixelData.IsValidIndex(CenterIndex))
	{
		FColor CenterColor = PixelData[CenterIndex];
		float Brightness = (CenterColor.R + CenterColor.G + CenterColor.B) / (3.0f * 255.0f);

		UE_LOG(LogTemp, Log, TEXT("Light level: %.3f"), Brightness);
		CurrentLightLevel = Brightness;
	}
}

