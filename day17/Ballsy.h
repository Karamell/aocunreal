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
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	UPROPERTY(EditAnywhere)
	class UMaterial* Material;
	UPROPERTY(EditAnywhere)
	class UStaticMesh* Mesh;
private:
	void Step();
	void Paint();
    void InitCubes();
    static TArray<FVector4> Neighbours(const FVector4 v);
	UInstancedStaticMeshComponent* IMesh;
	FTimerHandle GameTimerHandle;
	TSet<FVector4> Cubes;	//TSet/TArray<FIntVector4> fungerer ikke. BUG??? Contains /Set.Add er buggers
	int Cycles = 0;
	UPROPERTY()
	TArray<FTransform> Instances;
};
