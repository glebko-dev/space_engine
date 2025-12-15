#include "raylib.h"
#include <cmath>

#include "constants.h"


class CameraController
{
private:
	Camera3D camera;
	float rotationHor, rotationVert;

public:
	CameraController(float x = 10.0f, float y = 0.0f, float z = 0.0f, float rotationHor = 0.0f, float rotationVert = 0.0f)
	{
		this->rotationHor = rotationHor;
		this->rotationVert = rotationVert;

		this->camera.position = { x, y, z };
		this->camera.target = { 0.0f, 0.0f, 0.0f };
		this->camera.up = { 0.0f, 1.0f, 0.0f };
		this->camera.fovy = 45.0f;
		this->camera.projection = CAMERA_PERSPECTIVE;
	}

	Camera3D getCamera()
	{
		return camera;
	}

	float getDistanceToTarget()
	{
		float dx = camera.position.x - camera.target.x, dy = camera.position.y - camera.target.y, dz = camera.position.z - camera.target.z;

		return sqrt(dx * dx + dy * dy + dz * dz);
	}

	void updateCameraPosition(float distance)
	{
		camera.position.x = camera.target.x + distance * cos(rotationVert) * cos(rotationHor);
		camera.position.y = camera.target.y + distance * sin(rotationVert);
		camera.position.z = camera.target.z + distance * cos(rotationVert) * sin(rotationHor);
	}

	void setDistance(float distance)
	{
		updateCameraPosition(distance);
	}

	void rotateHor(float angle)
	{
		rotationHor += angle;

		updateCameraPosition(getDistanceToTarget());
	}

	void rotateVert(float angle)
	{
		if (rotationVert + angle > PI / 2)
			rotationVert = PI / 2;

		else if (rotationVert + angle < -PI / 2)
			rotationVert = -PI / 2;

		rotationVert += angle;

		updateCameraPosition(getDistanceToTarget());
	}
};


int main()
{
	InitWindow(WIDTH, HEIGHT, "Space engine");

	CameraController camera;

	Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };

	SetTargetFPS(FPS);
	
	while (!WindowShouldClose())
	{
		float wheelMove = GetMouseWheelMove();

		if (wheelMove)
			camera.updateCameraPosition(camera.getDistanceToTarget() - wheelMove);

		if (IsKeyDown(KEY_W))
			camera.rotateVert(PI / 100);

		if (IsKeyDown(KEY_S))
			camera.rotateVert(-PI / 100);

		if (IsKeyDown(KEY_A))
			camera.rotateHor(PI / 100);

		if (IsKeyDown(KEY_D))
			camera.rotateHor(-PI / 100);

		BeginDrawing();

		ClearBackground(RAYWHITE);

		BeginMode3D(camera.getCamera());

		DrawCube(cubePosition, 2.0f, 2.0f, 2.0f, RED);
		DrawCubeWires(cubePosition, 2.0f, 2.0f, 2.0f, MAROON);

		DrawGrid(10, 1.0f);

		EndMode3D();

		DrawFPS(10, 10);

		EndDrawing();
	}

	CloseWindow();

	return 0;
}