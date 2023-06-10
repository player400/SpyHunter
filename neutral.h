#pragma once

//Class representing neutral cars
class Neutral : public NPC
{
public:
	//Method checks if the entity would be in collison with THE PLAYER it if was currently in the given point
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

	//Method checks if the entity would be in collison with ANY OBJECT it if was currently in the given point
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
			for (int i = 0; i < neutrals->size; i++ )
			{
				if (&neutrals->get_ptr()[i] != this && neutrals->get_ptr()[i].handle_collisions(corners->get_ptr()[j]))
				{
					collisions++;
				}
			}
			for (int i = 0; i < hostiles->size; i++)
			{
				if (hostiles->get_ptr()[i].handle_collisions(corners->get_ptr()[j]))
				{
					collisions++;
				}
			}
		}
		delete corners;
		return collisions;
	}

	//Method uses 4 rays to determine if (and how far) the nearest obstacle is in front, next to, or behind the entity
	int vision(int direction)
	{
		if (direction == UP)
		{
			for (int i = (int)position.vertical; i >= 0; i-=VISION_SAMPLING_PRECISION)
			{
				Pos point = position;
				point.vertical = i;
				if (check_for_collisions(point))
				{
					return abs(i-(int)position.vertical);
				}
			}
		}
		if (direction == DOWN)
		{
			for (int i = (int)position.vertical; i < SCREEN_HEIGHT; i+=VISION_SAMPLING_PRECISION)
			{
				Pos point = position;
				point.vertical = i;
				if (check_for_collisions(point))
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
				if (check_for_collisions(point))
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
				if (check_for_collisions(point))
				{
					return abs(i - position.horizontal);
				}
			}
		}
		return 0;
	}

	//Method containing the neutral entity logic (random turns and avoiding obstacles)
	void ai()
	{

		int up = vision(UP);
		int right = vision(RIGHT);
		int left = vision(LEFT);
		int down = vision(DOWN);
		bool do_turn = false;
		//If obstacle is close to the entity, turn
		if (up > 0 && up <= height) 
		{
			do_turn = true;
		}
		if (down > 0 && down <= height)
		{
			do_turn = true;
		}
		else if (left > 0 && vision(LEFT) <= width)
		{
			do_turn = true;
		}
		else if (right > 0 && vision(RIGHT) <= width)
		{
			do_turn = true;
		}
		//Turn in the direction where the nerest obstacle is furthest from the entity
		if (do_turn)
		{
			if (right >= left)
			{
				turn(1);
			}
			else
			{
				turn(-1);
			}
		}
		else if(!(rand()%NPC_TURNING_CHANCE))
		{
			turn(rand() % 3 - 1);
		}
	}

	void manage()
	{
		damage(check_for_collisions(position) * COLLISION_DAMAGE);
		if (check_for_player_collisions(position) && hp <= 0)
		{
			killed_by_player = true;
		}
		if (!(rand() % NPC_REACTION_CHANCE))
		{
			ai();
		}
	}

	Neutral(Rendering* rendering_engine, Player* player, Vector<Hostile>* hostiles, Vector<Neutral>* neutrals, Road* road)
	{
		this->road = road;
		this->hostiles = hostiles;
		this->neutrals = neutrals;
		this->player = player;
		this->rendering_engine = rendering_engine;
		texture = rendering_engine->LoadTexture(NEUTRAL_TEXTURE);
		height = texture->h;
		width = texture->w;
		default_hp = NEUTRAL_HP;
		speed = NEUTRAL_SPEED;
		restart();
	}
};