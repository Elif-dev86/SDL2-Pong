#include <iostream>
#include <string>
#include <time.h>
#include <cmath>
#include <algorithm>

#include <SDL.h>
#include <SDL_main.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <direct.h> // on Windows for changing directories

using namespace std;

#define EMPTY_LINE cout << endl
#define OUT cout << 
#define IN cin >> 
#define MAX_BOUNCE_ANGLE (M_PI / 3.0f)

// Create a custom event type
enum GameEventType
{
	GAME_EVENT_BALL_RESET = SDL_USEREVENT + 1
};

// Create a struct for our custom event
struct GameEvent
{
	SDL_Event sdlEvent; // SDL Base event
	int ballDirectionX; // Example: Direction of the ball
	int ballDirectionY; // Example: Direction of the ball
};

struct Ball
{
	float x, y; 	// position
	float vx, vy; 	// Velocity
	float size; 	// width/ height
	float speed;	
};

struct Paddle
{
	float x, y;
	float vx, vy;
	float h, w;
	float speed;
};

struct NetSquare
{
	float x, y;
	float h, w;
};

SDL_Window* window;
SDL_Renderer* renderer;
TTF_Font* font;
Mix_Music* music;
Mix_Chunk* paddleBlip;
Mix_Chunk* wallBlip;
Mix_Chunk* scoreBlip;

Ball ball;

Paddle paddle1;
Paddle paddle2;

bool isRunning;
bool waitToStart;

int score1 = 0;
int score2 = 0;

int left_a, left_b;
int right_a, right_b;
int	top_a, top_b;
int bottom_a, bottom_b;

float fontText;

string scoreText1;
string scoreText2;
string gameStartText;

bool Init();
void HandleInput();
void Update();
void Render();
void Quit();

void BallUpdate();
void Paddle1Update();
void Paddle2Update();
void CreateBallResetEvent(int dirX, int dirY);
void RenderText(string text, SDL_Rect dest);
void RenderMiddleNet();

float Clamp(float value, float min, float max);

void CheckBallCollision(Paddle paddle);

int main (int argc, char** argvs)
{
	if (!Init()) return 1;
	
	isRunning = true;
	waitToStart = true;
	const int frameDelay = 1000 / 60;
	Uint32 totalFrameTicks = 0;
	Uint32 totalFrames = 0;
	
	while (isRunning)
	{
		totalFrames++;
		Uint32 startTicks = SDL_GetTicks();
		
		HandleInput();
		Update();
		Render();
		
		SDL_Delay(frameDelay);
		
		Uint32 endTicks = SDL_GetTicks();
		totalFrameTicks += endTicks - startTicks;
		
		OUT "\rAverage FPS: " << 1000.0f / ((float)totalFrameTicks / totalFrames) << "		" << flush;
		
	}
	
	Quit();
	
	return 0;
}

void HandleInput()
{
	
	SDL_Event event;
		
	while( SDL_PollEvent(&event) )
	{
		
		switch(event.type)
		{
			
			case SDL_QUIT:
			
				isRunning = false;
				
				break;
				
			case SDL_KEYDOWN:
			{
				SDL_Keycode keyPressed = event.key.keysym.sym;
			
				if (keyPressed == SDLK_ESCAPE)
				{
					isRunning = false;
				}
				
				if (keyPressed == SDLK_SPACE)
				{
					waitToStart = false;
				}

			}
				
			// Handle Custom event type (ball reset)
			case SDL_USEREVENT:
				{
					if (event.user.code == GAME_EVENT_BALL_RESET)
					{
						GameEvent* gameEvent = (GameEvent*)event.user.data1;
						//OUT "Ball reset! Direction: X=" << gameEvent->ballDirectionX << ", Y=" << gameEvent->ballDirectionY << endl;
						// Reset the ball position  and direction
						
						ball.x = 400.0f - ball.size;
						ball.y = 300.0f - ball.size;
						
						ball.vx = 4;
						ball.vy = 4;
						
						int val = (rand() % 2) ? 1 : -1;
						
						ball.vx *= val;
						ball.vy *= val;
						
						// (just a place holder for your actual pong logic)
						delete gameEvent; // Don't forget to clean up
					}
				}
				break;
				
			default:
				break;
		}
	}
}

