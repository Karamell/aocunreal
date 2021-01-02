// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Ballsy.generated.h"

UCLASS()
class DAY17_API ABallsy : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ABallsy();
protected:
	virtual void BeginPlay() override;
	UFUNCTION() void OnGameTick();
	UFUNCTION() void OnGameShrunk();
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	UPROPERTY(EditAnywhere)
	class UMaterialInstance* Material;
	UPROPERTY(EditAnywhere)
	class UStaticMesh* Mesh;

private:
	void Step();
	void Paint();
    TArray<FVector4> InitCubes();
    static TArray<FVector4> Neighbours(const FVector4 v);
	UPROPERTY()
	UInstancedStaticMeshComponent* IMesh;
	FTimerHandle GameTimerHandle;
	int Cycle = 0;
	TArray<FVector4> Cubes;	//TSet/TArray<FIntVector4> fungerer ikke. BUG??? Contains /Set.Add er buggers
	TArray<int> Shrinks;
	UPROPERTY()
	TArray<FTransform> Instances;
};
