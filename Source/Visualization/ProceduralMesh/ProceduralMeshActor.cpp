// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralMeshActor.h"

// Sets default values
AProceduralMeshActor::AProceduralMeshActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AProceduralMeshActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProceduralMeshActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}




TArray<FVector> ploygon::MoveLost(TArray<FVector> point)
{
	TArray<FVector> _point;
	_point.Empty();
	for (int i = 0; i < point.Num() - 1; i++)
	{
		_point.Add(point[i]);
	}
	return _point;
}

TArray<ploygon::point3d> ploygon::guoDu(TArray<FVector> point)
{
	ploygon::point3d a;
	TArray<ploygon::point3d> mmm;
	mmm.Empty();
	for (int i = 0; i < point.Num(); i++)
	{
		a.index = i;
		a.p = point[i];
		mmm.Add(a);
	}
	return mmm;
}

TArray<ploygon::point2d> ploygon::aotuPoint(TArray<ploygon::point3d> point3d)//3d数组输入转为2d数组，并判断每个点在多边形中的凹凸性
{
	TArray <ploygon::point2d> DIN;
	DIN.Empty();
	ploygon::point2d P;
	FVector2D m;
	FVector  b, c, Q;    //Q的作用主要是确定多边形时针顺序
	bool aotu = true;
	float k;
	if (point3d.Num() < 3)
	{
		return DIN;
	}
	int id = 0; FVector a = point3d[0].p;
	for (int i = 0; i < point3d.Num() - 1; i++)//找出x最小点的index；
	{
		if (a.X >= point3d[i + 1].p.X)
		{
			a = point3d[i + 1].p;
			id = i + 1;
		}
	}
	if (id == 0)
	{
		a = point3d[point3d.Num() - 1].p - point3d[id].p;
		b = point3d[1].p - point3d[id].p;
		Q = a.operator^(b);     //Q的作用主要是确定多边形时针顺序
	}
	if (id == point3d.Num() - 1)
	{
		a = point3d[id - 1].p - point3d[id].p;
		b = point3d[0].p - point3d[id].p;
		Q = a.operator^(b);
	}
	if (id != 0 && id != point3d.Num() - 1)
	{
		a = point3d[id - 1].p - point3d[id].p;
		b = point3d[id + 1].p - point3d[id].p;
		Q = a.operator^(b);
	}
	if (Q.Z < 0)
	{
		bClockwise = true;  //确定多边形方向，顺时针为true。
	}
	else
	{
		bClockwise = false;
	}

	for (int i = 0; i < point3d.Num(); i++)
	{
		m.X = point3d[i].p.X;
		m.Y = point3d[i].p.Y;
		if (i == 0)
		{
			a = point3d[point3d.Num() - 1].p - point3d[0].p;
			b = point3d[i + 1].p - point3d[0].p;
			c = a.operator^(b);
			k = c.Z*Q.Z;
			if (k > 0) aotu = true;
			else aotu = false;
		}
		if (i == point3d.Num() - 1)
		{
			a = point3d[i - 1].p - point3d[i].p;
			b = point3d[0].p - point3d[i].p;
			c = a.operator^(b);
			k = c.Z*Q.Z;
			if (k > 0) aotu = true;
			else aotu = false;
		}
		if (i != 0 && i != point3d.Num() - 1)
		{
			a = point3d[i - 1].p - point3d[i].p;
			b = point3d[i + 1].p - point3d[i].p;
			c = a.operator^(b);
			k = c.Z*Q.Z;
			if (k > 0) aotu = true;
			else aotu = false;
		}
		P.p = m;
		P.index = point3d[i].index;
		P.aotu = aotu;
		DIN.Add(P);
	}
	return DIN;
}

bool  ploygon::isOut(FVector2D a, FVector2D b, FVector2D c, FVector2D d)//判断d点是否在三角形abc内部
{
	float du, du1, du2, du3;
	bool out = false;
	FVector2D A, B, C;
	A = a - d;
	B = b - d;
	C = c - d;
	if (A.Size() + C.Size() == (a - c).Size())
	{
		return out = false;

	}
	du1 = (A.operator|(B)) / ((A.Size())*(B.Size()));  //余弦值
	du1 = acos(du1);                                  //反三角求角度
	du2 = (B.operator|(C)) / ((B.Size())*(C.Size())); //余弦值
	du2 = acos(du2);                                   //反三角求角度
	du3 = (C.operator|(A)) / ((C.Size())*(A.Size()));  //余弦值
	du3 = acos(du3);
	du = du1 + du2 + du3 - 6.2831852;
	if (-0.01 < du&&du < 0.01)
	{
		return out = false;
	}
	else
	{
		return out = true;
	}
}

