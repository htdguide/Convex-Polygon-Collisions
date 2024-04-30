#include "main.h"
#include "raylib.h"

int main() {
	InitWindow(800, 600, "Convex Polygon Collisions");
	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		BeginDrawing();


		ClearBackground(RAYWHITE);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}