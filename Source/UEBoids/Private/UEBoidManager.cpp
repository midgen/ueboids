#include "UEBoidManager.h"
#include "Components/BrushComponent.h"
#include "UEBoids.h"
#include "Runtime\Core\Public\Math\Box.h"
#include "Runtime\Engine\Classes\Kismet\KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Components/InstancedStaticMeshComponent.h"


ABoidManager::ABoidManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
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
	Super::BeginPlay();

	myMeshComponent->SetStaticMesh(myMesh);
	myBoidData.Reserve(myParameters.myNumBoids);

	myBounds = GetComponentsBoundingBox(false);

	myAABBMin = myBounds.GetCenter() - myBounds.GetExtent();
	myAABBMax = myBounds.GetCenter() + myBounds.GetExtent();

	for (int i = 0; i < myParameters.myNumBoids; i++)
	{
		int index = myBoidData.Emplace();
		BoidData& data = myBoidData[index];
		
		data.myPosition = (FMath::RandPointInBox(GetComponentsBoundingBox(true)) / GetTransform().GetScale3D());
		data.myVelocity = FVector(FMath::VRand());

		FTransform tx;
		tx.SetLocation(data.myPosition);
		tx.SetScale3D(FVector(myMeshScale));
		myMeshComponent->AddInstance(tx);
	}
}

void ABoidManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	int index = 0;

	for (BoidData& data : myBoidData)
	{
		FVector meanPos(0.f);
		FVector meanVel(0.f);
		FVector avoidance(0.f);

		int numInCohesionRange = 0;
		int numInAlignRange = 0;

		for (int i = 0; i < myBoidData.Num(); i++)
		{
			if (i != index)
			{
				BoidData& innerBoid = myBoidData[i];
				float dist2 = FVector::DistSquared(data.myPosition, innerBoid.myPosition);

				if (dist2 < myParameters.myCohesionRange * myParameters.myCohesionRange)
				{
					meanPos += innerBoid.myPosition;
					numInCohesionRange++;
				}
				if (dist2 < (myParameters.myAvoidRange * myParameters.myAvoidRange))
				{
					avoidance +=  data.myPosition - innerBoid.myPosition;
				}
				if (dist2 < (myParameters.myAlignRange * myParameters.myAlignRange))
				{
					meanVel += innerBoid.myVelocity;
				}
				
			}
		}

		if(numInCohesionRange)
			meanPos /= numInCohesionRange;

		FVector coherence = (meanPos - data.myPosition) * myParameters.myCohesionWeight;

		if (numInAlignRange)
			meanVel /= numInAlignRange;

		FVector alignment = meanVel * myParameters.myAlignWeight;

		// Bounds
		FVector bounds(0.f);

		if (data.myPosition.X < myAABBMin.X) {
			bounds.X = myParameters.myAvoidBoundsWeight;
		}
		else if (data.myPosition.X > myAABBMax.X) {
			bounds.X = -myParameters.myAvoidBoundsWeight;
		}

		if (data.myPosition.Y < myAABBMin.Y) {
			bounds.Y = myParameters.myAvoidBoundsWeight;
		}
		else if (data.myPosition.Y > myAABBMax.Y) {
			bounds.Y = -myParameters.myAvoidBoundsWeight;
		}

		if (data.myPosition.Z < myAABBMin.Z) {
			bounds.Z = myParameters.myAvoidBoundsWeight;
		}
		else if (data.myPosition.Z > myAABBMax.Z) {
			bounds.Z = -myParameters.myAvoidBoundsWeight;
		}

		bounds *= myParameters.myAvoidBoundsWeight;


		data.myVelocity = FMath::Lerp(data.myVelocity, (data.myVelocity + (coherence + avoidance + alignment + bounds)).GetClampedToMaxSize(myParameters.myVelocityMax), DeltaSeconds * 10.f);
		data.myPosition += data.myVelocity * DeltaSeconds;
		
		FTransform tx; 
		myMeshComponent->GetInstanceTransform(index, tx);

		FQuat lookAtRotator = FRotationMatrix::MakeFromX(data.myVelocity).ToQuat();

		
		//tx.SetRotation((UKismetMathLibrary::FindLookAtRotation(data.myPosition, data.myPosition + data.myVelocity).Add(270.0f, 0.0f, 0.0f)).Quaternion());
		tx.SetRotation(lookAtRotator);
		tx.SetLocation(data.myPosition);

		myMeshComponent->UpdateInstanceTransform(index, tx, false, index < myBoidData.Num() - 1, true);

		index++;
	}


}


