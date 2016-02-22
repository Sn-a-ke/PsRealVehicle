// Copyright 2016 Vladimir Alyamkin. All Rights Reserved.

#include "VrvPlugin.h"

UVrvVehicleMovementComponent::UVrvVehicleMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoActivate = true;
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;

	SprocketMass = 65.f;
	SprocketRadius = 25.f;
	TrackMass = 600.f;

	bAutoGear = true;
}

//////////////////////////////////////////////////////////////////////////
// Initialization

void UVrvVehicleMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	CalculateMOI();
	InitSuspension();
	InitGears();
}

void UVrvVehicleMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


//////////////////////////////////////////////////////////////////////////
// Physics Initialization

void UVrvVehicleMovementComponent::CalculateMOI()
{
	float SprocketSquareRadius = (SprocketRadius * SprocketRadius);
	float SprocketMOI = (SprocketMass / 2) * SprocketSquareRadius;
	float TrackMOI = TrackMass * SprocketSquareRadius;

	FinalMOI = SprocketMOI + TrackMOI;

	UE_LOG(LogVrvVehicle, Warning, TEXT("Final MOI: %f"), FinalMOI);
}

void UVrvVehicleMovementComponent::InitSuspension()
{
	for (auto SuspInfo : SuspensionSetup)
	{
		if (SuspInfo.bInheritWheelBoneTransform)
		{
			USkinnedMeshComponent* Mesh = GetMesh();
			if (Mesh)
			{
				FTransform WheelTransform = Mesh->GetSocketTransform(SuspInfo.BoneName, RTS_Actor);
				SuspInfo.Location = WheelTransform.GetLocation();
				SuspInfo.Rotation = WheelTransform.GetRotation().Rotator();
			}
		}

		FSuspensionState SuspState;
		SuspState.SuspensionInfo = SuspInfo;
		SuspState.PreviousLength = SuspInfo.MaximumLength;

		SuspensionData.Add(SuspState);
	}
}

void UVrvVehicleMovementComponent::InitGears()
{
	for (int32 i = 0; i < GearSetup.Num(); i++)
	{
		if (GearSetup[i].Ratio == 0.f)
		{
			NeutralGear = (bAutoGear) ? i : FMath::Max(i + 1, GearSetup.Num());
		}
	}

	UE_LOG(LogVrvVehicle, Warning, TEXT("Neutral gear: %d"), NeutralGear);
}


//////////////////////////////////////////////////////////////////////////
// Vehicle control

void UVrvVehicleMovementComponent::SetThrottleInput(float Throttle)
{
	RawThrottleInput = FMath::Clamp(Throttle, -1.0f, 1.0f);
}

void UVrvVehicleMovementComponent::SetSteeringInput(float Steering)
{
	RawSteeringInput = FMath::Clamp(Steering, -1.0f, 1.0f);
}

void UVrvVehicleMovementComponent::SetHandbrakeInput(bool bNewHandbrake)
{
	bRawHandbrakeInput = bNewHandbrake;
}


//////////////////////////////////////////////////////////////////////////
// Vehicle stats

float UVrvVehicleMovementComponent::GetForwardSpeed() const
{
	return 0.0f;
}

float UVrvVehicleMovementComponent::GetEngineRotationSpeed() const
{
	return 0.0f;
}

float UVrvVehicleMovementComponent::GetEngineMaxRotationSpeed() const
{
	return 0.0f;
}


//////////////////////////////////////////////////////////////////////////
// Data access

USkinnedMeshComponent* UVrvVehicleMovementComponent::GetMesh()
{
	return Cast<USkinnedMeshComponent>(UpdatedComponent);
}


//////////////////////////////////////////////////////////////////////////
// Debug

void UVrvVehicleMovementComponent::DrawDebug(UCanvas* Canvas, float& YL, float& YPos)
{
	// @todo
}
