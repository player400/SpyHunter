#pragma once

//Class representing ,,spies" (hostile cars)
class Hostile : public NPC
{
public:

	//Function checks if there would be a collision with the player if the entity was in the given point (for checking current and potential collisions)
	int check_for_player_collisions(Pos point)
	{
		int collisions = 0;
		Vector<Pos>* corners = calculate_corners(point, width, height);
		for (int j = 0; j < 4; j++)
		{
			if (player->handle_collisions(corners->get_ptr()[j]))
			{
				collisions++;
			}
		}
		delete corners;
		return collisions;
	}

	//Function checks if there would be a collision with anything OTHER THAN THE PLAYER AND NEUTRAL if the entity was in the given point (for checking current and potential collisions)
	int check_for_collisions(Pos point)
	{
		int collisions = 0;
		Vector<Pos>* corners = calculate_corners(point, width, height);
		for (int j = 0; j < 4; j++)
		{
			if (road->handle_collsions(corners->get_ptr()[j]))
			{
				collisions++;
			}
			if (player->handle_collisions(corners->get_ptr()[j]))
			{
				collisions++;
			}
			for (int i = 0; i < hostiles->size; i++)
			{
				if (&hostiles->get_ptr()[i] != this && hostiles->get_ptr()[i].handle_collisions(corners->get_ptr()[j]))
				{
					collisions++;
				}
			}
		}
		delete corners;
		return collisions;
	}

	//function uses 4 rays to determine if (and how far) the player is in front of, behind, or next to the enitity
	int vision(int direction)
	{
		if (direction == UP)
		{
			for (int i = (int)position.vertical; i >= 0; i-= VISION_SAMPLING_PRECISION)
			{
				Pos point = position;
				point.vertical = i;
				if (check_for_player_collisions(point))
				{
					return abs(i - (int)position.vertical);
				}
			}
		}
		if (direction == DOWN)
		{
			for (int i = (int)position.vertical; i < SCREEN_HEIGHT; i+= VISION_SAMPLING_PRECISION)
			{
				Pos point = position;
				point.vertical = i;
				if (check_for_player_collisions(point))
				{
					return abs(i - (int)position.vertical);
				}
			}
		}
		if (direction == RIGHT)
		{
			for (int i = position.horizontal; i < SCREEN_WIDTH; i+= VISION_SAMPLING_PRECISION)
			{
				Pos point = position;
				point.horizontal = i;
				if (check_for_player_collisions(point))
				{
					return abs(i - position.horizontal);
				}
			}
		}
		if (direction == LEFT)
		{
			for (int i = position.horizontal; i >= 0; i-= VISION_SAMPLING_PRECISION)
			{
				Pos point = position;
				point.horizontal = i;
				if (check_for_player_collisions(point))
				{
					return abs(i - position.horizontal);
				}
			}
		}
		return 0;
	}

	//Enity beaviour logic
	void ai()
	{
		int right = vision(RIGHT);
		int left = vision(LEFT);
		//Go towards the player if detected
		if (left)
		{
			turn(-1);
		}
		else if (right)
		{
			turn(1);
		}
		//Random turn if player is not detected
		else if (!(rand() % NPC_TURNING_CHANCE))
		{
			turn(rand() % 3 - 1);
		}
	}

	//Check collisions and execute logic
	void manage()
	{
		damage(check_for_collisions(position) * COLLISION_DAMAGE);
		if (check_for_player_collisions(position))
		{
			damage(COLLISION_DAMAGE);
			if(hp <= 0) //If the entity died after collison with the player, the ,,killed_by_player" flag is set to true
			{ 
				killed_by_player = true;
			}
		}
		if (!(rand() % NPC_REACTION_CHANCE)) //AI logic is not executed in every frame to give player a chance against the computer
		{
			ai();
		}
	}

	Hostile(Rendering* rendering_engine, Player* player, Vector<Hostile>*hostiles, Vector<Neutral>* neutrals, Road* road)
	{
		this->road = road;
		this->hostiles = hostiles;
		this->player = player;
		this->rendering_engine = rendering_engine;
		texture = rendering_engine->LoadTexture(HOSTILE_TEXTURE);
		height = texture->h;
		width = texture->w;
		default_hp = HOSTILE_HP;
		speed = HOSTILE_SPEED;
		restart();
	}
};