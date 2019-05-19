#include "UEBoidManager.h"
#include "Components/BrushComponent.h"
#include "UEBoids.h"
#include "Runtime/Engine/Classes/Components/InstancedStaticMeshComponent.h"


ABoidManager::ABoidManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.SetTickFunctionEnable(true);
	PrimaryActorTick.bStartWithTickEnabled = true;

	SetActorTickEnabled(true);


	myMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("myMeshComponent"));
	myMeshComponent->SetupAttachment(RootComponent);
	GetBrushComponent()->Mobility = EComponentMobility::Static;

	BrushColor = FColor(255, 255, 255, 255);

	bColored = true;
}


ABoidManager::~ABoidManager()
{
	myBoidData.Empty();
}

void ABoidManager::BeginPlay()
{
	myMeshComponent->SetStaticMesh(myMesh);
	myBoidData.Reserve(myParameters.myNumBoids);

	myBounds = GetComponentsBoundingBox(false);

	for (int i = 0; i < myParameters.myNumBoids; i++)
	{
		int index = myBoidData.Emplace();
		UEBoidData& data = myBoidData[index];
		
		data.myPosition = (FMath::RandPointInBox(GetComponentsBoundingBox(true)) / GetTransform().GetScale3D()) - (GetTransform().GetLocation() * 0.005f);

		FTransform tx;
		tx.SetLocation(data.myPosition);
		tx.SetScale3D(FVector(myMeshScale));
		myMeshComponent->AddInstance(tx);
	}
}

void ABoidManager::Tick(float DeltaSeconds)
{
	FVector meanPos(0.f);
	FVector meanVel(0.f);

	for (UEBoidData& data : myBoidData)
	{
		meanPos += data.myPosition;
		meanVel += data.myVelocity;
	}

	meanPos /= myBoidData.Num();
	meanVel /= myBoidData.Num();

	int index = 0;

	for (UEBoidData& data : myBoidData)
	{
		FVector coherence = meanPos - data.myPosition;
		FVector avoidance;

		for (int i = 0; i < myBoidData.Num(); i++)
		{
			if (i != index)
			{
				UEBoidData& innerBoid = myBoidData[i];
				float dist2 = FVector::DistSquared(data.myPosition, innerBoid.myPosition);
				if (dist2 < (myParameters.myAvoidRange * myParameters.myAvoidRange))
				{
					avoidance = innerBoid.myPosition - data.myPosition;
				}
			}
		}

		FVector alignment = meanVel - data.myVelocity;

		data.myVelocity += coherence + avoidance + alignment;

		

		data.myPosition += data.myVelocity;

		FTransform tx;
		tx.SetLocation(data.myPosition);

		myMeshComponent->UpdateInstanceTransform(index, tx, false, index < myBoidData.Num() - 1, true);

		index++;
	}


}


