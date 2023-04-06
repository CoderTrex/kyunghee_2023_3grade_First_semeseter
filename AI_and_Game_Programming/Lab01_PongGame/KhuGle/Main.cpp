//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//
#include "KhuGleWin.h"
#include <iostream>

#pragma warning(disable:4996)

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

class CCollision : public CKhuGleWin
{
public:
	CKhuGleLayer *m_pGameLayer;

	CKhuGleSprite *m_pCircle1;
	CKhuGleSprite *m_pCircle2;
	CKhuGleSprite *m_pLine;
	CKhuGleSprite* m_block;
	CKhuGleSprite *m_pNewCircle[100];

	CCollision(int nW, int nH);
	void Update();

	CKgPoint m_LButtonStart, m_LButtonEnd;
	int m_nLButtonStatus;
};

CCollision::CCollision(int nW, int nH) : CKhuGleWin(nW, nH) 
{
	m_nLButtonStatus = 0;
	m_Gravity = CKgVector2D(0., 98.);
	m_AirResistance = CKgVector2D(0.1, 0.1);

	m_pScene = new CKhuGleScene(640, 480, KG_COLOR_24_RGB(100, 100, 150));

	m_pGameLayer = new CKhuGleLayer(600, 420, KG_COLOR_24_RGB(150, 150, 200), CKgPoint(20, 30));

	m_pScene->AddChild(m_pGameLayer);

	m_pCircle1 = new CKhuGleSprite(GP_STYPE_ELLIPSE, GP_CTYPE_DYNAMIC, CKgLine(CKgPoint(30, 30), CKgPoint(90, 90)), 
		KG_COLOR_24_RGB(255, 0, 0), true, 100);
	m_pCircle2 = new CKhuGleSprite(GP_STYPE_ELLIPSE, GP_CTYPE_DYNAMIC, CKgLine(CKgPoint(70, 70), CKgPoint(130, 130)), 
		KG_COLOR_24_RGB(255, 0, 0), false, 100);
	m_pLine = new CKhuGleSprite(GP_STYPE_LINE, GP_CTYPE_STATIC, CKgLine(CKgPoint(300, 350), CKgPoint(350, 350)), 
		KG_COLOR_24_RGB(255, 0, 0), false, 10);
	m_block = new CKhuGleSprite(GP_STYPE_ELLIPSE, GP_CTYPE_DYNAMIC,  CKgLine(CKgPoint(200, 200), CKgPoint(250, 200)),
		KG_COLOR_24_RGB(255, 0, 0), true, 100);

	m_pGameLayer->AddChild(m_pCircle1);
	m_pGameLayer->AddChild(m_pCircle2);
	m_pGameLayer->AddChild(m_pLine);
	m_pGameLayer->AddChild(m_block);

	m_pCircle1->m_Velocity = CKgVector2D(900, 50);
	m_pCircle2->m_Velocity = CKgVector2D(-100, -300);
}

