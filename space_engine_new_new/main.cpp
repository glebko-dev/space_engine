#include <iostream>
#include <string>
#include "raylib.h"
#include <cmath>

#include "constants.h"


class CameraController
{
private:
	Camera3D camera;

	Vector2	rotation;

	float speed, acceleration, slowDown, angleSpeed;

public:
	CameraController(float x = 10.0f, float y = 0.0f, float z = 0.0f, float speed = CAMERA_SPEED, float acceleration = CAMERA_ACCELERATION, float slowDown = CAMERA_SLOWDOWN, float angleSpeed = PI / 50, Vector2 rotation = {0.0f, 0.0f})
	{
		this->speed = speed;
		this->angleSpeed = angleSpeed;

		this->acceleration = acceleration;
		this->slowDown = slowDown;

		this->rotation = rotation;
		
		this->camera.position = { x, y, z };
		this->camera.target = { x - 10.0f, y, z };
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

	float getAcceleration()
	{
		return acceleration;
	}

	float getSlowDown()
	{
		return slowDown;
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

	void incSpeed(float value)
	{
		speed += value;
	}

	void updateCameraPosition(float distance)
	{
		camera.position.x = camera.target.x + distance * cos(rotation.y) * cos(rotation.x);
		camera.position.y = camera.target.y + distance * sin(rotation.y);
		camera.position.z = camera.target.z + distance * cos(rotation.y) * sin(rotation.x);
	}

	void move(Vector2 positionDelta, bool moveSide = false)
	{
		if (moveSide)
			rotation.x += PI / 2;

		camera.position.x += positionDelta.x * cos(rotation.x);
		camera.position.y += positionDelta.y;
		camera.position.z += positionDelta.x * sin(rotation.x);

		camera.target.x += positionDelta.x * cos(rotation.x);
		camera.target.y += positionDelta.y;
		camera.target.z += positionDelta.x * sin(rotation.x);

		if (moveSide)
			rotation.x -= PI / 2;
	}

	void rotate(Vector2 rotationDelta)
	{
		rotation.x += rotationDelta.x;

		updateCameraPosition(getDistanceToTarget());

		if (rotation.y + rotationDelta.y >= PI / 2)
		{
			rotation.y = PI / 2 - 0.05f;
			return;
		}

		if (rotation.y + rotationDelta.y <= -PI / 2)
		{
			rotation.y = -PI / 2 + 0.05f;
			return;
		}

		rotation.y += rotationDelta.y;

		updateCameraPosition(getDistanceToTarget());
	}
};


class Object
{
private:
	Vector3 position, velocity, force; // [position] = meter, [velocity] = meter/second, [force] = Newton
	Color color;
	float mass, radius; // [mass] = kilogram, [radius] = meter

public:
	Object(Vector3 position, float mass, float radius, Color color = BLACK, Vector3 velocity = { 0.0f, 0.0f, 0.0f }, Vector3 force = { 0.0f, 0.0f, 0.0f })
	{
		this->position = position;
		this->velocity = velocity;
		this->force = force;

		this->mass = mass;
		this->radius = radius;

		this->color = color;
	}

	void draw()
	{
		Vector3 scaledPosition = { position.x / SCALE, position.y / SCALE , position.z / SCALE };
		float scaledRadius = radius / SCALE;

		std::cout << scaledPosition.x << ' ' << scaledPosition.y << ' ' << scaledPosition.z << std::endl;

		DrawSphereEx(scaledPosition, scaledRadius, SPHERE_RINGS, SPHERE_SLICES, color);
	}
};


int main()
{
	InitWindow(WIDTH, HEIGHT, "Space engine");

	CameraController camera;

	Object sun({ 0.0f, 0.0f, 0.0f }, 1.9885f * (float)pow(10, 30), 0.696f * (float)pow(10, 9), ORANGE);
	Object earth({ 1.496f * (float)pow(10, 11), 0.0f, 0.0f }, (float)5.9726 * (float)pow(10, 24), 6371000.0f, GREEN);
	
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
			camera.rotate({ mouseDelta.x / 100, mouseDelta.y / 100 });

		if (IsKeyDown(KEY_W))
			camera.move({ -cameraSpeed, 0.0f });

		if (IsKeyDown(KEY_S))
			camera.move({ cameraSpeed, 0.0f });

		if (IsKeyDown(KEY_A))
			camera.move({ cameraSpeed, 0.0f }, true);

		if (IsKeyDown(KEY_D))
			camera.move({ -cameraSpeed, 0.0f }, true);

		if (IsKeyDown(KEY_SPACE))
			camera.move({ 0.0f, cameraSpeed }, true);

		if (IsKeyDown(KEY_LEFT_SHIFT))
			camera.move({ 0.0f, -cameraSpeed }, true);

		if (IsKeyPressed(KEY_LEFT_CONTROL))
		{
			float cameraSpeed = camera.getSpeed(), cameraAcceleration = camera.getAcceleration();

			if (cameraSpeed == CAMERA_SPEED)
				camera.incSpeed(cameraAcceleration);
		}

		if (IsKeyUp(KEY_LEFT_CONTROL))
		{
			float cameraSpeed = camera.getSpeed(), cameraAcceleration = camera.getAcceleration();

			if (cameraSpeed > CAMERA_SPEED)
				camera.incSpeed(-cameraAcceleration);
		}

		if (IsKeyPressed(KEY_LEFT_ALT))
		{
			float cameraSpeed = camera.getSpeed(), cameraSlowDown = camera.getSlowDown();

			if (cameraSpeed == CAMERA_SPEED)
				camera.incSpeed(-cameraSlowDown);
		}

		if (IsKeyUp(KEY_LEFT_ALT))
		{
			float cameraSpeed = camera.getSpeed(), cameraSlowDown = camera.getSlowDown();

			if (cameraSpeed < CAMERA_SPEED)
				camera.incSpeed(cameraSlowDown);
		}

		SetMousePosition(WIDTH / 2, HEIGHT / 2);

		BeginDrawing();

		ClearBackground(RAYWHITE);

		BeginMode3D(camera.getCamera());

		DrawGrid(1500, 1.0f);

		sun.draw();
		earth.draw();

		EndMode3D();

		DrawFPS(10, 10);

		Vector3 cameraPos = camera.getCamera().position;
		
		DrawText(TextFormat("X: %i Y: %i Z: %i", (int)cameraPos.x, (int)cameraPos.y, (int)cameraPos.z), 10, 30, 18, RED);
		
		EndDrawing();
	}

	CloseWindow();

	return 0;
}