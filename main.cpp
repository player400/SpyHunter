#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<time.h>
#include<stdlib.h>

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

#define SCREEN_WIDTH 700
#define SCREEN_HEIGHT 600

//Map style
#define ROAD_WIDTH 300
#define ROAD_TEXTURE "land_texture2.bmp"
#define ROAD_COLOR 120, 120, 120
#define HUNTER_TEXTURE "hunter2.bmp"
#define HOSTILE_TEXTURE "hostile.bmp"
#define NEUTRAL_TEXTURE "neutral.bmp"

//NPC settings
#define NPC_REACTION_CHANCE 10
#define HOSTILE_SPAWNING_CHANCE 1000
#define NEUTRAL_SPAWNING_CHANCE 1000
#define HOSTILE_SPEED 480
#define NEUTRAL_SPEED 250
#define MAX_HOSTILES 3
#define MAX_NEUTRALS 10
#define NPC_TURNING_CHANCE 50
#define VISION_SAMPLING_PRECISION 3
#define ROAD_SPAWN_BUFFER 20
#define NPC_VERTICAL_SPREAD 300

//General movement settings
#define DEFAULT_PLAYER_SPEED 500
#define MAX_PLAYER_SPEED 10000
#define MIN_PLAYER_SPEED 150
#define TURNING_SPEED 5
#define ACCELERATION 5
#define COLLISION_DAMAGE 5

//Scoring rules
#define POINTS_PER_KILL 250
#define NEUTRAL_KILL_COOLDOWN 5
#define POINTS_PER_SECOND 150
#define POINTS_PER_SECOND_HIGH_SPEED 250
#define SPEED_QUOTA 600
#define INFINITE_CARS_PERIOD 30
#define POINTS_FOR_ADDITIONAL_CAR 5000
#define GOING_OUT_OF_THE_ROAD_TOLERANCE 10

//HP
#define PLAYER_HP 100
#define HOSTILE_HP 50
#define NEUTRAL_HP 5

//Other
#define UP 3
#define DOWN 2
#define RIGHT 1
#define LEFT 0

#define HITBOX_MARGIN 5
#define INIT_VECTOR_CAP 2

struct Pos
{
	double vertical;
	int horizontal;
};

#include"vector.h"

//Function calculates corners of an entity for the purpose of collision checking, taking HITBOX_MARGIN into accound (returned vector HAS TO BE DELETED)
Vector<Pos>* calculate_corners(Pos center, int width, int height)
{
	width -= 2 * HITBOX_MARGIN;
	height -= 2 * HITBOX_MARGIN;
	Vector<Pos>* corners = new Vector<Pos>;
	Pos top_left;
	top_left.horizontal = center.horizontal - width / 2;
	top_left.vertical = center.vertical + height / 2;
	corners->push_back(top_left);
	Pos top_right;
	top_left.horizontal = center.horizontal + width / 2;
	top_left.vertical = center.vertical + height / 2;
	corners->push_back(top_left);
	Pos bottom_left;
	top_left.horizontal = center.horizontal - width / 2;
	top_left.vertical = center.vertical - height / 2;
	corners->push_back(top_left);
	Pos bottom_right;
	top_left.horizontal = center.horizontal + width / 2;
	top_left.vertical = center.vertical - height / 2;
	corners->push_back(top_left);
	return corners;
}

#include"rendering.h"
#include"road.h"
#include"car.h"
#include"player.h"
#include"hostile.h"
#include"neutral.h"

//Main class represanting the game status
class Game
{
private:
	int player_speed = DEFAULT_PLAYER_SPEED;
	Road road;
	Rendering* rendering_engine;
	SDL_Surface* screen;
	double time_since_start = 0;
	double time_since_scoring = 0;
	int points = 0;
	int points_since_additional_car = 0;
	int cars_left = 2;
	bool do_reset_speed = true;
	double point_cooldown = 0;

public:
	Player player;
	Vector<Hostile>hostiles;
	Vector<Neutral>neutrals;
	bool finished = false;

	//Function changes the speed and stops the speed resetting process
	void change_speed(int value)
	{
		if (player_speed + value >= MAX_PLAYER_SPEED)
		{
			player_speed = MAX_PLAYER_SPEED;
		}
		else if (player_speed + value <= MIN_PLAYER_SPEED)
		{
			player_speed = MIN_PLAYER_SPEED;
		}
		else
		{
			player_speed += value;
		}
		do_reset_speed = false;
	}

	//Reseting player speed to default (after speed-up / slow-down key has been released)
	void reset_speed()
	{
		do_reset_speed = true;
		if (player_speed - ACCELERATION / 2 >= DEFAULT_PLAYER_SPEED)
		{
			player_speed -= ACCELERATION / 2;
		}
		else if (player_speed + ACCELERATION / 2 <= DEFAULT_PLAYER_SPEED)
		{
			player_speed += ACCELERATION / 2;
		}
		else
		{
			player_speed = DEFAULT_PLAYER_SPEED;
			do_reset_speed = false;
		}
	}