void CCollision::Update()
{
	if(m_bMousePressed[0]) {
		if(m_nLButtonStatus == 0){
			m_LButtonStart = CKgPoint(m_MousePosX, m_MousePosY);
		}
		m_LButtonEnd = CKgPoint(m_MousePosX, m_MousePosY);
		m_nLButtonStatus = 1;
	}
	else {
		if(m_nLButtonStatus == 1) {
			std::cout << m_LButtonStart.X << "," << m_LButtonStart.Y << std::endl;
			std::cout << m_LButtonEnd.X << "," << m_LButtonEnd.Y << std::endl;

			m_nLButtonStatus = 0;
		}
	}

	if(m_bKeyPressed['S']) 
	{
		m_pCircle1->m_Velocity = CKgVector2D(0, 0);
	}

	if(m_bKeyPressed[VK_LEFT]) m_block->m_Velocity  = CKgVector2D(-500, 0);
	if(m_bKeyPressed[VK_UP]) m_block->m_Velocity    = CKgVector2D(0, -500);
	if(m_bKeyPressed[VK_RIGHT]) m_block->m_Velocity = CKgVector2D(500, 0);
	if(m_bKeyPressed[VK_DOWN]) m_block->m_Velocity  = CKgVector2D(0, 500);

	for(auto &Layer : m_pScene->m_Children)
	{
		for(auto &Sprite : Layer->m_Children)
		{
			CKhuGleSprite *Ball = (CKhuGleSprite *)Sprite;
			Ball->m_bCollided = false;

			if(Ball->m_nType == GP_STYPE_RECT) continue;

			if(Ball->m_nType != GP_STYPE_ELLIPSE) continue;
			if(Ball->m_nCollisionType != GP_CTYPE_DYNAMIC) continue;

			Ball->m_Acceleration.x = m_Gravity.x - Ball->m_Velocity.x * m_AirResistance.x;			
			Ball->m_Acceleration.y = m_Gravity.y - Ball->m_Velocity.y * m_AirResistance.y;

			Ball->m_Velocity.x += Ball->m_Acceleration.x * m_ElapsedTime;
			Ball->m_Velocity.y += Ball->m_Acceleration.y * m_ElapsedTime;

			Ball->MoveBy(Ball->m_Velocity.x*m_ElapsedTime, Ball->m_Velocity.y*m_ElapsedTime);

			if(Ball->m_Center.x < 0) Ball->MoveTo(m_nW+Ball->m_Center.x, Ball->m_Center.y);
			if(Ball->m_Center.x > m_nW) Ball->MoveTo(Ball->m_Center.x-m_nW, Ball->m_Center.y);
			if(Ball->m_Center.y < 0) Ball->MoveTo(Ball->m_Center.x, m_nH+Ball->m_Center.y);
			if(Ball->m_Center.y > m_nH) Ball->MoveTo(Ball->m_Center.x, Ball->m_Center.y-m_nH);

			if(CKgVector2D::abs(Ball->m_Velocity) < 0.01)
				Ball->m_Velocity = CKgVector2D(0, 0);
		}

		std::vector<std::pair<CKhuGleSprite*, CKhuGleSprite*>> CollisionPairs;

		for(auto &SpriteA : Layer->m_Children)
		{
			CKhuGleSprite *Ball = (CKhuGleSprite *)SpriteA;
			for (auto& SpriteB : Layer->m_Children)
			{
				CKhuGleSprite* Target = (CKhuGleSprite*)SpriteB;

				if (Ball == Target) continue;

				if (((CKhuGleSprite*)Target)->m_nType == GP_STYPE_ELLIPSE)
				{
					CKgVector2D PosVec;
					double Overlapped;
					if (((CKhuGleSprite*)Target)->m_nType == GP_CTYPE_DYNAMIC)
					{
						PosVec = Ball->m_Center - Target->m_Center;
						Overlapped = CKgVector2D::abs(PosVec) - Ball->m_Radius - Target->m_Radius;
					}
					else
					{
						PosVec = Ball->m_Center - Target->m_Center;
						Overlapped = CKgVector2D::abs(PosVec) - Ball->m_Radius - Target->m_Radius;
					}
					if (Overlapped <= 0)
					{
						CollisionPairs.push_back({ Ball, Target });

						if (CKgVector2D::abs(PosVec) == 0)
						{
							if (Ball->m_nCollisionType != GP_CTYPE_STATIC)
								Ball->MoveBy(rand() % 3 - 1, rand() % 3 - 1);
							if (Target->m_nCollisionType != GP_CTYPE_STATIC)
								Target->MoveBy(rand() % 3 - 1, rand() % 3 - 1);
						}
						else
						{
							if (Ball->m_nCollisionType != GP_CTYPE_STATIC)
							{
								if (Target->m_nCollisionType == GP_CTYPE_STATIC)
									Ball->MoveBy(-PosVec.x * Overlapped / CKgVector2D::abs(PosVec), -PosVec.y * Overlapped / CKgVector2D::abs(PosVec) * 0.5);
								else
									Ball->MoveBy(-PosVec.x * Overlapped / CKgVector2D::abs(PosVec) * 0.5, -PosVec.y * Overlapped / CKgVector2D::abs(PosVec) * 0.5);
							}
							if (Target->m_nCollisionType != GP_CTYPE_STATIC)
							{
								if (Ball->m_nCollisionType == GP_CTYPE_STATIC)
									Target->MoveBy(PosVec.x * Overlapped / CKgVector2D::abs(PosVec), PosVec.y * Overlapped / CKgVector2D::abs(PosVec) * 0.5);
								else
									Target->MoveBy(PosVec.x * Overlapped / CKgVector2D::abs(PosVec) * 0.5, PosVec.y * Overlapped / CKgVector2D::abs(PosVec) * 0.5);
							}
							if (Target->m_nType == GP_STYPE_LINE)
							{
								CKgVector2D LinePos = CKgVector2D(Target->m_lnLine.End.X, Target->m_lnLine.End.Y)
									- CKgVector2D(Target->m_lnLine.Start.X, Target->m_lnLine.Start.Y);
								CKgVector2D LineCirclePos = Ball->m_Center - CKgVector2D(Target->m_lnLine.Start.X, Target->m_lnLine.Start.Y);


								double AA = LinePos.Dot(LinePos);
								double ProjectionRate = max(0., min(AA, LinePos.Dot(LineCirclePos)))/AA;

								CKgVector2D ProjectionPoint = CKgVector2D(Target->m_lnLine.Start) + ProjectionRate * LinePos;
								CKgVector2D Normal = Ball->m_Center - ProjectionPoint;

								double overlapped = CKgVector2D::abs(Normal) - Ball->m_Radius - Target->m_nWidth/2;
								if (overlapped <= 0)
								{
									std::vector<CKhuGleSprite*> VirtualBalls;
									CKhuGleSprite* VirtualBall = new CKhuGleSprite(GP_STYPE_ELLIPSE, GP_CTYPE_STATIC,
										CKgLine((int)(ProjectionPoint.x - Target->m_nWidth / 2.), (int)(ProjectionPoint.y - Target->m_nWidth / 2.),
										(int)(ProjectionPoint.x + Target->m_nWidth / 2.), (int)(ProjectionPoint.y + Target->m_nWidth / 2.)),
										KG_COLOR_24_RGB(255, 0, 0), false, 100);

									VirtualBall->m_Mass = 1E50;
									VirtualBalls.push_back(VirtualBall);

									CollisionPairs.push_back({ Ball, VirtualBall });
									if (CKgVector2D::abs(Normal) == 0)
									{
										if (Ball->m_nCollisionType != GP_CTYPE_STATIC)
											Ball->MoveBy(rand() % 3 - 1, rand() % 3 - 1);
									}
									else
									{
										if (Ball->m_nCollisionType != GP_CTYPE_STATIC)
											Ball->MoveBy(-Normal.x * overlapped / CKgVector2D::abs(Normal), -Normal.y * overlapped / CKgVector2D::abs(Normal));
									}
								}
							}
							Ball->m_bCollided = true;
							Target->m_bCollided = true;
						}
					}
				}
			}
		}

		for(auto &Pair : CollisionPairs)
		{
			CKhuGleSprite *BallA = Pair.first;
			CKhuGleSprite *BallB = Pair.second;

			CKgVector2D PosVec = BallB->m_Center - BallA->m_Center;
			double Distance = CKgVector2D::abs(PosVec);
			if(Distance == 0) Distance = 1E-6;
			CKgVector2D Normal = (1./Distance)*PosVec;

			double kx = (BallA->m_Velocity.x - BallB->m_Velocity.x);
			double ky = (BallA->m_Velocity.y - BallB->m_Velocity.y);
			double p = 2.0 * (Normal.x * kx + Normal.y * ky) / (BallA->m_Mass + BallB->m_Mass);

			BallA->m_Velocity.x = BallA->m_Velocity.x - p * BallB->m_Mass * Normal.x;
			BallA->m_Velocity.y = BallA->m_Velocity.y - p * BallB->m_Mass * Normal.y;

			BallB->m_Velocity.x = BallB->m_Velocity.x + p * BallA->m_Mass * Normal.x;
			BallB->m_Velocity.y = BallB->m_Velocity.y + p * BallA->m_Mass * Normal.y;
		}
	}

	m_pScene->Render();
	DrawSceneTextPos("Collision and Physics", CKgPoint(0, 0));

	CKhuGleWin::Update();
}

int main()
{

	CCollision *pCollision = new CCollision(640, 480);

	KhuGleWinInit(pCollision);

	return 0;
}