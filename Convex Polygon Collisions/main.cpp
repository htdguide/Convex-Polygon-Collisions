#include "main.h"
#include "raylib.h"
#include <vector>


class PolygonicCollisions 
{

	struct polygon
	{
		std::vector<Vector2> p;						//Transformed Points
		Vector2 pos;								//Position of shape
		float angle;								//Direction of shape
		std::vector<Vector2> o;						//"Model" of shape
		bool overlap = false;						//Flag to indicate if overlap has occured
	};

public:
	std::vector<polygon> vecShapes;
	int nMode = 0;

	bool OnUserCreate() 
	{

		//Create Pentagon
		polygon s1;
		float fTheta = 3.14159f * 2.0f / 5.0f;
		s1.pos = { 100, 100 };
		s1.angle = 0.0f;

		for (int i = 0; i < 5; i++)
		{
			s1.o.push_back({ 30.0f * cosf(fTheta * i), 30.0f * sinf(fTheta * i) });
			s1.p.push_back({ 30.0f * cosf(fTheta * i), 30.0f * sinf(fTheta * i) });
		}

		//Create Triangle
		polygon s2;
		fTheta = 3.14159f * 2.0f / 3.0f;
		s2.pos = { 200, 150 };
		s2.angle = 0.0f;
		for (int i = 0; i < 3; i++) 
		{
			s2.o.push_back({ 20.0f * cosf(fTheta * i), 20.0f * sinf(fTheta * i) });
			s2.p.push_back({ 20.0f * cosf(fTheta * i), 20.0f * sinf(fTheta * i) });
		}

		//Create Quad
		polygon s3;
		s3.pos = { 50, 200 };
		s3.angle = 0.0f;
		s3.o.push_back({ -30, -30 });
		s3.o.push_back({ -30, +30 });
		s3.o.push_back({ +30, +30 });
		s3.o.push_back({ +30, -30 });
		s3.p.resize(4);

		vecShapes.push_back(s1);
		vecShapes.push_back(s2);
		vecShapes.push_back(s3);
		
		return true;
	}

	bool ShapeOverlap_SAT(polygon& r1, polygon& r2) 
	{
		polygon *poly1 = &r1;
		polygon *poly2 = &r2;

		for (int shape = 0; shape < 2; shape++)
		{
			if (shape == 1)
			{
				poly1 = &r2;
				poly2 = &r1;
			}

			for (int a = 0; a < poly1->p.size(); a++)
			{
				int b = (a + 1) % poly1->p.size();
				Vector2 axisProj = { -(poly1->p[b].y - poly1->p[a].y), poly1->p[b].x - poly1->p[a].x };

				//Work out min and max 1D points for r1
				float min_r1 = INFINITY, max_r1 = -INFINITY;
				for (int p = 0; p < poly1->p.size(); p++)
				{
					float q = (poly1->p[p].x * axisProj.x + poly1->p[p].y * axisProj.y);
					min_r1 = std::min(min_r1, q);
					max_r1 = std::max(max_r1, q);
				}

				//Work out min and max 1D points for r2
				float min_r2 = INFINITY, max_r2 = -INFINITY;
				for (int p = 0; p < poly2->p.size(); p++)
				{
					float q = (poly2->p[p].x * axisProj.x + poly2->p[p].y * axisProj.y);
					min_r2 = std::min(min_r2, q);
					max_r2 = std::max(max_r2, q);
				}

				if (!(max_r2 >= min_r1 && max_r1 >= min_r2))
					return false;
			}
		}
		return true;
	}

	bool onUserUpdate() 
	{
		//Shape 1
		if (IsKeyDown(KEY_LEFT)) vecShapes[0].angle -= 0.2f;
		if (IsKeyDown(KEY_RIGHT)) vecShapes[0].angle += 0.2f;

		if (IsKeyDown(KEY_UP)) 
		{
			vecShapes[0].pos.x += cosf(vecShapes[0].angle) * 6.0f;
			vecShapes[0].pos.y += sinf(vecShapes[0].angle) * 6.0f;
		}

		if (IsKeyDown(KEY_DOWN)) 
		{
			vecShapes[0].pos.x -= cosf(vecShapes[0].angle) * 6.0f;
			vecShapes[0].pos.y -= sinf(vecShapes[0].angle) * 6.0f;
		}

		//Shape 2
		if (IsKeyDown(KEY_A)) vecShapes[1].angle -= 0.2f;
		if (IsKeyDown(KEY_D)) vecShapes[1].angle += 0.2f;

		if (IsKeyDown(KEY_W)) 
		{
			vecShapes[1].pos.x += cosf(vecShapes[1].angle) * 6.0f;
			vecShapes[1].pos.y += sinf(vecShapes[1].angle) * 6.0f;
		}

		if (IsKeyDown(KEY_S)) 
		{
			vecShapes[1].pos.x -= cosf(vecShapes[1].angle) * 6.0f;
			vecShapes[1].pos.y -= sinf(vecShapes[1].angle) * 6.0f;
		}

		//Update shapes and reset flags
		for (auto& r : vecShapes)
		{
			for (int i = 0; i < r.o.size(); i++)
				r.p[i] =
			{
				//2D Rotation Transform + 2D Translation
				(r.o[i].x * cosf(r.angle)) - (r.o[i].y * sinf(r.angle)) + r.pos.x,
				(r.o[i].x * sinf(r.angle)) + (r.o[i].y * cosf(r.angle)) + r.pos.y,
			};

			r.overlap = false;
		}

		//Check for overlap
		for (int m = 0; m < vecShapes.size(); m++)
			for (int n = m + 1; n < vecShapes.size(); n++)
			{
				vecShapes[m].overlap |= ShapeOverlap_SAT(vecShapes[m], vecShapes[n]);
			}
		return true;
	}

};

int main() 
{
	InitWindow(800, 600, "Convex Polygon Collisions");
	SetTargetFPS(24);
	PolygonicCollisions polyColls;
	polyColls.OnUserCreate();

	while (!WindowShouldClose()) 
	{
		BeginDrawing();
		ClearBackground(DARKBLUE);

		for (auto& r : polyColls.vecShapes) 
		{
			//Draw Boundary
			for (int i = 0; i < r.p.size(); i++)
				DrawLine(r.p[i].x, r.p[i].y, r.p[(i + 1) % r.p.size()].x, r.p[(i + 1) % r.p.size()].y, (r.overlap ? RED : WHITE));

			//Draw Directions
			DrawLine(r.p[0].x, r.p[0].y, r.pos.x, r.pos.y, (r.overlap ? RED : WHITE));
		}

		polyColls.onUserUpdate();

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
