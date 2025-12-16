#include "raylib.h"
#include <cmath>

#include "constants.h"


class CameraController
{
private:
	Camera3D camera;
	float rotationHor, rotationVert, speed, angleSpeed;

public:
	CameraController(float x = 10.0f, float y = 0.0f, float z = 0.0f, float speed = 0.5f, float angleSpeed = PI / 50, float rotationHor = 0.0f, float rotationVert = 0.0f)
	{
		this->rotationHor = rotationHor;
		this->rotationVert = rotationVert;

		this->speed = speed;
		this->angleSpeed = angleSpeed;

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

	float getSpeed()
	{
		return speed;
	}

	float getAngleSpeed()
	{
		return angleSpeed;
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

	void move(float moveHor, float moveVert, bool moveSide = false)
	{
		if (moveSide)
			rotationHor += PI / 2;

		camera.position.x += moveHor * cos(rotationHor);
		camera.position.y += moveVert;
		camera.position.z += moveHor * sin(rotationHor);

		camera.target.x += moveHor * cos(rotationHor);
		camera.target.y += moveVert;
		camera.target.z += moveHor * sin(rotationHor);

		if (moveSide)
			rotationHor -= PI / 2;
	}

	void rotate(float angleHor, float angleVert)
	{
		rotationHor += angleHor;
		updateCameraPosition(getDistanceToTarget());

		if (rotationVert + angleVert >= PI / 2)
		{
			rotationVert = PI / 2 - 0.05f;
			return;
		}

		if (rotationVert + angleVert <= -PI / 2)
		{
			rotationVert = -PI / 2 + 0.05f;
			return;
		}

		rotationVert += angleVert;
		updateCameraPosition(getDistanceToTarget());
	}
};


int main()
{
	InitWindow(WIDTH, HEIGHT, "Space engine");

	CameraController camera;

	Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };

	HideCursor();

	SetTargetFPS(FPS);
	
	while (!WindowShouldClose())
	{
		float wheelMove = GetMouseWheelMove();
		Vector2 mouseDelta = GetMouseDelta();

		float cameraSpeed = camera.getSpeed(), cameraAngleSpeed = camera.getAngleSpeed();

		if (wheelMove)
			camera.updateCameraPosition(camera.getDistanceToTarget() - wheelMove);

		if (!(mouseDelta.x == 0.0f && mouseDelta.y == 0.0f))
			camera.rotate(mouseDelta.x / 100, mouseDelta.y / 100);

		if (IsKeyDown(KEY_W))
			camera.move(-cameraSpeed, 0.0f);

		if (IsKeyDown(KEY_S))
			camera.move(cameraSpeed, 0.0f);

		if (IsKeyDown(KEY_A))
			camera.move(cameraSpeed, 0.0f, true);

		if (IsKeyDown(KEY_D))
			camera.move(-cameraSpeed, 0.0f, true);

		if (IsKeyDown(KEY_SPACE))
			camera.move(0.0f, cameraSpeed, true);

		if (IsKeyDown(KEY_LEFT_SHIFT))
			camera.move(0.0f, -cameraSpeed, true);

		SetMousePosition(WIDTH / 2, HEIGHT / 2);

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