void Update()
{
	if (!waitToStart)
	{
		SDL_PumpEvents();
		
		const Uint8* keystate = SDL_GetKeyboardState(NULL);
		
		if (keystate[SDL_SCANCODE_UP])
		{
			paddle1.y -= 6.0f;
		}
		
		if (keystate[SDL_SCANCODE_DOWN])
		{
			paddle1.y += 6.0f;
		}
		
		Paddle1Update();
		Paddle2Update();
		BallUpdate();
		
		CheckBallCollision(paddle1);
		CheckBallCollision(paddle2);
		
		if (score1 == 5 || score2 == 5)
		{
			SDL_Delay(2000);
			
			score1 = 0;
			score2 = 0;
			
			scoreText1 = "0";
			scoreText2 = "0";
		}
	}
	
}

void Render()
{
	SDL_SetRenderDrawColor(renderer, 16, 31, 2, 255);
	SDL_RenderClear(renderer);
	
	TTF_SetFontSize(font, 70);
	SDL_Rect dest1 = {420, 10, 0, 0};
	RenderText(scoreText1, dest1);
	
	SDL_Rect dest2 = {330, 10, 0, 0};
	RenderText(scoreText2, dest2);
	
	if (waitToStart)
	{	
		TTF_SetFontSize(font, 40);
		SDL_Rect startrect = {100, 260, 0};
		RenderText(gameStartText, startrect);
	}
	
	SDL_Rect ballRect;
	ballRect.x = static_cast<int>(ball.x);
	ballRect.y = static_cast<int>(ball.y);
	ballRect.h = static_cast<int>(ball.size);
	ballRect.w = static_cast<int>(ball.size);
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &ballRect);
	
	SDL_Rect paddle1Rect;
	paddle1Rect.x = static_cast<int>(paddle1.x);
	paddle1Rect.y = static_cast<int>(paddle1.y);
	paddle1Rect.h = static_cast<int>(paddle1.h);
	paddle1Rect.w = static_cast<int>(paddle1.w);
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &paddle1Rect);
	
	SDL_Rect paddle2Rect;
	paddle2Rect.x = static_cast<int>(paddle2.x);
	paddle2Rect.y = static_cast<int>(paddle2.y);
	paddle2Rect.h = static_cast<int>(paddle2.h);
	paddle2Rect.w = static_cast<int>(paddle2.w);
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &paddle2Rect);
	
	RenderMiddleNet();
	
	// Update the renderer
	SDL_RenderPresent(renderer);
}

void BallUpdate()
{
	ball.x += ball.vx;
	ball.y += ball.vy;
	
	if (ball.y <= 0)
	{
		ball.vy *= -1;
		Mix_PlayChannel( -1, wallBlip, 0);
	}
	
	if (ball.y > 600 - ball.size)
	{
		ball.vy *= -1;
		Mix_PlayChannel( -1, wallBlip, 0);
	}
	
	if (ball.x <= 0 - ball.size)
	{
		CreateBallResetEvent(static_cast<int>(ball.x), static_cast<int>(ball.y));
		score1++;
		scoreText1 = to_string(score1);
		Mix_PlayChannel(-1, scoreBlip, 0);
	}
	
	if (ball.x > 800 + ball.size)
	{
		CreateBallResetEvent(static_cast<int>(ball.x), static_cast<int>(ball.y));
		score2++;
		scoreText2 = to_string(score2);
		Mix_PlayChannel(-1, scoreBlip, 0);
	}
}

