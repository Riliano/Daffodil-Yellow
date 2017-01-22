struct node_t
{
	int x;
	int y;
	int cameFromX;
	int cameFromY;
	int value;
	bool operator < ( const node_t &cmp ) const
	{
		return value > cmp.value;
	}
};
struct flag_t
{
	int x;
	int y;
	int w;
	int h;

	bool forAll = false;
	bool fullCollision = true;
	int belongsToID;
	int type;

	char goTo[2] = {0, 0};
	void ConstructFromNode_t( const node_t &node ) 
	{
		this->x = node.cameFromX;
		this->y = node.cameFromY;
		if( node.cameFromY > node.y )
			this->goTo[0] = 'n';
		if( node.cameFromY < node.y )
			this->goTo[0] = 's';
		if( node.cameFromY == node.y )
			this->goTo[0] = 0;
		if( node.cameFromX > node.x )
			this->goTo[1] = 'w';	
		if( node.cameFromX < node.x )
			this->goTo[1] = 'e';
		if( node.cameFromX == node.x )
			this->goTo[1] = 0;
	}
};

struct worker_t
{
	std::thread trd;
	bool done = true;
	bool quit = false;
};

struct human_t
{
	int id;
	int textureID;
	int netID = -1;
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
	int w;
	int h;
	std::vector<int> avalSpells;
	int eqpSpell;
	int curSpellNum = 0;
	int spellWaitTime[5];
	long long drawT = 0;
	
	int state;
	int threadID;
	int targetX;
	int targetY;
	int targetID;
	int visionW = 500;
	int visionH = 800;
	char prevAttDir;

	std::vector<flag_t> navMesh;
	std::vector<flag_t> patrolPoint;
	int patrolCycle = 0;
	int switchToNextPoint = 1;
	bool cycle = true;

	TCPsocket socket;
	bool active;

	void CreateFromInfo( std::vector<int> info, int giveMeID, int giveMeTextureID, int giveMeThreadID, float scale = 1 )
	{
		for( int i=0;i<5;i++ )
			spellWaitTime[i]=0;
		id = giveMeID;
		textureID = giveMeTextureID;
		threadID = giveMeThreadID;
		state = info[0];
		x = info[1];
		y = info[2];
		w = info[3]*scale;
		h = info[4]*scale;

		maxHealth = info[5];
		curHealth = maxHealth;
		normSpeed = info[6];
		pos = {info[7], info[8], (int) (info[9]*scale), (int) (info[10]*scale)};
		frame = {info[11], info[12], info[13], info[14]};
		
		if( frame.y == frame.h*0 )
			prevDrawDirection = 's';
		if( frame.y == frame.h*1 )
			prevDrawDirection = 'w';
		if( frame.y == frame.h*2 )
			prevDrawDirection = 'e';
		if( frame.y == frame.h*3 )
			prevDrawDirection = 'n';

		for( int i=15;i<info.size();i++ )
			avalSpells.push_back( info[i] );
		if( !avalSpells.empty() )
		{
			curSpellNum = 0;
			eqpSpell = avalSpells[0];
		}

		flag_t start;
		start.x = x;
		start.y = y;
		patrolPoint.push_back(start);

	}
	void DrawDir()
	{
		switch(drawDirection)
		{
			case 'n' : frame.y=3*frame.h;break;
			case 's' : frame.y=0*frame.h;break;
			case 'e' : frame.y=2*frame.h;break;
			case 'w' : frame.y=1*frame.h;break;
			case 0 : return;
		}
		prevDrawDirection = drawDirection;
		drawDirection = 0;
	}
	void Draw()
	{
		if( drawDirection == 0 )
		{
			frame.x = 1*frame.w;
			return;
		}
		if( drawDirection == prevDrawDirection or prevDrawDirection == attDirection )
		{
			frame.x+=frame.w*frameDirection;
			if( frame.x >=frame.w*2 or frame.x == 0 )
				frameDirection = frameDirection*(-1);
		}else
		{
			frame.x = 0;
			frameDirection = 1;
		}
		prevDrawDirection = drawDirection;
		drawDirection = 0;
	}
};

struct obsticle_t
{
	int textureID;
	int id;
	SDL_Rect pos;
	SDL_Rect frame;
	int curHealth;
	bool destroyable;
	bool stopsSpells;
	bool stopsHumans;
	int x;
	int y;
	int w;
	int h;

	obsticle_t( std::vector<int> info, int giveMeTextureID, float scale = 1 )
	{
		textureID = giveMeTextureID;
		x = info[0];
		y = info[1];
		w = info[2]*scale;
		h = info[3]*scale;

		curHealth = info[4];
		destroyable = info[5];
		stopsSpells = info[6];
		stopsHumans = info[7];
		
		pos = {info[8], info[9], (int) (info[10]*scale), (int) (info[11]*scale)};
		frame = {info[12], info[13], info[14], info[15]};
	}
};
struct aoe_t
{
	int id;
	SDL_Rect pos;
	SDL_Rect frame;
	int frames;
	int textureID;
	double angle;
	SDL_Point* point = NULL;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
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
	void Draw()
	{
        	if(duration<=-1)
	        {
        	        duration--;
                	return;
		}
		frame.x += frame.w;
		
		if( frame.x >= frames*frame.w )
		{
			frame.x = 0;
			if( duration == 0 )
			{
				frame.x = frames*frame.w;
				duration = -1;
			}
		}
	}
	void CreateFromInfo( std::vector<int> info, int getTextureID, float scale = 1 )
	{
		textureID = getTextureID;
		dmg = info[0];
		speed = info[1];
		w = info[2];
		h = info[3];
		duration = info[4];
		usrSlowDown = info[5];
		usrSlowDownDur = info[6];
		waitTime = info[7];
		pos.w = (float) info[8]*scale;
		pos.h = (float) info[9]*scale;
		frame = { info[10], info[11], info[12], info[13] };
		frames = info[14];
	}
};
