#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include "object.h"

/*GLOBALS*/
const int HEIGHT = 640;
const int WIDTH = 480;
const int NUM_BULLETS = 20;
const int NUM_STONES = 10;
const int NUM_EXPLOSIONS = 5;

enum KEYS{UP, DOWN, LEFT, RIGHT, SPACE};
bool keys[5] = {false, false, false, false , false};
enum STATE{MENU, PLAYING};

/*PROTOTYPES*/
void initShip(SpaceShip *ship,ALLEGRO_BITMAP *image);
void drawShip(SpaceShip *ship,ALLEGRO_BITMAP *image);
void upShip(SpaceShip *ship);
void downShip(SpaceShip *ship);
void leftShip(SpaceShip *ship);
void rightShip(SpaceShip *ship);

void InitBullet(Bullet bullet[], int size);
void DrawBullet(Bullet bullet[], int size);
void FireBullet(Bullet bullet[], int size, SpaceShip *ship);
void UpdateBullet(Bullet bullet[], int size);
void collideBullet(Bullet bullet[],int bSize,Stone stone[],int cSize,Explosion explosions[],int eSize,SpaceShip *ship);

void initStone(Stone stone[],int size,ALLEGRO_BITMAP *stone_image);
void drawStone(Stone stone[],int size,ALLEGRO_BITMAP *stone_image);
void startStone(Stone stone[],int size);
void updateStone(Stone stone[],int size);
void collideStone(Stone stone[],int sSize,SpaceShip *ship,Explosion explosions[],int eSize);

void initExplosions(Explosion explosions[], int size, ALLEGRO_BITMAP *image);
void drawExplosions(Explosion explosions[], int size);
void startExplosions(Explosion explosions[], int size, int x, int y);
void updateExplosions(Explosion explosions[], int size);


void initLaser(Laser *laser,SpaceShip *ship);
void drawLaser(Laser *laser);
void updateLaser(Laser *laser,SpaceShip *ship);
void collideLaser(Laser *laser,Stone stone[],int size,SpaceShip *ship,Explosion explosions[],int eSize);



