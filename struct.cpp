struct human_t
{
	int id;
	SDL_Rect pos;
	SDL_Rect frame;
	char movDirection[2] = {0, 0};
	char drawDirection = 0;
	char attDirection = 0;
	char prevDrawDirection = 's';
	int frameDirection = 1;
	int curHealth;
	int maxHealth;
	int speed;
	int normSpeed;
	int x;
	int y;
	int w = 50;
	int h = 50;
	std::vector<int> avalSpells;
	int eqpSpell;
	int curSpellNum;
	int spellWaitTime[5];
	
	int state = 0;
	
	human_t()
	{
		for(int i=0;i<5;i++)
			spellWaitTime[i] = 0;
	}
};

struct obsticle_t
{
	SDL_Rect pos;
	SDL_Rect frame;
	int curHealth;
	bool destroyable = true;
	char type;
	int x;
	int y;
	int w;
	int h;
};

struct aoe_t
{
	int id;
	SDL_Rect pos;
	SDL_Rect frame;
	double angle;
	SDL_Point* point;
	SDL_RendererFlip flip;
	char dir;
	int castByID;
	int dmg;
	int waitTime;
	int usrSlowDown;
	int usrSlowDownDur;
	int speed;
	int duration;
	int x;
	int y;
	int w;
	int h;
};
