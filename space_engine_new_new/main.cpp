#include "raylib.h"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

#include "constants.h"


float distance(Vector3 point1, Vector3 point2)
{
	float dx = point1.x - point2.x, dy = point1.y - point2.y, dz = point1.z - point2.z;

	return sqrt(dx * dx + dy * dy + dz * dz);
}


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

		updateCameraPosition(distance(camera.position, camera.target));

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

		updateCameraPosition(distance(camera.position, camera.target));
	}
};


class Object
{
private:
	std::string name;
	Vector3 position, velocity, force; // [position] = meter, [velocity] = meter/second, [force] = Newton
	Color color;
	float mass, radius; // [mass] = kilogram, [radius] = meter

public:
	Object(std::string name, Vector3 position, float mass, float radius, Color color = BLACK, Vector3 velocity = { 0.0f, 0.0f, 0.0f }, Vector3 force = { 0.0f, 0.0f, 0.0f })
	{
		this->name = name;

		this->position = position;
		this->velocity = velocity;
		this->force = force;

		this->mass = mass;
		this->radius = radius;

		this->color = color;
	}

	Vector3 getPosition()
	{
		return position;
	}

	float getMass()
	{
		return mass;
	}

	float getRadius()
	{
		return radius;
	}

	Vector3 getForce()
	{
		return force;
	}

	void setForce(Vector3 newForce)
	{
		force = newForce;
	}

	void tick(float dt)
	{
		Vector3 acceleration = { force.x / mass / SCALE, force.y / mass / SCALE, force.z / mass / SCALE };
		float scaledRadius = radius / SCALE;

		velocity = { velocity.x + acceleration.x * dt, velocity.y + acceleration.y * dt, velocity.z + acceleration.z * dt };
		position = { position.x + velocity.x * dt, position.y + velocity.y * dt, position.z + velocity.z * dt };

		Vector3 scaledPosition = { position.x / SCALE, position.y / SCALE , position.z / SCALE };

		std::cout << scaledPosition.x << ' ' << scaledPosition.y << ' ' << scaledPosition.z << std::endl;

		DrawSphereEx(scaledPosition, scaledRadius, SPHERE_RINGS, SPHERE_SLICES, color);
	}
};


int main()
{
	InitWindow(WIDTH, HEIGHT, "Space engine");

	CameraController camera;

	std::vector<Object> objects;

	Object sun("Sun", { 0.0f, 0.0f, 0.0f }, 1.9885f * (float)pow(10, 30), 0.696f * (float)pow(10, 9), ORANGE);
	Object earth("Earth", { 1.496f * (float)pow(10, 11), 0.0f, 0.0f }, (float)5.9726 * (float)pow(10, 24), 6371000.0f, GREEN, { 0.0f, 0.0f, 29780.0f });

	objects.push_back(sun);
	objects.push_back(earth);

	HideCursor();

	SetTargetFPS(FPS);
	
	while (!WindowShouldClose())
	{
		float wheelMove = GetMouseWheelMove(), dt = 100.0f;
		Vector2 mouseDelta = GetMouseDelta();

		float cameraSpeed = camera.getSpeed(), cameraAngleSpeed = camera.getAngleSpeed();

		Camera3D innerCamera = camera.getCamera();

		if (wheelMove)
			camera.updateCameraPosition(distance(innerCamera.position, innerCamera.target) - wheelMove);

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

		for (int i = 0; i < objects.size(); i++)
			objects[i].setForce({ 0.0f, 0.0f, 0.0f });

		BeginDrawing();
		BeginMode3D(innerCamera);

		ClearBackground(RAYWHITE);
		DrawGrid(1500, 1.0f);
		
		for (int i = 0; i < objects.size(); i++)
		{
			for (int j = 0; j < objects.size(); j++)
			{
				if (i == j)
					continue;

				Vector3 pos1 = objects[i].getPosition(), pos2 = objects[j].getPosition();

				float dist = distance(pos1, pos2);

				if (dist > objects[i].getRadius() + objects[i].getRadius())
				{
					float forceValue = G * objects[i].getMass() * objects[j].getMass() / (dist * dist);
					Vector3 directionNormalized = { (pos1.x - pos2.x) / dist, (pos1.y - pos2.y) / dist, (pos1.z - pos2.z) / dist };
					
					objects[j].setForce({ directionNormalized.x * forceValue, directionNormalized.y * forceValue, directionNormalized.z * forceValue });

					std::cout << objects[i].getForce().x << ' ' << objects[i].getForce().y << ' ' << objects[i].getForce().z << std::endl;
				}
			}
		}

		for (int i = 0; i < objects.size(); i++)
			objects[i].tick(dt);

		SetMousePosition(WIDTH / 2, HEIGHT / 2);

		EndMode3D();

		DrawFPS(10, 10);

		Vector3 cameraPos = innerCamera.position;
		DrawText(TextFormat("X: %i Y: %i Z: %i", (int)cameraPos.x, (int)cameraPos.y, (int)cameraPos.z), 10, 30, 18, RED);
		
		EndDrawing();
	}

	CloseWindow();

	return 0;
}