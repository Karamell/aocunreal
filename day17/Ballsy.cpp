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

const char* GInput_[] = {
	".#.",
	"..#",
	"###"
};

const float GGameRate = 4.0; // game ticks every GGameRate seconds + 1 sec to shrink and x seconds to calculate.
const int GCycles = 8; // number of Cycles to play.

ABallsy::ABallsy()
// Sets default values
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ACube(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	this->Material = CreateDefaultSubobject<UMaterialInstance>(TEXT("Material"));
	this->Mesh = CreateOptionalDefaultSubobject<UStaticMesh>(TEXT("Mesh"));
	this->IMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("IMesh"));
	this->IMesh->SetStaticMesh(ACube.Object); // default to a cube
	this->SetRootComponent(this->IMesh);
	this->Cubes = TArray<FVector4>();
	this->Instances = TArray<FTransform>();
	this->Shrinks = TArray<int>();
	this->Cubes = InitCubes();
	UE_LOG(LogTemp, Display, TEXT("Constructor called"))
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
	Paint();
}

// Called when the game starts or when spawned
void ABallsy::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(GameTimerHandle, this, &ABallsy::OnGameTick, GGameRate, false);
}

void ABallsy::Paint()
{
	Shrinks.Empty();
	Instances.Empty(Cubes.Num());
	for (auto c: this->Cubes) {
		float OffsetX = -400;
		float OffsetY = -350;
		auto Translation = FVector(OffsetX + 100 * c.X, OffsetY + 100 * c.Y, 100 * c.Z);
		auto Rotation = FQuat(FVector(1, 0, 0), FMath::DegreesToRadians(29 * c.W));
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

TArray<FVector4> ABallsy::InitCubes()
{
	const int Ylen = *(&GInput + 1) - GInput;
	const int Xlen = strlen(GInput[0]);
	auto Init = TArray<FVector4>();
	Init.Empty(Ylen * Xlen);
	for (int y = 0; y < Ylen; y++)
	{
		for(int x = 0; x < Xlen; x++)
		{
			if (GInput[y][x] == '#') {
				Init.Add(FVector4(x, y, 0, 0));
			}
		}
	}
	UE_LOG(LogTemp, Display, TEXT("Init Cubes count: %d"), Init.Num())
	return Init;
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
	for (const auto C : this->Cubes) {
		Consider.Append(this->Neighbours(C));
	}
	UE_LOG(LogTemp, Display, TEXT("Consider %d neighbours"), Consider.Num())
	const auto CubeSet = TSet<FVector4>(this->Cubes);
	auto NewCubes = TArray<FVector4>();
	Shrinks.Empty();
	for (const auto C : Consider)
	{
		int activeNeighbours = 0;
		for (const auto N: this->Neighbours(C)) {
			if (CubeSet.Contains(N)) {activeNeighbours++;}
		}
		const auto Found = this->Cubes.Find(C);
		if (Found == INDEX_NONE)
		{
			if (activeNeighbours == 3) {NewCubes.Add(C);}
		}
		else
		{
			if (activeNeighbours == 2 || activeNeighbours == 3)
			{
				NewCubes.Add(C);
			}
			else
			{
				Shrinks.Add(Found);
			}
		}
	}
	UE_LOG(LogTemp, Display, TEXT("Making %d new cubes."), NewCubes.Num())
	this->Cubes = NewCubes;
}
void ABallsy::OnGameShrunk()
{
	this->Shrinks.Empty();
	Paint();
	if (Cycle < GCycles)
	{
		GetWorld()->GetTimerManager().SetTimer(GameTimerHandle, this, &ABallsy::OnGameTick, GGameRate, false);
	}
}

void ABallsy::OnGameTick()
{
	Cycle++;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("TICK: Cycle %i, Cubes:%i"), Cycle, Cubes.Num()));
	Step();
	UE_LOG(LogTemp, Display, TEXT("Cycle %d contains %d cubes."), Cycle, Cubes.Num())
	GetWorld()->GetTimerManager().SetTimer(GameTimerHandle, this, &ABallsy::OnGameShrunk, 1.0, false);
}

// Called every frame
void ABallsy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (0 < this->Shrinks.Num() && this->Shrinks.Num() <= Instances.Num()) {
		auto I = TArray<FTransform>(Instances);
		for (auto S : this->Shrinks)
		{
			const auto Scale = Instances[S].GetScale3D().X;
			auto NuScale = FVector(FMath::Max(Scale - DeltaTime, 0.0f));
			Instances[S].SetScale3D(NuScale);
			IMesh->UpdateInstanceTransform(S, Instances[S]);
		}
		IMesh->MarkRenderStateDirty();
	}
}
