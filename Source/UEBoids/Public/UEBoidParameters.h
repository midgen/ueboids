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
		float myAvoidWeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float myCohesionWeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float myCohesionRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float myAlignWeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float myAlignRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float myAvoidBoundsWeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float myVelocityMax;

	float myAvoidRange2, myCohesionRange2, myAlignRange2;


	FUEBoidParameters() :
		myNumBoids(100),
		myAvoidRange(10.f),
		myAvoidWeight(1.0f),
		myCohesionWeight(1.0f),
		myCohesionRange(1.0f),
		myAlignWeight(1.0f),
		myAlignRange(1.0f),
		myAvoidBoundsWeight(1.0f),
		myVelocityMax(10.f),
		myAvoidRange2(0.f),
		myCohesionRange2(0.f),
		myAlignRange2(0.f){}

	void Update()
	{
		myAvoidRange2 = myAvoidRange * myAvoidRange;
		myCohesionRange2 = myCohesionRange * myCohesionRange;
		myAlignRange2 = myAlignRange * myAlignRange;
	}
};