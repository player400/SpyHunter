#pragma once

class Player;
class Neutral;
class Hostile;

//Abstract class representing all entities on the road
class Car
{
protected:
	Pos position;
	SDL_Surface* texture;
	Rendering* rendering_engine;

public:
	int hp;
	int height;
	int width;

	void turn(int delta)
	{
		position.horizontal += TURNING_SPEED * delta;
	}

	bool is_alive()
	{
		if (hp > 0)
		{
			return true;
		}
		return false;
	}

	void damage(int value)
	{
		hp -= value;
	}

	//Method checks if the given point is within the entity
	bool handle_collisions(Pos point)
	{
		int left_bound = position.horizontal - width / 2;
		int right_bound = position.horizontal + width / 2;
		int upper_bound = position.vertical + height / 2;
		int lower_bound = position.vertical - height / 2;
		if (point.horizontal >= left_bound && point.horizontal <= right_bound)
		{
			if (point.vertical >= lower_bound && point.vertical <= upper_bound)
			{
				return true;
			}
		}
		return false;
	}

	void render()
	{
		rendering_engine->DrawSurface(texture, position.horizontal, (int)position.vertical);
	}

	Pos get_position()
	{
		return position;
	}

};

class NPC : public Car
{
protected:
	int speed;
	int default_hp;
	Road* road;
	Vector<Neutral>* neutrals = nullptr;
	Vector<Hostile>* hostiles = nullptr;
	Player* player = nullptr;

public:
	//Variable is set after the entity is killed and indicates it the player destroyed it
	bool killed_by_player = false;

	//Function resets the car to it's starting position on top of the window
	void restart()
	{
		position.vertical = (-1) * (height / 2) - rand() % NPC_VERTICAL_SPREAD;
		position.horizontal = rand() % (ROAD_WIDTH - ROAD_SPAWN_BUFFER);
		position.horizontal += (SCREEN_WIDTH - ROAD_WIDTH + ROAD_SPAWN_BUFFER) / 2;
		hp = default_hp;
		killed_by_player = false;
	}

	//Function updates the entity position (if the entity is outside of the window it is restored to it's starting position) and renders it
	void refresh(double time, int player_speed)
	{
		position.vertical += (player_speed - speed) * time;
		if (position.vertical - height / 2 > SCREEN_HEIGHT || position.vertical + height < 0)
		{
			restart();
		}
		render();
	}
};
