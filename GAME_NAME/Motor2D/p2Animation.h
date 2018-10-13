#ifndef __P2ANIMATION_H__
#define __P2ANIMATION_H__	// @Carles

#include "p2Defs.h"
#include "SDL\include\SDL_rect.h"

#define MAX_FRAMES 50

class Animation
{
public:
	bool loop = true;
	float speed = 1.0f;
	SDL_Rect frames[MAX_FRAMES];

private:
	float current_frame;
	int last_frame = 0;
	int loops = 0;

public:

	void PushBack(const SDL_Rect& rect)
	{
		frames[last_frame++] = rect;
	}

	SDL_Rect& GetCurrentFrame()
	{
		current_frame += speed;
		if (current_frame >= last_frame)
		{
			current_frame = (loop) ? 0.0f : last_frame - 1;
			loops++;
		}

		if (current_frame >= MAX_FRAMES)
		{
			Reset();
		}

		return frames[(int)current_frame];
	}

	bool Finished() const
	{
		return loops > 0;
	}

	void Reset()
	{
		current_frame = 0;
	}

	void ResetLoops()
	{
		loops = 0;
	}
	// @Carles: Allocates an animation from a row of frames by knowing the position of the first frame and other important data
	void AllocAnimation(iPoint spritePos, iPoint spriteSize, float speed, uint frames, bool loop) {

		int xCounter = 0;

		for (uint i = frames; i > 0; i--) {
			PushBack({ spritePos.x + spriteSize.x * xCounter++, spritePos.y, spriteSize.x, spriteSize.y });
		}
		
		this->speed = speed;
		this->loop = loop;
	}
};

#endif	__P2ANIMATION_H__