int main(void)
{
	/*pirimitive variable*/
	bool done = false;
	const int FPS = 60;
	bool redraw = true;
	bool GameOver = false;
	bool laserFlag = false;
	int bulletCurDelay = 0;
	int bulletDelay = 7;
	int state = MENU;
	Laser laser;
	/*object variables*/
	SpaceShip ship;
	Bullet bullets[NUM_BULLETS];
	Stone stones[NUM_STONES];
	Explosion explosions[NUM_EXPLOSIONS];

	

	/*Allegro variable*/
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer;
	ALLEGRO_BITMAP *image;
	ALLEGRO_BITMAP *stone_image;
	ALLEGRO_FONT *font18 = NULL;
	ALLEGRO_BITMAP *expImage;
	/*init functions*/
	if(!al_init())
		return -1;

	display = al_create_display(WIDTH,HEIGHT);

	if(!display)
		return -1;

	al_init_primitives_addon();
	al_install_keyboard();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();

	image = al_load_bitmap("sheet.jpeg");
	al_convert_mask_to_alpha(image, al_map_rgb(255, 255, 255));
	stone_image = al_load_bitmap("astreoid.png");
	al_convert_mask_to_alpha(stone_image, al_map_rgb(0, 0, 0));

	expImage = al_load_bitmap("exp.png");

	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / FPS);


	font18 = al_load_font("arial.ttf", 18, 0);

	srand(time(NULL));
	initShip(&ship,image);
	InitBullet(bullets,NUM_BULLETS);
	initStone(stones,NUM_STONES,stone_image);
	initExplosions(explosions, NUM_EXPLOSIONS, expImage);
	initLaser(&laser,&ship);



	al_register_event_source(event_queue,al_get_display_event_source(display));
	al_register_event_source(event_queue,al_get_keyboard_event_source());
	al_register_event_source(event_queue,al_get_timer_event_source(timer));
	al_start_timer(timer);
	while(!done)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue,&ev);
		if(ev.type == ALLEGRO_EVENT_TIMER)
		{
			redraw = true;

			if(state == MENU)
			{
				if(keys[SPACE])
				{
					state = PLAYING;
				}
			}

			else if(state == PLAYING)
			{
				if(keys[UP])
				{
					upShip(&ship);
				}
				if(keys[DOWN])
				{
					downShip(&ship);
				}
				if(keys[LEFT])
				{
					leftShip(&ship);
				}
				if(keys[RIGHT])
				{
					rightShip(&ship);
				}
			
					if(!GameOver)
					{	
						if(++bulletCurDelay >= bulletDelay)
						{	
							FireBullet(bullets, NUM_BULLETS,&ship);
							bulletCurDelay = 0;
						}	
						updateExplosions(explosions, NUM_EXPLOSIONS);
						UpdateBullet(bullets, NUM_BULLETS);
						collideStone(stones,NUM_STONES,&ship,explosions,NUM_EXPLOSIONS);
						startStone(stones,NUM_STONES);
						updateStone(stones,NUM_STONES);
						collideBullet(bullets,NUM_BULLETS,stones,NUM_STONES,explosions,NUM_EXPLOSIONS,&ship);
						if(laserFlag)
						{
							updateLaser(&laser,&ship);
							collideLaser(&laser,stones,NUM_STONES,&ship,explosions,NUM_EXPLOSIONS);
							if(rand() % 650 == 0)
							{
								laserFlag = false;
							}

						}
					if(ship.health <= 0)
					{
						GameOver = true;
					}
					}
			}


			
		}
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch(ev.keyboard.keycode)
			{
				case ALLEGRO_KEY_ESCAPE:
				done = true;
				break;

				case ALLEGRO_KEY_UP:
				keys[UP] = true;
				break;

				case ALLEGRO_KEY_DOWN:
				keys[DOWN] = true;
				break;

				case ALLEGRO_KEY_LEFT:
				keys[LEFT] = true;
				break;

				case ALLEGRO_KEY_RIGHT:
				keys[RIGHT] = true;
				break;

				case ALLEGRO_KEY_SPACE:
				keys[SPACE] = true;
				
				break;
		
			
			}
		}
		else if(ev.type == ALLEGRO_EVENT_KEY_UP)
		{
			switch(ev.keyboard.keycode)
			{
				case ALLEGRO_KEY_ESCAPE:
				done = true;
				break;

				case ALLEGRO_KEY_UP:
				keys[UP] = false;
				break;

				case ALLEGRO_KEY_DOWN:
				keys[DOWN] = false;
				break;

				case ALLEGRO_KEY_LEFT:
				keys[LEFT] = false;
				break;

				case ALLEGRO_KEY_RIGHT:
				keys[RIGHT] = false;
				break;

				case ALLEGRO_KEY_SPACE:
				keys[SPACE] = false;
				break;
	
			
			}
		}			
		

		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			done = true;
		}

		if(redraw && al_is_event_queue_empty(event_queue))
		{
			redraw = false;
			
			if(state == MENU)
			{
				al_draw_textf(font18, al_map_rgb(0, 255, 255), WIDTH / 2, HEIGHT / 2, ALLEGRO_ALIGN_CENTRE, "Press Space to play");
			}
			else if(state == PLAYING)
			{
			al_draw_textf(font18, al_map_rgb(0, 255, 255), 5, 5, 0, "Your Score: %d", ship.health);
				if(!(GameOver))
				{	
					drawShip(&ship,image);
					DrawBullet(bullets,NUM_BULLETS);
					drawStone(stones,NUM_STONES,stone_image);
					drawExplosions(explosions, NUM_EXPLOSIONS);
					if(rand() % 900 == 0)
					{
						laserFlag = true;
					}
					if(laserFlag)
					{
						drawLaser(&laser);
					}

				}
			}
			else if(GameOver)
			{
				al_draw_textf(font18, al_map_rgb(0, 255, 255), WIDTH / 2, HEIGHT / 2, ALLEGRO_ALIGN_CENTRE, "Game Over. Final Score: %i", ship.score);
			}
			al_flip_display();
			al_clear_to_color(al_map_rgb(255,255,255));
		}



	}
	al_destroy_font(font18);
	al_destroy_bitmap(image);
	al_destroy_bitmap(stone_image);
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_destroy_display(display);


	return 0;

}

