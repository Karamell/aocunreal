// Fill out your copyright notice in the Description page of Project Settings.


#include "Solver.h"

// Sets default values
ASolver::ASolver()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASolver::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASolver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

