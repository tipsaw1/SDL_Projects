# include <SDL.h>
#include <string>
#include <iostream>

void SetColor(int rgb[3], int alpha = 255);
// screen setup
const int SCREEN_W = 1280, SCREEN_H = 720; //screen size
SDL_Window* window = nullptr; //variable window stores the name of no box 
SDL_Renderer* renderer = nullptr; // variable renderer stores the name of no box
SDL_Event event;
bool running = true; // running variable
double gravity = 0.25, drag = 0.2;


// colors
int WHITE[3] = {255, 255, 255};
int BLACK[3] = { 0,0,0 };
int RED[3] = {255, 0, 0};
int ORANGE[3] = {255, 165, 0};
int YELLOW[3] = {255, 255, 0};
int GREEN[3] = {0, 255, 0};
int BLUE[3] = {0, 0, 255};
int PURPLE[3] = {160, 32, 240};


class Player {
public:
	SDL_Rect rect;
	int color[3];
	double dx = 0, dy = 0, speed;
	bool onGround = false;

	Player(int x, int y, int width, int height, int Color[3], double Speed) {
		rect.x = x;
		rect.y = y;
		rect.w = width;
		rect.h = height;
		std::copy(Color, Color + 3, color);
		speed = Speed;
	}

	void draw() {
		SetColor(color);
		SDL_RenderFillRect(renderer, &rect);
	}

	void update() {
		rect.x += round(dx);
		rect.y += round(dy);

		dy += gravity;

		groundCollision();

		if (dx > 0) {
			dx -= drag;
		}
		else if (dx < 0) {
			dx += drag;
		}
		
	}
	
	void jump() {
		if (onGround){
			dy = -10;
		}
	}

	void move(double amount) {
		dx += amount;
		if (dx < -10) {
			dx = -10;
		}
		else if (dx > 10) {
			dx = 10;
		}
	}

	int getBottom() {
		return rect.y + rect.h;
	}

	int getRight() {
		return rect.x + rect.w;
	}

private:
	void groundCollision() {
		if (getBottom() >= SCREEN_H && gravity > 0) {
			rect.y = SCREEN_H - rect.h;
			dy = 0;
			onGround = true;
		}
		else if (rect.y <= 0 && gravity < 0) {
			rect.y = 0;
			dy = 0;
			onGround = true;
		}
		else { onGround = false; }
	}

};

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_VIDEO); // initialize sdl
	SDL_CreateWindowAndRenderer(SCREEN_W, SCREEN_H, 0, &window, &renderer); // creates window and renderer. window and renderer variables now point to the window/renderer boxes

	Player p1(50, 50, 50, 50, BLUE, 2);

	int keys[4];
	int count = 0;
	while (running)
	{
		// background color
		SetColor(BLACK);
		SDL_RenderClear(renderer);


		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) { running = false; }
			{//needs fixing (check if event is already in array)
				if (event.type == SDL_KEYDOWN) {
					keys[count] = event.key.keysym.sym;
					count++;
				}
				if (event.type == SDL_KEYUP) {
					std::fill(keys, keys + 4, 0);
					count = 0;
				}
			}
		}
		
		for (int i = 0; i < 3; i++) {
			switch (keys[i]) {
			case SDLK_SPACE:
				p1.jump();
				break;
			case SDLK_RIGHT:
				p1.move(p1.speed);
				break;
			case SDLK_LEFT:
				p1.move(-p1.speed);
				break;
			case SDLK_q:
				gravity *= -1;
				std::cout << gravity << std::endl;
				break;

			}
		}

		p1.draw();
		p1.update();

		SDL_RenderPresent(renderer); // draw all previous renders onto the screen
		SDL_Delay(1000 / 60); //60 fps
	}

	return 0;
}

void SetColor(int rgb[3], int alpha) 
{
	SDL_SetRenderDrawColor(renderer, rgb[0], rgb[1], rgb[2], alpha);
}
