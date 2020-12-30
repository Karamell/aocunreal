// Fill out your copyright notice in the Description page of Project Settings.


#include "Ballsy.h"

const char* GInput[] = {
	"##.#####",
    "#.##..#.",
    ".##...##",
    "###.#...",
    ".#######",
    "##....##",
    "###.###.",
    ".#.#.#.."
};


ABallsy::ABallsy()
// Sets default values
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
//	PrimaryActorTick.bTickEvenWhenPaused = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> aCube(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	this->Material = CreateDefaultSubobject<UMaterial>(TEXT("Material"));
	this->Mesh = CreateOptionalDefaultSubobject<UStaticMesh>(TEXT("Mesh"));
	this->IMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("IMesh"));
	this->IMesh->SetStaticMesh(aCube.Object); // default to a cube
	this->SetRootComponent(this->IMesh);
	UE_LOG(LogTemp, Display, TEXT("constructed"))
}

void ABallsy::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (this->Material != nullptr) {
		UE_LOG(LogTemp, Display, TEXT("set the material"))
		this->IMesh->SetMaterial(0, this->Material);
	}
	if (this->Mesh != nullptr && this->Mesh->HasValidRenderData()) {
		UE_LOG(LogTemp, Display, TEXT("set a mesh"))
		this->IMesh->SetStaticMesh(this->Mesh);
	}
	this->IMesh->SetFlags(RF_Transactional);
	const auto T0 = FTransform();
	IMesh->AddInstance(T0); // mesh at 0,0,0
}

// Called when the game starts or when spawned
void ABallsy::BeginPlay()
{
	Super::BeginPlay();
	if (this->Material != nullptr) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Set dat material"));
		this->IMesh->SetMaterial(0, this->Material);
	}
    TArray<FIntVector4> Cubes;
	for (int y = 0; y < 7; y++)
	{
		for(int x = 0; x < 8; x++)
		{
			if (GInput[y][x] == '#') {
				Cubes.Push(FIntVector4(x * 100, y * 100, 0, 0));
			}
		}
	}

	TArray<FTransform> instances;
	for (auto c: Cubes) {
		auto Translation = FVector(c.X, c.Y, c.Z);

		auto Rotation = FQuat(1, 0, 0, FMath::DegreesToRadians(c.W)).GetNormalized();
		auto T = FTransform(Rotation, Translation, FVector(0.7));
		instances.Push(T);
	}

	IMesh->AddInstances(instances, false);
	int const Count = IMesh->GetInstanceCount();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Instance count: %d"), Count));
    IMesh->MarkRenderStateDirty();

	GetWorld()->GetTimerManager().SetTimer(GameTimerHandle, this, &ABallsy::OnGameTick, 10.0, false);

}
void ABallsy::OnGameTick()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("TICK"));
	//GetWorld()->GetTimerManager().SetTimerForNextTick()
}

// Called every frame
void ABallsy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