	//Function displays all objects (and calculates movement based on speed)
	void render(double time)
	{
		road.refresh(time);
		player.render();
		for (int i = 0; i < hostiles.size; i++)
		{
			hostiles.get_ptr()[i].refresh(time, player_speed);
		}
		for (int i = 0; i < neutrals.size; i++)
		{
			neutrals.get_ptr()[i].refresh(time, player_speed);
		}
	}

	void end(const char* message)
	{
		//Sending ,,finish the game" signal 
		finished = true;
		rendering_engine->DrawString(SCREEN_WIDTH / 2 - strlen(message) * 8 / 2, SCREEN_HEIGHT / 2 - 16, message);
		char text[128];
		sprintf(text, "Wynik: %d", points);
		rendering_engine->DrawString(SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2, text);
		sprintf(text, "Nowa gra: n. Wylacz program: esc");
		rendering_engine->DrawString(SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2 + 16, text);
	}

	//Function checking collisons of the player with the road border (taking the,,tolerance" into account) 
	bool player_outside_of_the_road(int tolerance)
	{
		Pos position = player.get_position();
		if (position.horizontal + player.width/2 - tolerance > (SCREEN_WIDTH + ROAD_WIDTH) / 2)
		{
			return true;
		}
		if (position.horizontal - player.width/2 + tolerance < (SCREEN_WIDTH - ROAD_WIDTH) / 2)
		{
			return true;
		}
		return false;
	}

	void add_points(int value)
	{
		if (point_cooldown <= 0)
		{
			if (!player_outside_of_the_road(0))
			{
				points += value;
				points_since_additional_car += value;
			}
		}
	}

	void manage_lives()
	{
		if (!player.is_alive())
		{
			if (time_since_start < INFINITE_CARS_PERIOD)
			{
				player.restart();
			}
			else if (cars_left > 0)
			{
				cars_left--;
				player.restart();
			}
			else
			{
				end("Gra skonczona! Zostales zniszczony!");
			}
		}
		for (int i = 0; i < neutrals.size; i++)
		{
			if (!neutrals.get_ptr()[i].is_alive())
			{
				if (neutrals.get_ptr()[i].killed_by_player)
				{
					point_cooldown = NEUTRAL_KILL_COOLDOWN;
				}
				neutrals.get_ptr()[i].restart();
			}
		}
		for (int i = 0; i < hostiles.size; i++)
		{
			if (!hostiles.get_ptr()[i].is_alive())
			{
				if (hostiles.get_ptr()[i].killed_by_player)
				{
					add_points(POINTS_PER_KILL);
				}
				hostiles.get_ptr()[i].restart();
			}
		}
		if (points_since_additional_car >= POINTS_FOR_ADDITIONAL_CAR)
		{
			if (time_since_start >= INFINITE_CARS_PERIOD)
			{
				cars_left++;
			}
			points_since_additional_car = 0;
		}
	}

	//NPC spawning
	void manage_npc()
	{
		if (hostiles.size < MAX_HOSTILES)
		{
			int random_number = rand() % HOSTILE_SPAWNING_CHANCE;
			if (!random_number)
			{
				Hostile hostile = Hostile(rendering_engine, &player, &hostiles, &neutrals, &road);
				hostiles.push_back(hostile);
			}
		}
		if (neutrals.size < MAX_NEUTRALS)
		{
			int random_number = rand() % NEUTRAL_SPAWNING_CHANCE;
			if (!random_number)
			{
				Neutral neutral = Neutral(rendering_engine, &player, &hostiles, &neutrals, &road);
				neutrals.push_back(neutral);
			}
		}
	}

	//Function computes if the player's car is hitting any object
	void check_for_player_collisions()
	{
		Vector<Pos>* corners = calculate_corners(player.get_position(), player.width, player.height);
		for (int j = 0; j < 4; j++)
		{
			if (player_outside_of_the_road(GOING_OUT_OF_THE_ROAD_TOLERANCE))
			{
				player.damage(COLLISION_DAMAGE);
			}
			for (int i = 0; i < hostiles.size; i++)
			{
				if (hostiles.get_ptr()[i].handle_collisions(corners->get_ptr()[j]))
				{
					player.damage(COLLISION_DAMAGE);
				}
			}
			for (int i = 0; i < neutrals.size; i++)
			{
				if (neutrals.get_ptr()[i].handle_collisions(corners->get_ptr()[j]))
				{
					player.damage(COLLISION_DAMAGE);
				}
			}
		}
		delete corners;
	}

