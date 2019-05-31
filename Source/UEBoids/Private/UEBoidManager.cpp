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

	myParameters.Update();
}

void ABoidManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	int i = 0;

	for (BoidData& data : myBoidData)
	{
		FVector meanPos(0.f);
		FVector meanVel(0.f);
		FVector avoidance(0.f);

		int numInCohesionRange = 0;
		int numInAlignRange = 0;
		int j = 0;

		for (const BoidData& innerBoid : myBoidData)
		{
			if (i != j)
			{
				float dist2 = FVector::DistSquared(data.myPosition, innerBoid.myPosition);

				// Calculate our values for Cohesion, Avoidance, and Alignment

				// For Cohesion, we need the mean position of nearby boids
				if (dist2 < myParameters.myCohesionRange2)
				{
					meanPos += innerBoid.myPosition;
					numInCohesionRange++;
				}

				// For avoidance, we add a vector pointing away from any nearby boids in range
				if (dist2 < (myParameters.myAvoidRange2))
				{
					avoidance +=  data.myPosition - innerBoid.myPosition;
				}

				// For alignment, we need the mean velocity of nearby boids
				if (dist2 < (myParameters.myAlignRange2))
				{
					meanVel += innerBoid.myVelocity;
				}
				
			}
			++j;
		}


		// Coherence is a vector pointing from current pos towards the mean position of other boids within range
		if(numInCohesionRange)
			meanPos /= numInCohesionRange;

		FVector coherence = (meanPos - data.myPosition) * myParameters.myCohesionWeight;

		if (numInAlignRange)
			meanVel /= numInAlignRange;

		FVector alignment = meanVel * myParameters.myAlignWeight;

		// Calculate bounds vector, push us back inside the volume if we leave it
		FVector bounds(0.f);

		if (data.myPosition.X < myAABBMin.X) {
			bounds.X = myParameters.myAvoidBoundsWeight;
		} else if (data.myPosition.X > myAABBMax.X) {
			bounds.X = -myParameters.myAvoidBoundsWeight;
		}

		if (data.myPosition.Y < myAABBMin.Y) {
			bounds.Y = myParameters.myAvoidBoundsWeight;
		} else if (data.myPosition.Y > myAABBMax.Y) {
			bounds.Y = -myParameters.myAvoidBoundsWeight;
		}

		if (data.myPosition.Z < myAABBMin.Z) {
			bounds.Z = myParameters.myAvoidBoundsWeight;
		} else if (data.myPosition.Z > myAABBMax.Z) {
			bounds.Z = -myParameters.myAvoidBoundsWeight;
		}

		bounds *= myParameters.myAvoidBoundsWeight;


		// Add up the component vectors, and Lerp towards to the new velocity to keep things smooth.
		data.myVelocity = FMath::Lerp(data.myVelocity, (data.myVelocity + (coherence + avoidance + alignment + bounds)).GetClampedToMaxSize(myParameters.myVelocityMax), DeltaSeconds * 10.f);
		data.myPosition += data.myVelocity * DeltaSeconds;

		// Calculate instance transform. Probably a more efficient way to calculate rotation here?
		FTransform tx; 
		myMeshComponent->GetInstanceTransform(i, tx);
		FQuat lookAtRotator = FRotationMatrix::MakeFromX(data.myVelocity).ToQuat();
		tx.SetRotation(lookAtRotator);
		tx.SetLocation(data.myPosition);

		// Apply the new transform
		myMeshComponent->UpdateInstanceTransform(i, tx, false, i < myBoidData.Num() - 1, true);

		++i;
	}


}