void initShip(SpaceShip *ship,ALLEGRO_BITMAP *image)
{
	ship->x = WIDTH/2;
	ship->y = HEIGHT/2;
	ship->score = 0;
	ship->health = 3;
	ship->dead = false;

	ship->speed = 5;


	ship->frameWidth = 60;
	ship->frameHeight = 78;
	ship->image = image;
	ship->boundx = ship->frameWidth/2;
	ship->boundy = ship->frameHeight/2;
}

void drawShip(SpaceShip *ship,ALLEGRO_BITMAP *image)
{

	al_draw_bitmap_region(ship->image,0,0,ship->frameWidth,ship->frameHeight,ship->x - ship->frameWidth / 2, ship->y - ship->frameHeight / 2,0);
}

void upShip(SpaceShip *ship)
{
	
	ship->y -= ship->speed;

	if(ship->y < 0)
		ship->y = 0;
}
void downShip(SpaceShip *ship)
{

	ship->y += ship->speed;

	if(ship->y > HEIGHT)
		ship->y = HEIGHT;
}
void leftShip(SpaceShip *ship)
{
	
	ship->x -= ship->speed;

	if(ship->x < 0)
		ship->x = 0;
}
void rightShip(SpaceShip *ship)
{
	
	ship->x += ship->speed;

	if(ship->x > WIDTH)
		ship->x = WIDTH;
}

void InitBullet(Bullet bullet[], int size)
{
	int i;
	for (i = 0; i < size; i++)
	{
		bullet[i].speed = 12;
	}
}
void DrawBullet(Bullet bullet[], int size)
{
	int i;
	for(i = 0; i < size; i++)
	{
		if(bullet[i].live)
			al_draw_filled_circle(bullet[i].x+10, bullet[i].y, 4, al_map_rgb(0,0,0));
			al_draw_filled_circle(bullet[i].x-10, bullet[i].y, 4, al_map_rgb(0,0,0));

	}
}

void FireBullet(Bullet bullet[], int size, SpaceShip *ship)
{
	int i;
	for(i = 0; i < size; i++)
	{
		if(!bullet[i].live)
		{
			bullet[i].x = ship->x;
			bullet[i].y = ship->y - 17;
			bullet[i].live = true;
			break;
		}
	}
}
void UpdateBullet(Bullet bullet[], int size)
{
	int i;
	for(i = 0; i < size; i++)
	{
		if(bullet[i].live)
		{
			bullet[i].y -= bullet[i].speed;
			if(bullet[i].y < 0)
				bullet[i].live = false;
		}
	}
}

void collideBullet(Bullet bullet[],int bSize,Stone stone[],int cSize,Explosion explosions[],int eSize,SpaceShip *ship)
{
	int i,j;

	for (i = 0; i < bSize; i++)
	{
		if(bullet[i].live)
		{
			for (j = 0; j < cSize; j++)
			{
				if(stone[j].live)
				{
					if((bullet[i].x + 10 > stone[j].x && bullet[i].x + 10 < (stone[j].x + stone[j].boundx) && 
						bullet[i].y > stone[j].y && bullet[i].y < (stone[j].y + stone[j].boundy)) || 
						bullet[i].x - 10 > stone[j].x && bullet[i].x - 10 < (stone[j].x + stone[j].boundx) && 
						bullet[i].y > stone[j].y && bullet[i].y < (stone[j].y + stone[j].boundy))
					{
						bullet[i].live = false;
						stone[j].health--;
						if(stone[j].health <= 0)
						{
							stone[j].live = false;
							ship->score++;
							startExplosions(explosions, NUM_EXPLOSIONS, stone[j].x + stone[j].boundx, stone[j].y + stone[j].boundy);
						}
					}
				}
			}
		}
	}
}

void initStone(Stone stone[],int size,ALLEGRO_BITMAP *stone_image)
{
	int i;
	for (i = 0; i < size; i++)
	{
		stone[i].speed = 4;
		stone[i].stone_image = stone_image;
		stone[i].live = false;
		stone[i].boundx = 35;
		stone[i].boundy = 35;
		stone[i].health = 4;

	}
}
void drawStone(Stone stone[],int size,ALLEGRO_BITMAP *stone_image)
{
	int i;
	for (i = 0; i < size; i++)
	{

		if(stone[i].y >= HEIGHT)
		{
			stone[i].live = false;
		}
		if(stone[i].live)
		{
			al_draw_bitmap(stone[i].stone_image,stone[i].x,stone[i].y,0);
		}
	}
}
void startStone(Stone stone[],int size)
{
	int i;
	for(i = 0; i < size; i++)
	{
		if(!stone[i].live)
		{
			if(rand() % 200 == 0)
			{
				stone[i].live = true;
				stone[i].x = rand() % (WIDTH - 60);
				stone[i].y = 0;

				break;
			}
		}
	}
}
void updateStone(Stone stone[],int size)
{
	int i;
	for(i = 0; i < size; i++)
	{


		if(stone[i].live)
		{
			stone[i].y += stone[i].speed;
		}
	}
}

