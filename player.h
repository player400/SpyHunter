#pragma once

class Player : public Car
{
public:

	//Function resetting the player's car after death
	void restart()
	{
		position.horizontal = SCREEN_WIDTH / 2;
		position.vertical = SCREEN_HEIGHT / 2;
		hp = PLAYER_HP;
	}

	Player(Rendering* rendering_engine, Vector<Hostile>*hostiles, Vector<Neutral>* neutrals, Road* road)
	{
		restart();
		this->rendering_engine = rendering_engine;
		texture = rendering_engine->LoadTexture(HUNTER_TEXTURE);
		height = texture->h;
		width = texture->w;
	}
};