void CheckBallCollision(Paddle paddle)
{
	// Calculate centers
	float ballCenterX = ball.x + ball.size / 2.0f;
	float ballCenterY = ball.y + ball.size / 2.0f;
	float paddleCenterX = paddle.x + paddle.w / 2.0f;
	float paddleCenterY = paddle.y + paddle.h / 2.0f;
	
	// Offset distance from center
	float distanceFromCenter = ballCenterY - paddleCenterY;
	
	// How far apart centers are horizontally and vertically. (Distance Between Centers)
	float dx = ballCenterX - paddleCenterX;
	float dy = ballCenterY - paddleCenterY;
	
	// Define how far apart the two centers can be before the objects touch.
	float combinedHalfWidth = (ball.size / 2.0f) + (paddle.w / 2.0f);
	float combinedHalfHeight = (ball.size / 2.0f) + (paddle.h / 2.0f);
	
	// Subtract the actual center gap from the allowed max. (Overlap amunt)
	float overlapX = combinedHalfWidth - fabs(dx);
	float overlapY = combinedHalfHeight - fabs(dy);
	
	// Normalize (-1 to 1)
	float normalized = distanceFromCenter / (paddle.h / 2.0f);
	
	normalized = Clamp(normalized, -1.0f, 1.0f);
	
	// set new angle
	float speed = sqrt(ball.vx * ball.vx + ball.vy * ball.vy); // preserve speed
	float angle = normalized * MAX_BOUNCE_ANGLE; // radians, like 60 max
	
	if (overlapX > 0 && overlapY > 0)
	{
		// Collision is happening
		if (overlapX < overlapY)
		{
			// Hit from the left or right
			if (dx > 0)
			{
				// Ball hit paddle from the right
				ball.vx = speed * cos(angle);
				
				if (angle > 0)
				{
					if (angle > 0 && ball.vy > 0)
					{
						ball.vy = speed * sin(angle);
					}
					else
					{
						ball.vy -= speed * sin(angle);
					}
				}
				else if (angle < 0)
				{
					if (angle < 0 && ball.vy > 0)
					{
						ball.vy -= speed * sin(angle);
					}
					else if (angle < 0 && ball.vy < 0)
					{
						ball.vy = speed * sin(angle);
					}
					else
					{
						ball.vy -= speed * sin(angle);
					}
				}
				
				ball.x += overlapX;
				ball.vx = fabs(ball.vx); // move right
				ball.vx += 0.2f;
				Mix_PlayChannel( -1, paddleBlip, 0);
			}
			else
			{
				// Ball hit paddle from the left
				ball.vx = speed * cos(angle);
				
				if (angle > 0)
				{
					if (angle > 0 && ball.vy > 0)
					{
						ball.vy = speed * sin(angle);
					}
					else
					{
						ball.vy -= speed * sin(angle);
					}
				}
				else if (angle < 0)
				{
					if (angle < 0 && ball.vy > 0)
					{
						ball.vy -= speed * sin(angle);
					}
					else if (angle < 0 && ball.vy < 0)
					{
						ball.vy = speed * sin(angle);
					}
					else
					{
						ball.vy -= speed * sin(angle);
					}
				}
				
				ball.x -= overlapX;
				ball.vx = -fabs(ball.vx); // move left
				ball.vx -= 0.2f;
				Mix_PlayChannel( -1, paddleBlip, 0);
			}
		}
		else
		{
			// Hit from the top or bottom
			if (dy > 0)
			{
				// Ball hit from bottom
				ball.y += overlapY;
				ball.vy = fabs(ball.vy);
			}
			else
			{
				// Ball hit from top
				ball.y -= overlapY;
				ball.vy = -fabs(ball.vy); // move up
			}
		}
	}
}

float Clamp(float value, float min, float max) 
{
    return (value < min) ? min : (value > max) ? max : value;
}

void Paddle1Update()
{
	// Paddle 1 boundry check
	if (paddle1.y <= 0)
	{
		paddle1.y = 0;
	}
	else if (paddle1.y > 600 - paddle1.h)
	{
		paddle1.y = 600 - paddle1.h;
	}
}

void Paddle2Update()
{
	
	// Track ball ball position
	// move paddle accordingly
	// paddle moves where the ball is with its own speed
	
	float ballPosition = ball.y + ball.size / 2.0f;
	float paddlePosition = paddle2.y + paddle2.h / 2.0f;
	
	if (ball.vx < 1)
	{
		if (ballPosition < paddlePosition)
		{
			paddle2.y -= paddle2.vy;
		}
		if (ballPosition > paddlePosition)
		{
			paddle2.y += paddle2.vy;
		}
	}
	
	// Paddle 2 boundary check
	if (paddle2.y <= 0)
	{
		paddle2.y = 0;
	}
	else if (paddle2.y > 600 - paddle2.h)
	{
		paddle2.y = 600 - paddle2.h;
	}
}

void RenderMiddleNet()
{
	NetSquare netSquare[32];
	SDL_Rect netSquareRect[10];
	int squareLastPosition = 0;
	
	for (int i = 0; i < 32; i++)
	{
		// Initialize NetSquare
		netSquare[i].h = 18.0f;
		netSquare[i].w = 18.0f;
		netSquare[i].x = 400.0f - netSquare[i].w / 2;
		netSquare[i].y = 10.0f - netSquare[i].h;
		
		if (i > 1)
		{
			netSquare[i].y = squareLastPosition;
			netSquare[i].y += 40.0f;
		}
	
		netSquareRect[i].x = static_cast<int>(netSquare[i].x);
		netSquareRect[i].y = static_cast<int>(netSquare[i].y);
		netSquareRect[i].h = static_cast<int>(netSquare[i].h);
		netSquareRect[i].w = static_cast<int>(netSquare[i].w);
		
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(renderer, &netSquareRect[i]);
		
		squareLastPosition = netSquare[i].y;
	}
	
}

