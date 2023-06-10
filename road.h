#pragma once
//The class is managing the scroll-down road by bulding the terrain out of texture blocks and moving them down
//When the lowest blocks go under the bottom of the screen, they get ,,teleported" on top of the stack.
class Road
{
private:
	int* player_speed = nullptr;
	Rendering* rendering_engine;
	int segment_number_per_column;

	class RoadSegment
	{
	private:
		int height;
		int width;
		//position stored in the RoadSegment class is relative to the middle of the segment
		Pos position;
		Rendering* rendering_engine;
		SDL_Surface* texture;

	public:
		bool handle_collisions(Pos point)
		{
			int left_bound = position.horizontal - width / 2;
			int right_bound = position.horizontal + width / 2;
			if (point.horizontal > left_bound && point.horizontal < right_bound)
			{
				return true;
			}
			return false;
		}

		void refresh(int player_speed, int segment_number, double time)
		{
			position.vertical += player_speed * time;
			if (position.vertical - height / 2 > SCREEN_HEIGHT) //Checks if segment is fully under the bottom of the screen and if so, it gets moved back to the top
			{
				position.vertical -= segment_number * height; //segment_number is the total height of the segment column
			}
			rendering_engine->DrawSurface(texture, position.horizontal, (int)position.vertical);
		}

		RoadSegment(int starting_horizontal_position, int starting_vertical_position, Rendering* rendering_engine) //Class constructor gets the vertical position relative to the 
		{
			texture = rendering_engine->LoadTexture(ROAD_TEXTURE);
			height = texture->h;
			width = texture->w;
			position.horizontal = starting_horizontal_position + width / 2;
			position.vertical = starting_vertical_position + height / 2;
			this->rendering_engine = rendering_engine;
		}
	};

	Vector<RoadSegment>segments;

public:

	void refresh(double time)
	{
		for (int i = 0; i < segments.size; i++)
		{
			segments.get_ptr()[i].refresh(*player_speed, segment_number_per_column, time);
		}
	}

	bool handle_collsions(Pos point)
	{
		for (int i = 0; i < segments.size; i++)
		{
			if (segments.get_ptr()[i].handle_collisions(point))
			{
				return true;
			}
		}
		return false;
	}

	Road(int* player_speed, Rendering* rendering_engine)
	{
		SDL_Surface* sample_texture = rendering_engine->LoadTexture(ROAD_TEXTURE);
		int height = sample_texture->h;
		int width = sample_texture->w;
		this->player_speed = player_speed;
		this->rendering_engine = rendering_engine;
		segment_number_per_column = ceil(SCREEN_HEIGHT / (float)height) + 1; //Height of the segment stack is calculated (as many segments as needed to fill the entire height of the screen, plus one more to cover the scroll-down effect)
		int segments_per_side = ceil((SCREEN_WIDTH - ROAD_WIDTH) / (width * 2.00)); //Based on the screen width and road with the number of columns needed on each side of the road is calculated, so thet there is no empty space on the sides.
		for (int i = 1; i <= segment_number_per_column; i++)
		{
			for (int j = 0; j < segments_per_side; j++) //Each iteration of the loop generates 'j' and '2*segment_number_per_side - j' segments out of '2*segment_number_per_side' segments in the row
			{
				RoadSegment left = RoadSegment((SCREEN_WIDTH - ROAD_WIDTH) / 2 - (j + 1) * width, SCREEN_HEIGHT - i * height, rendering_engine);
				RoadSegment right = RoadSegment((SCREEN_WIDTH + ROAD_WIDTH) / 2 + j * width, SCREEN_HEIGHT - i * height, rendering_engine);
				segments.push_back(left);
				segments.push_back(right);
			}
		}
	}
};