typedef struct
{
	int x;
	int y;
	int score;
	int health;
	bool dead;
	int speed;
	int boundx;
	int boundy;
	int frameWidth;
	int frameHeight;
	ALLEGRO_BITMAP *image;

}SpaceShip;

typedef struct 
{
	int x;
	int y;
	int live;
	int speed;

}Bullet;

typedef struct 
{
	int x;
	int y;
	bool live;
	int speed;
	int width;
	int height;
	int boundx;
	int boundy;
	int health;
	ALLEGRO_BITMAP *stone_image;	
}Stone;

typedef struct 
{
	int x;
	int y;
	bool live;

	int maxFrame;
	int curFrame;
	int frameCount;
	int frameDelay;
	int frameWidth;
	int frameHeight;
	int animationColumns;
	int animationDirection;

	ALLEGRO_BITMAP *image;

}Explosion;

typedef struct 
{
	int x;
	int y;
	int boundx;
	int boundy;
	bool live;
}Laser;