void RenderText(string text, SDL_Rect dest)
{
	if (!font) return;
	
	SDL_Color fg = {255, 255, 255};
	SDL_Surface* surf = TTF_RenderText_Solid(font, text.c_str(), fg);
	
	if (!surf) return;
	
	dest.w = surf->w;
	dest.h = surf->h;
	
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
	
	if (!tex) return;
	
	SDL_RenderCopy(renderer, tex, NULL, &dest);
	SDL_DestroyTexture(tex);
	SDL_FreeSurface(surf);
}

bool Init()
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		OUT "SDL_Init Error: " << SDL_GetError() << endl;
		system("pause");
		return false;
	}
	
	// Initialize TTF
	if (TTF_Init() < 0)
	{
		OUT "TTF_Init Error: " << SDL_GetError() << endl;
		system("pause");
		return false;
	}
	
	srand(time(0));
	
	// Create a window
	window = SDL_CreateWindow("Pong Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
	
	if (!window)
	{
		OUT "SDL_CreateWindow Error: " << SDL_GetError() << endl;
		system("pause");
		return false;
	}
	
	// create renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	if (!renderer)
	{
		OUT "SDL_CreateRenderer Error: " << SDL_GetError() << endl;
		system("pause");
		return false;
	}
	
	_chdir("C:/game-dev-journey/engine_dev/sdl_pong");
	
	// Initialize SDL Mixer with audio format
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) 
	{
		OUT "Mix_OpenAudio Error: " << Mix_GetError() << endl;
		system("pause");
		return false;
	}
	
	// Load Font
	font = TTF_OpenFont("data/PixelFont.ttf", 70);
	
	if (!font)
	{
		OUT "TTF_OpenFont Error: " << SDL_GetError() << endl;
		return false;
	}
	
	// Load sound
	paddleBlip = Mix_LoadWAV("data/BlipPaddle.wav");
	wallBlip = Mix_LoadWAV("data/WallHit.wav");
	scoreBlip = Mix_LoadWAV("data/ScoreSound.wav");
	if (!paddleBlip && !wallBlip && scoreBlip)
	{
		OUT "Mix_LoadWAV Error: " << Mix_GetError() << endl;
		system("pause");
		return false;
	}
	
	// Initialize sound volumes
	Mix_VolumeChunk(paddleBlip, 10);
	Mix_VolumeChunk(wallBlip, 10);
	Mix_VolumeChunk(scoreBlip, 10);
	
	// Intitialize text variables
	scoreText1 = to_string(score1);
	scoreText2 = to_string(score2);
	gameStartText = "Press Space to Start";
	
	// Initialize Ball
	ball.size = 20.0f;
	ball.x = 400.0f - ball.size;
	ball.y = 300.0f - ball.size;
	ball.speed = 300.0f;
	ball.vx = 5.0f;
	ball.vy = 5.0f;
	
	// Initialize Paddle 2
	paddle1.h = 200;
	paddle1.w = 30;
	paddle1.x = 800.0f - paddle1.w;
	paddle1.y = 300.0f - paddle1.h / 2;
	paddle1.vx = 0;
	paddle1.vy = 0;
		
	// Initialize Paddle 2
	paddle2.h = 200;
	paddle2.w = 30;
	paddle2.x = 0.0f;
	paddle2.y = 300.0f - paddle2.h / 2;
	paddle2.vx = 0;
	paddle2.vy = 6;
	
	return true;
}

void CreateBallResetEvent(int dirX, int dirY)
{
	GameEvent* customEvent = new GameEvent;
	customEvent->ballDirectionX = dirX;
	customEvent->ballDirectionY = dirY;
	
	SDL_Event event;
	event.type = SDL_USEREVENT;
	event.user.code = GAME_EVENT_BALL_RESET;
	event.user.data1 = customEvent;
	SDL_PushEvent(&event); // Push the event to the queue
}

void Quit()
{
	TTF_CloseFont( font );
	Mix_FreeChunk(paddleBlip);
	Mix_FreeChunk(wallBlip);
	Mix_FreeChunk(scoreBlip);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	Mix_Quit();
	SDL_Quit();
}