TArray<ploygon::triangle> ploygon::Triange(TArray<ploygon::point2d> point2d)             //三角形数组
{
	TArray<ploygon::triangle> triange;
	TArray<ploygon::point2d> b;
	triange.Empty();
	b.Empty();
	ploygon::triangle sanjiao;
	if (point2d.Num() < 3) return triange;

	for (int i = 0; i < point2d.Num(); i++)
	{
		if (point2d.Num() == 3)
		{
			sanjiao.index = 0;
			sanjiao.O = point2d[2].index;
			sanjiao.P = point2d[0].index;
			sanjiao.Q = point2d[1].index;
			sanjiao.R = point2d;
		}
		if (i == 0)
		{
			sanjiao.index = i;
			sanjiao.O = point2d[point2d.Num() - 1].index;
			sanjiao.P = point2d[i].index;
			sanjiao.Q = point2d[i + 1].index;
			b = point2d;
			b.RemoveAt(0);
			b.RemoveAt(0);
			b.RemoveAt(b.Num() - 1);
			sanjiao.R = b;
		}
		if (i == point2d.Num() - 1)
		{
			sanjiao.index = i;
			sanjiao.O = point2d[i - 1].index;
			sanjiao.P = point2d[i].index;
			sanjiao.Q = point2d[0].index;
			b = point2d;
			b.RemoveAt(i);
			b.RemoveAt(i - 1);
			b.RemoveAt(0);
			sanjiao.R = b;
		}
		if (i != 0 && i != point2d.Num() - 1)
		{
			sanjiao.index = i;
			sanjiao.O = point2d[i - 1].index;
			sanjiao.P = point2d[i].index;
			sanjiao.Q = point2d[i + 1].index;
			b = point2d;
			b.RemoveAt(i - 1);
			b.RemoveAt(i - 1);
			b.RemoveAt(i - 1);
			sanjiao.R = b;
		}
		triange.Add(sanjiao);
	}
	return triange;
}

TArray<FVector2D> ploygon::threeTo(TArray<FVector> point)
{
	TArray<FVector2D> ddian;
	ddian.Empty();
	FVector2D m;
	for (int i = 0; i < point.Num(); i++)
	{
		m.X = point[i].X;
		m.Y = point[i].Y;
		ddian.Add(m);
	}
	return ddian;
}

TArray<ploygon::point3d> ploygon::movePoint(TArray<ploygon::point3d> point3d)
{
	TArray<ploygon::point3d> diandian;
	diandian.Empty();
	FVector a, b, c, M, N;
	FVector HH;
	HH.X = 0; HH.Y = 0; HH.Z = 0;
	//float j=3.1415926,k=5.326987;
	for (int i = 0; i < point3d.Num(); i++)
	{
		if (i != 0 && i != point3d.Num() - 1)
		{
			a = point3d[i - 1].p;
			b = point3d[i].p;
			c = point3d[i + 1].p;
		}
		if (i == 0)
		{
			a = point3d[point3d.Num() - 1].p;
			b = point3d[i].p;
			c = point3d[i + 1].p;
		}
		if (i == point3d.Num() - 1)
		{
			a = point3d[i - 1].p;
			b = point3d[i].p;
			c = point3d[0].p;
		}
		M = b - a;
		N = c - a;
		if (M.operator^(N) == HH)
		{
			point3d.RemoveAt(i);
			i = 0;
		}
	}
	diandian = point3d;
	return diandian;
}

