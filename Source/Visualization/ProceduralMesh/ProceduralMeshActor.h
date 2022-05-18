// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshActor.generated.h"



class ploygon
{
private:
	
	struct point3d    //3d点数组
	{
		FVector p;
		int index;
	};
	
	struct point2d    //2d点数组
	{
		FVector2D p;
		int index;
		bool aotu;
	};

	struct triangle     //2d三角形
	{
		int index;   //第几个三角形
		int  O;       //三个点的点的index
		int  P;
		int  Q;
		TArray<point2d> R;
	};
	
public:
	
	bool bClockwise = true;
	TArray<int> triangles;
	ploygon(TArray<FVector> vertices);
	
private:
	
	TArray<FVector> MoveLost(TArray<FVector> point);
	TArray<point3d> guoDu(TArray<FVector> point);
	TArray<point2d> aotuPoint(TArray<point3d> point3d);
	bool  isOut(FVector2D a, FVector2D b, FVector2D c, FVector2D d);
	TArray<triangle> Triange(TArray<point2d> point2d);
	TArray<FVector2D> threeTo(TArray<FVector> point);
	TArray<point3d> movePoint(TArray<point3d> point3d);
	TArray<int> chaiFen(TArray<FVector> point);
	
};



UCLASS()
class VISUALIZATION_API AProceduralMeshActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralMeshActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



	//三角剖分
	UFUNCTION(BlueprintPure, Category = "Visualization|ProceduralMesh", meta = (Keywords = ProceduralMesh))
		static void GetTriangles(TArray<FVector> vertices, TArray<int> &triangles);
	UFUNCTION(BlueprintPure, Category = "Visualization|ProceduralMesh", meta = (Keywords = ProceduralMesh))
		static bool SubDivide(const TArray<FVector> &inVertices, const TArray<int> &inTriangles, const int number, TArray<FVector> &vertices, TArray<int> &triangles);
	
};