void collideStone(Stone stone[],int sSize,SpaceShip *ship,Explosion explosions[],int eSize)
{
	int j;

	for (j = 0; j < sSize; j++)
	{
		if(stone[j].live)
		{


					if(ship->x + ship->boundx > stone[j].x  &&
						ship->x - ship->boundx < stone[j].x + stone[j].boundx &&
						ship->y + ship->boundy  > stone[j].y  &&
						ship->y - ship->boundy < stone[j].y + stone[j].boundy
						)
					{
						
							ship->health--;
							stone[j].live = false;
							startExplosions(explosions, NUM_EXPLOSIONS, ship->x, ship->y);
						
						
					}
			
		}
	}

}

void initExplosions(Explosion explosions[], int size, ALLEGRO_BITMAP *image)
{
	int i;
	for(i = 0; i < size; i++)
	{
		explosions[i].live = false;

		explosions[i].maxFrame = 16;
		explosions[i].curFrame = 0;
		explosions[i].frameCount = 0;
		explosions[i].frameDelay = 1;
		explosions[i].frameWidth = 64;
		explosions[i].frameHeight = 64;
		explosions[i].animationColumns = 4;
		explosions[i].animationDirection = 1;

		explosions[i].image = image;
	}
}
void drawExplosions(Explosion explosions[], int size)
{
	int i,fx,fy;

	for(i = 0; i < size; i++)
	{
		if(explosions[i].live)
		{
			fx = (explosions[i].curFrame % explosions[i].animationColumns) * explosions[i].frameWidth;
			fy = (explosions[i].curFrame / explosions[i].animationColumns) * explosions[i].frameHeight;

			al_draw_bitmap_region(explosions[i].image, fx, fy, explosions[i].frameWidth,
				explosions[i].frameHeight, explosions[i].x - explosions[i].frameWidth / 2, explosions[i].y - explosions[i].frameHeight / 2, 0);
		}
	}
}
void startExplosions(Explosion explosions[], int size, int x, int y)
{
	int i;
	for(i = 0; i < size; i++)
	{
		if(!explosions[i].live)
		{
			explosions[i].live = true;
			explosions[i].x = x;
			explosions[i].y = y;
			break;
		}
	}
}
void updateExplosions(Explosion explosions[], int size)
{
	int i;
	for(i = 0; i < size; i++)
	{
		if(explosions[i].live)
		{
			if(++explosions[i].frameCount >= explosions[i].frameDelay)
			{
				explosions[i].curFrame += explosions[i].animationDirection;
				if(explosions[i].curFrame >= explosions[i].maxFrame)
				{
					explosions[i].curFrame = 0;
					explosions[i].live = false;
				}

				explosions[i].frameCount = 0;
			}
		}
	}
}

void initLaser(Laser *laser,SpaceShip *ship)
{
	laser->x = ship->x;
	laser->y = ship->y - 17;
	laser->live = true;
}

void drawLaser(Laser *laser)
{
	al_draw_line(laser->x,laser->y,laser->x,0,al_map_rgb(0,255,0),5);
}

void updateLaser(Laser *laser,SpaceShip *ship)
{
	laser->x = ship->x;
	laser->y = ship->y - 17;
}
void collideLaser(Laser *laser,Stone stone[],int size,SpaceShip *ship,Explosion explosions[],int eSize)
{
	int i;

	for (i = 0; i < size; i++)
	{
		if(stone[i].live)
		{
			//if(laser->y > ship->)
			if(laser->x > stone[i].x + 10 && laser->x < stone[i].x + 40 && stone[i].y  < ship->y)
			{
				stone[i].live = false;
				ship->score++;
				startExplosions(explosions,NUM_EXPLOSIONS, stone[i].x + stone[i].boundx, stone[i].y + stone[i].boundy);
			}
		}
	}
}
