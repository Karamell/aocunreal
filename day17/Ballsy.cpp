// Fill out your copyright notice in the Description page of Project Settings.


#include "Ballsy.h"

const char* GInput1[] = {
	"##.#####",
    "#.##..#.",
    ".##...##",
    "###.#...",
    ".#######",
    "##....##",
    "###.###.",
    ".#.#.#.."
};

const char* GInput[] = {
	".#.",
	"..#",
	"###"
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
	this->Cubes = TSet<FVector4>();
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
	InitCubes();
	Paint();
}

// Called when the game starts or when spawned
void ABallsy::BeginPlay()
{
	Super::BeginPlay();
	InitCubes();
	GetWorld()->GetTimerManager().SetTimer(GameTimerHandle, this, &ABallsy::OnGameTick, 5.0, false);
}

void ABallsy::Paint()
{
	Instances.Empty(Cubes.Num());
	for (auto c: this->Cubes) {
		auto Translation = FVector(100 * c.X, 100 * c.Y, 100 * c.Z);
		auto Rotation = FQuat(1, 0, 0, FMath::DegreesToRadians(29 * c.W)).GetNormalized();
		auto Scale = FVector(0.7);
		auto T = FTransform(Rotation, Translation, Scale);
		Instances.Add(T);
	}
	IMesh->ClearInstances();
	IMesh->AddInstances(this->Instances, false);
	int const Count = IMesh->GetInstanceCount();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Instance count: %d"), Count));
	IMesh->MarkRenderStateDirty();
}

void ABallsy::InitCubes()
{
	int ylen = *(&GInput + 1) - GInput;
	int xlen = strlen(GInput[0]);
	auto Init = TArray<FVector4>();
	Init.Empty(ylen * xlen);
	for (int y = 0; y < ylen; y++)
	{
		for(int x = 0; x < xlen; x++)
		{
			if (GInput[y][x] == '#') {
				Init.Add(FVector4(x, y, 0, 0));
			}
		}
	}
	UE_LOG(LogTemp, Display, TEXT("Init Cubes count: %d"), Init.Num())
	auto Cb = TSet<FVector4>();
	Cb.Append(Init);
	this->Cubes = Cb;
}

TArray<FVector4> ABallsy::Neighbours(const FVector4 v)
{
	auto Ta = TArray<FVector4>();
	Ta.Empty(80);
	for (int x = -1; x <= 1 ; x++) {
		for (int y = -1; y <= 1; y++) {
			for (int z = -1; z <= 1; z++) {
				for (int w = -1; w <= 1; w++) {
					if (!(x == 0 && y == 0 && z == 0 && w == 0))
					{
						auto Vec = FVector4(v.X + x, v.Y + y, v.Z + z, v.W + w);
						Ta.Add(Vec);
					}
				}
			}
		}
	} // puh
	return Ta;
}

void ABallsy::Step()
{
	auto Consider = TSet<FVector4>();
	for (auto C : this->Cubes) {
		Consider.Append(this->Neighbours(C));
	}
	UE_LOG(LogTemp, Display, TEXT("Consider %d neighbours"), Consider.Num())
	auto NewCubes = TSet<FVector4>();
	for (auto C : Consider)
	{
		int activeNeighbours = 0;
		for (auto N: this->Neighbours(C)) {
			if (this->Cubes.Contains(N)) {activeNeighbours++;}
		}

		if (this->Cubes.Contains(C))
		{
			if (activeNeighbours == 2 || activeNeighbours == 3) {NewCubes.Add(C);}
		}
		else
		{
			if (activeNeighbours == 3) {NewCubes.Add(C);}
		}
	}
	UE_LOG(LogTemp, Display, TEXT("Making %d new cubes."), NewCubes.Num())
	this->Cubes = NewCubes;
}

void ABallsy::OnGameTick()
{
	Cycles++;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("TICK: Cycle %i, Cubes:%i"), Cycles, Cubes.Num()));
	Step();
	Paint();
	UE_LOG(LogTemp, Display, TEXT("Cycle %d contains %d cubes."), Cycles, Cubes.Num())
	if (Cycles < 6) 
	{
		GetWorld()->GetTimerManager().SetTimer(GameTimerHandle, this, &ABallsy::OnGameTick, 5.0, false);
	}
}

// Called every frame
void ABallsy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
