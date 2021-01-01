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
	this->Cubes = TArray<FIntVector4>();
	Instances = TArray<FTransform>();

	UE_LOG(LogTemp, Display, TEXT("constructed"))
}

void ABallsy::OnConstruction(const FTransform& Transform)
{
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


	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Init Cubes count: %d"), Cubes.Num()));
	UE_LOG(LogTemp, Display, TEXT("Init Cubes count: %d"), this->Cubes.Num())
//	Super::OnConstruction(Transform);
}

// Called when the game starts or when spawned
void ABallsy::BeginPlay()
{
	Super::BeginPlay();
	// init state
	for (int y = 0; y < 7; y++)
	{
		for(int x = 0; x < 8; x++)
		{
			if (GInput[y][x] == '#') {
				this->Cubes.Add(FIntVector4(x, y, 0, 0));
			}
		}
	}
	Paint();
	GetWorld()->GetTimerManager().SetTimer(GameTimerHandle, this, &ABallsy::OnGameTick, 5.0, false);
}

void ABallsy::Paint()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("CUBE count: %d"), this->Cubes.Num()));
	Instances.Empty(Cubes.Num());
	for (auto c: this->Cubes) {
		auto Translation = FVector(100 * c.X, 100 * c.Y, 100 * c.Z);
		auto Rotation = FQuat(1, 0, 0, FMath::DegreesToRadians(c.W)).GetNormalized();
		auto T = FTransform(Rotation, Translation, FVector(0.7));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Adding %s"), *Translation.ToString()));
		Instances.Add(T);
	}
	IMesh->ClearInstances();
	IMesh->AddInstances(this->Instances, false);
	int const Count = IMesh->GetInstanceCount();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Instance count: %d"), Count));
	IMesh->MarkRenderStateDirty();
}

TSet<FIntVector4> ABallsy::Neighbours(const FIntVector4 v)
{
	auto Ta = TSet<FIntVector4>();
	for (int x = 0; x < 3; x++) {
		for (int y = 0; y < 3; y++) {
			for (int z = 0; z < 3; z++) {
				for (int w = 0; w < 3; w++) {
					if (x + y + z + w != 0)
					{
						Ta.Add(FIntVector4(v.X + x - 1, v.Y + y - 1, v.Z + z - 1, v.W + w - 1));
					}
				}
			}
		}
	} // puh
	return Ta;
}

void ABallsy::Step()
{
	auto Consider = TSet<FIntVector4>();
	auto NewCubes = TSet<FIntVector4>();
	for (auto C : this->Cubes) {
		Consider.Append(this->Neighbours(C));
	}
	for (auto c : Consider)
	{
		int activeNeighbours = 0;
		for (auto N: this->Neighbours(c)) {
			if (this->Cubes.Contains(N)) {activeNeighbours++;}
		}
		if (this->Cubes.Contains(c))
		{
			if (activeNeighbours == 2 || activeNeighbours == 3) NewCubes.Add(c);
		}
		else
		{
			if (activeNeighbours == 3) NewCubes.Add(c);
		}
	}
	this->Cubes = NewCubes.Array();
}

void ABallsy::OnGameTick()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("TICK: Cycle %i"), Cycles));
	Step();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("TICK: Cycle %i, Cubes:%i"), Cycles, Cubes.Num()));
	Paint();
	Cycles++;
	GetWorld()->GetTimerManager().SetTimer(GameTimerHandle, this, &ABallsy::OnGameTick, 5.0, false);
}

// Called every frame
void ABallsy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