TArray<int> ploygon::chaiFen(TArray<FVector> point)
{
	//int num = 0;
	TArray<int> outuv;
	outuv.Empty();
	TArray<ploygon::point3d> ddianji = guoDu(point);
	TArray<FVector2D> m = threeTo(point);
	//return outuv;
	ddianji = movePoint(ddianji);
	for (int i = 0; i < ddianji.Num();)
	{
		if (ddianji.Num() < 3)
		{
			break;
		}
		//TArray<point3d> moved = movepoint(ddianji);
		//TArray<point>dian = aotudian(moved);
		TArray<ploygon::point2d>dian = aotuPoint(ddianji);
		TArray<ploygon::triangle>sanjiao = Triange(dian);
		TArray<ploygon::point2d> shenyu = sanjiao[i].R;
		if (ddianji.Num() == 3)
		{
			if (bClockwise)
			{
				outuv.Add(sanjiao[i].O);
				outuv.Add(sanjiao[i].P);
				outuv.Add(sanjiao[i].Q);
				break;
			}
			else
			{
				outuv.Add(sanjiao[i].Q);
				outuv.Add(sanjiao[i].P);
				outuv.Add(sanjiao[i].O);
				break;
			}
		}
		if (dian[i].aotu)
		{
			FVector2D a, b, c, d;
			int num = 0;
			a = m[sanjiao[i].O];
			b = m[sanjiao[i].P];
			c = m[sanjiao[i].Q];
			for (int j = 0; j < shenyu.Num(); j++)
			{
				d = shenyu[j].p;
				if (isOut(a, b, c, d))
				{
					num = num + 1;
				}
				if (!isOut(a, b, c, d))
				{
					i = i + 1;
					num = 0;
					break;
				}
			}
			if (num == shenyu.Num())
			{
				if (bClockwise)    //顺时针为true添加顺序OPQ
				{
					outuv.Add(sanjiao[i].O);
					outuv.Add(sanjiao[i].P);
					outuv.Add(sanjiao[i].Q);
					ddianji.RemoveAt(i);
					ddianji = movePoint(ddianji);
					i = 0;
					num = 0;
				}
				else          //逆时针为false反向添加
				{
					outuv.Add(sanjiao[i].Q);
					outuv.Add(sanjiao[i].P);
					outuv.Add(sanjiao[i].O);
					ddianji.RemoveAt(i);
					ddianji = movePoint(ddianji);
					i = 0;
					num = 0;
				}
			}
		}

		if (!dian[i].aotu)
		{
			i = i + 1;
		}
	}
	return outuv;
}

ploygon::ploygon(TArray<FVector> vertices)
{
	triangles.Empty();
	if (vertices.Num() < 4)
	{
		triangles = triangles;
	}
	else
	{
		TArray<FVector> dd = MoveLost(vertices);
		triangles = chaiFen(dd);//地板上面的面法向序列
		triangles = triangles;
	}
}

void AProceduralMeshActor::GetTriangles(TArray<FVector> vertices, TArray<int> &triangles)
{
	ploygon fuck = ploygon(vertices);
	triangles = fuck.triangles;
}

bool AProceduralMeshActor::SubDivide(const TArray<FVector> &inVertices, const TArray<int> &inTriangles, const int number, TArray<FVector> &vertices, TArray<int> &triangles)
{
	if (inVertices.Num() < 3)
	{
		return false;
	}
	int cishu = number;
	TArray<FVector> dian; TArray<int> mian;
	dian.Empty(); mian.Empty();
	dian = inVertices; mian = inTriangles;
	while (cishu)
	{
		int miannum = mian.Num() / 3;
		int diannum = dian.Num();
		TArray<int> guodu;
		guodu.Empty();
		guodu = mian;
		mian.Empty();
		for (int i = 0; i < miannum; i++)
		{
			FVector a = (dian[guodu[3 * i + 0]] + dian[guodu[3 * i + 1]]) / 2;
			FVector b = (dian[guodu[3 * i + 1]] + dian[guodu[3 * i + 2]]) / 2;
			FVector c = (dian[guodu[3 * i + 2]] + dian[guodu[3 * i + 0]]) / 2;
			dian.Add(a); dian.Add(b); dian.Add(c);
			mian.Add(guodu[3 * i + 0]); mian.Add(diannum + 3 * i + 0); mian.Add(diannum + 3 * i + 2);
			mian.Add(guodu[3 * i + 1]); mian.Add(diannum + 3 * i + 1); mian.Add(diannum + 3 * i + 0);
			mian.Add(guodu[3 * i + 2]); mian.Add(diannum + 3 * i + 2); mian.Add(diannum + 3 * i + 1);
			mian.Add(diannum + 3 * i + 0); mian.Add(diannum + 3 * i + 1); mian.Add(diannum + 3 * i + 2);
		}
		cishu = cishu - 1;
	}
	vertices = dian;
	triangles = mian;
	return true;
}