	//Function computes if NPC's are hitting any object
	void check_for_npc_collisions()
	{
		for (int i = 0; i < neutrals.size; i++)
		{
			neutrals.get_ptr()[i].manage();
		}
		for (int i = 0; i < hostiles.size; i++)
		{
			hostiles.get_ptr()[i].manage();
			//Collisions of hostiles with neutrals are computed outside of the Hostile class
			Vector<Pos>* corners = calculate_corners(player.get_position(), player.width, player.height);
			for (int j = 0; j < 4; j++)
			{
				for (int i = 0; i < neutrals.size; i++)
				{
					if (neutrals.get_ptr()[i].handle_collisions(corners->get_ptr()[j]))
					{
						hostiles.get_ptr()[i].damage(COLLISION_DAMAGE);
					}
				}
			}
			delete corners;
		}
	}

	void refresh(double time)
	{
		//If the game has been finished, all the actions are blocked
		if (!finished)
		{
			//Checking collisions
			check_for_npc_collisions();
			check_for_player_collisions();
			//Spawning NPC's
			manage_npc();
			//Checking if any entity has been killed (and managing points / additional cars accordingly)
			manage_lives();
			render(time);
			//If the speed up/slow down key has been released, reset_speed() function is called and locks itself in a cycle to gradually restore default player speed
			if (do_reset_speed)
			{
				reset_speed();
			}
			char text[128];
			sprintf(text, "Mateusz Nurczynski, 193053");
			rendering_engine->DrawString(SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, 10, text);
			sprintf(text, "Czas trwania: %.1lf s. Punkty: %d. Predkosc: %d, HP: %d, Zycia: %d.", time_since_start, points, player_speed, player.hp, cars_left+1);
			rendering_engine->DrawString(SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, 26, text);
			sprintf(text, "Esc - wyjscie, \030 - przyspieszenie, \031 - zwolnienie");
			rendering_engine->DrawString(SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, 42, text);
			sprintf(text, "n - nowa gra, f - zakoncz gre, spacja - strzal");
			rendering_engine->DrawString(SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, 58, text);
			sprintf(text, "a, b, c, d, e, f, i, m");
			rendering_engine->DrawString(SCREEN_WIDTH - strlen(text) * 8, SCREEN_HEIGHT - 10, text);
			time_since_start += time;
			time_since_scoring += time;
			int upscore = 0;
			if (time_since_scoring >= 1)
			{
				if (player_speed >= SPEED_QUOTA)
				{
					upscore = POINTS_PER_SECOND_HIGH_SPEED;
				}
				else
				{
					upscore = POINTS_PER_SECOND;
				}
				time_since_scoring = 0;
			}
			add_points(upscore);
			if (point_cooldown > 0)
			{
				point_cooldown -= time;
			}
		}
	}
	
	Game(Rendering* rendering_engine):road(&player_speed, rendering_engine), player(rendering_engine, &hostiles, &neutrals, &road)
	{
		this->rendering_engine = rendering_engine;
		screen = rendering_engine->GetScreen();
	}
};

#ifdef __cplusplus
extern "C"
#endif

//Function recieves events and manages the game accordingly
bool take_event(bool* freeze, Game** current_game, Rendering* rendering_engine)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_n:
			{
				delete *current_game;
				*current_game = new Game(rendering_engine);
				*freeze = false;
			} break;
			case SDLK_ESCAPE:
			{
				return true;
			} break;
			case SDLK_f:
			{
				(* current_game)->end("");
			} break;
			case SDLK_p:
			{
				if (*freeze)
				{
					*freeze = false;
				}
				else
				{
					*freeze = true;
				}
			} break;
			case SDLK_RIGHT:
			{
				(*current_game)->player.turn(1);
			} break;
			case SDLK_LEFT:
			{
				(*current_game)->player.turn(-1);
			} break;
			case SDLK_UP:
			{
				(*current_game)->change_speed(ACCELERATION);
			} break;
			case SDLK_DOWN:
			{
				(*current_game)->change_speed((-1) * ACCELERATION);
			} break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
			case SDLK_UP:
			{
				(*current_game)->reset_speed();
			} break;
			case SDLK_DOWN:
			{
				(*current_game)->reset_speed();
			} break;
			}
			break;
		case SDL_QUIT:
			return true;
			break;
		};
	};
	return false;
}

int main(int argc, char **argv) 
{
	srand(time(NULL));
	int t1, t2;
	Rendering rendering_engine = Rendering();
	SDL_Surface* screen = rendering_engine.GetScreen();
	Game* current_game = new Game(&rendering_engine);
	t1 = SDL_GetTicks();
	bool freeze = false;
	while(true) 
	{
		t2 = SDL_GetTicks(); //tics since start
		double delta = (t2 - t1) * 0.001; //tics since last frame * 0,001 - time
		t1 = t2;
		//If the game is not paused (freezed) game logic is refreshed and objects are rendered
		if (!freeze)
		{
			current_game->refresh(delta);
			rendering_engine.Refresh();
		}
		if (current_game->finished)
		{
			freeze = true;
		}
		if (take_event(&freeze, &current_game, &rendering_engine))
		{
			break;
		}
	};
	delete current_game;
	rendering_engine.Clear(); 	//freeing all surfaces
	return 0;
};
