#pragma once
#include "UEBoidTypes.h"
#include "UEBoidParameters.h"
#include "GameFramework/Volume.h"
#include "UEBoidManager.generated.h"

class UInstancedStaticMeshComponent;


UCLASS(hidecategories = (Tags, Cooking, Actor, HLOD, Mobile, LOD))
class UEBOIDS_API ABoidManager : public AVolume
{

	GENERATED_UCLASS_BODY()

public:

	ABoidManager();
	~ABoidManager();

	TArray<BoidData> myBoidData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UEBoids")
	UStaticMesh* myMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UEBoids")
	float myMeshScale = 0.01f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UEBoids")
	FUEBoidParameters myParameters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UEBoids")
	UInstancedStaticMeshComponent* myMeshComponent;

	void BeginPlay() override;

	void Tick(float DeltaSeconds) override;

protected:
	FBox myBounds;

	FVector myAABBMin, myAABBMax;
};
