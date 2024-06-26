# include <SDL.h>
#include <string>
#include <iostream>

class Player;
// function to easily set the color using an rgb preset
void SetColor(int rgb[3], int alpha = 255);

// Draw a rectangle with an offset
int GetOffset(Player* player);

// screen setup
const int SCREEN_W = 1280, SCREEN_H = 720; //screen size
SDL_Window* window = nullptr; //variable window stores the name of no box 
SDL_Renderer* renderer = nullptr; // variable renderer stores the name of no box
SDL_Event event; // user input
bool running = true; // true if the game is running
double gravity = 0.5, drag = 0.25; // strength of gravity and drag 
int movement_offset = 0;

// color presets
int WHITE[3] = {255, 255, 255};
int BLACK[3] = { 0,0,0 };
int RED[3] = {255, 0, 0};
int ORANGE[3] = {255, 165, 0};
int YELLOW[3] = {255, 255, 0};
int GREEN[3] = {0, 255, 0};
int BLUE[3] = {0, 0, 255};
int PURPLE[3] = {160, 32, 240};

// stores key input
struct {
	bool moving_left = false;
	bool moving_right = false;
	bool jumping = false;
	bool attacking = false;
} Actions;

struct {
	int move_right = SDLK_RIGHT;
	int move_left = SDLK_LEFT;
	int jump = SDLK_z;
	int attack = SDLK_x;
} PlayerKeys;

class Player {
public:
	// rectangle representing the player
	SDL_Rect rect;
	SDL_Rect offset_rect;
	SDL_Rect offset_attack_box;
	SDL_Rect attack_box;
	
	// color of the rectangle
	int color[3];

	// velocity, acceleration, and max speed
	double dx = 0, dy = 0, acceleration, max_speed = 10;

	// true if the player is on the ground
	bool onGround = false;

	// attacking
	bool attacking = false;
	int attack_box_offset = 0;

	Player(int x, int y, int width, int height, int Color[3], double Acelleration) {
		rect.x = x;
		rect.y =  y;
		rect.w = offset_rect.w = width;
		rect.h = offset_rect.h = height;
		std::copy(Color, Color + 3, color);
		acceleration = Acelleration;

		// attack box
		attack_box.w = offset_attack_box.w = rect.w;
		attack_box.h = offset_attack_box.h = rect.h/5;
		attack_box.x = getRight();
		attack_box.y = rect.y;
	}

	//draw player
	void draw() {
		offset_rect.x = rect.x - GetOffset(this);
		offset_rect.y = rect.y;
		SetColor(color);
		SDL_RenderFillRect(renderer, &offset_rect);

		offset_attack_box.x = attack_box.x - GetOffset(this);
		offset_attack_box.y = attack_box.y;
		SetColor(GREEN);
		SDL_RenderFillRect(renderer, &offset_attack_box);
	}
	void attack() {
		attack_box.x = getRight();
		attack_box.y = rect.y + attack_box_offset;
		if (attacking) {
			if (attack_box.y < getBottom()) {
				attack_box_offset += 2;
			}
			else {
				attacking = false;
				attack_box_offset = 0;
			}
		}
		
	}

	//updates done every frame
	void update() {
		// move
		check_keys();
		move();

		// apply gravity
		dy += gravity;

		// collisions
		check_ground_collision();
		
		// attack
		attack();
	}
	
	void jump() {
		if (onGround){
			dy = -15;
		}
	}

	
	//returns bottom of rect
	int getBottom() {
		return rect.y + rect.h;
	}

	// returns right side of rect
	int getRight() {
		return rect.x + rect.w;
	}

private:
	// checks for collision with ground
	void check_ground_collision() {
		// collision for the floor
		if (getBottom() >= SCREEN_H && gravity > 0) {
			rect.y = SCREEN_H - rect.h;
			dy = 0;
			onGround = true;
		}

		// collision for the roof
		else if (rect.y <= 0 && gravity < 0) {
			rect.y = 0;
			dy = 0;
			onGround = true;
		}

		// no collision
		else { onGround = false; }
	}

	// checks for movement input
	void check_keys() {
		if (Actions.moving_right && dx < max_speed) {
			dx += acceleration;
		}
		if (Actions.moving_left && dx > -max_speed) {
			dx -= acceleration;
		}
		if (Actions.jumping) {
			jump();
		}

		if (Actions.attacking) {
			attacking = true;
		}
	}

	// applys movement
	void move() {
		// move rect
		rect.x += round(dx);
		rect.y += round(dy);
		movement_offset += round(dx);
		
		if (movement_offset > 100) { movement_offset = 100; }
		else if (movement_offset < -100) { movement_offset = -100; }

		// apply drag
		if (dx > 0) {
			dx -= drag;
		}
		else if (dx < 0) {
			dx += drag;
		}
	}
};

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_VIDEO); // initialize sdl
	SDL_CreateWindowAndRenderer(SCREEN_W, SCREEN_H, 0, &window, &renderer); // creates window and renderer. window and renderer variables now point to the window/renderer boxes

	// Player 1 instance
	Player p1(50, 50, 50, 50, BLUE, 1);
	Player* ptr = &p1;
	std::cout << ptr << std::endl;
	// test object
	SDL_Rect test_rect{ 200, SCREEN_H - 50, 50, 50 };

	while (running)
	{
		// background color
		SetColor(BLACK);
		SDL_RenderClear(renderer);


		while (SDL_PollEvent(&event)) // collects events
		{
			switch (event.type) // event types
			{
			// ends game
			case SDL_QUIT:
				running = false;
				break;

			case SDL_KEYDOWN:
				if (event.key.keysym.sym == PlayerKeys.move_right) {
					Actions.moving_right = true;
				}
				if (event.key.keysym.sym == PlayerKeys.move_left) {
					Actions.moving_left = true;
				}
				if (event.key.keysym.sym == PlayerKeys.jump) {
					Actions.jumping = true;
				}
				if (event.key.keysym.sym == PlayerKeys.attack) {
					Actions.attacking = true;
				}
				break;

			case SDL_KEYUP:
				if (event.key.keysym.sym == PlayerKeys.move_right) {
					Actions.moving_right = false;
				}
				if (event.key.keysym.sym == PlayerKeys.move_left) {
					Actions.moving_left = false;
				}
				if (event.key.keysym.sym == PlayerKeys.jump) {
					Actions.jumping = false;
				}
				if (event.key.keysym.sym == PlayerKeys.attack) {
					Actions.attacking = false;
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				
				p1.dx = 20;

			}
			
		}

		p1.update();
		p1.draw();
		test_rect.x = 200 - GetOffset(&p1);
		SetColor(RED);
		SDL_RenderFillRect(renderer, &test_rect);

		SDL_RenderPresent(renderer); // draw all previous renders onto the screen
		SDL_Delay(1000 / 60); //60 fps
	}

	return 0;
}

void SetColor(int rgb[3], int alpha) 
{ 
	SDL_SetRenderDrawColor(renderer, rgb[0], rgb[1], rgb[2], alpha);
}


int GetOffset(Player* player) {
	return player->rect.x - SCREEN_W / 2 - movement_offset;
}