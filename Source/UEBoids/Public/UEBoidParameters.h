#pragma once
#include "CoreMinimal.h"
#include "UEBoidParameters.generated.h"


USTRUCT(BlueprintType)
struct UEBOIDS_API FUEBoidParameters
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int myNumBoids;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float myAvoidRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float myAvoidBoundsWeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float myAvoidWeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float myCohesionWeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float myAlignWeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float myVelocityMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float myInteractionRange;

	FUEBoidParameters() :
		myNumBoids(100),
		myAvoidRange(10.f),
		myAvoidBoundsWeight(1.0f),
		myAvoidWeight(1.0f),
		myCohesionWeight(1.0f),
		myAlignWeight(1.0f),
		myVelocityMax(10.f),
		myInteractionRange(50.f) {}
};