// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Ballsy.generated.h"

UCLASS()
class DAY24_API ABallsy : public AActor
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
	UInstancedStaticMeshComponent* IMesh;
	FTimerHandle GameTimerHandle;
};
