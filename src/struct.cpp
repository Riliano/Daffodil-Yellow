struct point_t
{
	double x;
	double y;

	void Set( double mx, double my )
	{
		x = mx;
		y = my;
	}
	void Update( double ux, double uy )
	{
		x += ux;
		y += uy;
	}
	point_t( double mx, double my )
	{
		Set( mx, my );
	}
	point_t()
	{}
};
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
	std::string name;
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
	int eqpSpell = -1;
	int curSpellNum = 0;
	int spellWaitTime[200];
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
	bool active = false;

	void Set( int mX, int mY )
	{
		x = mX;
		y = mY;
	}
	void SetPos( int mX, int mY )
	{
		pos.x = mX;
		pos.y = mY;
	}
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
		for( int i=0;i<200;i++ )
			spellWaitTime[i] = 0;

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

	human_t( std::string myName, int *info, int myTextureID = -1 )
	{
		name = myName;
		textureID = myTextureID;

		w = info[0];
		h = info[1];
		maxHealth = info[2];
		curHealth = maxHealth;
		normSpeed = info[3];
		speed = normSpeed;
		pos = { 0, 0, info[4], info[5] };
		frame = { info[6], info[7], info[8], info[9] };
	}
	human_t()
	{}
};

struct obsticle_t
{
	std::string name;
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

	void CreateFromInfo( std::vector<int> info, int giveMeTextureID, float scale = 1 )
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
	void Set( int mX, int mY )
	{
		x = mX;
		y = mY;
	}

	obsticle_t( std::string myName, int *info, int myTextureID )
	{
		name = myName;
		textureID = myTextureID;
		
		w = info[0];
		h = info[1];
		
		curHealth = info[2];
		destroyable = info[3];
		stopsHumans = info[4];
		stopsSpells = info[5];

		pos = { 0, 0, info[6], info[7] };
		frame = { info[8], info[9], info[10], info[11] };
	}
	obsticle_t()
	{}
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

struct pathSegment_t
{
	char type;
	int speed;
	int time;
	int angle;
};
struct path_t
{
	std::vector< pathSegment_t > segments;
	int pathIter = 0;
	pathSegment_t *curPath = nullptr;
	void NextPath()
	{
		pathIter++;
		if( pathIter >= segments.size() )
			pathIter = 0;
		curPath = &( segments[pathIter] );
		//circle?
	}
	void PushSegment( pathSegment_t toPush )
	{
		segments.push_back( toPush );
	}
	void Rotate( int angle )
	{
		if( angle != 0 )
			for( int i=0;i<segments.size();i++ )
				segments[i].angle += angle;
	}
	bool Empty()
	{
		return segments.empty();
	}
	
};
struct bullet_t
{
	point_t location;
	int w;
	int h;

	int remainingTime;
	int changePathTime;
	path_t path;
	void UpdatePos( point_t update )
	{
		location.Update( update.x, update.y );
	}
	void SetPos( point_t myPos )
	{
		location.Set( myPos.x, myPos.y );
	}		
	void Rotate( int angle )
	{
		path.Rotate( angle );
	}
	SDL_Rect MakeRect()
	{
		SDL_Rect myRect;
		myRect.x = location.x;
		myRect.y = location.y;
		myRect.w = w;
		myRect.h = h;
	}

	int textureID;
	int castByID;
	int dmg;
	double angle;
	SDL_Rect pos;
	SDL_Rect frame;



};
struct spawner_t
{
	point_t pos;
	bullet_t blueprint;
	int angleOffset = 0;
	
	int changePathTime;
	int timeToSpawn;
	int interval;
	int numBulletsToSpawn;
	int spawnedBullets = 0;

	path_t path;

	bullet_t CreateBullet()
	{
		bullet_t bullet = blueprint;
		bullet.SetPos( pos );
		bullet.Rotate( angleOffset );
		return bullet;
	}

	spawner_t( int mx, int my, int myAngle, int myInterval, int numBullets )
	{
		pos.Set( mx, my );
		angleOffset = myAngle;
		numBulletsToSpawn = numBullets;
		interval = myInterval;
	}
};
struct attack_t
{
	std::vector< spawner_t > spawners;
};

struct texture_t
{
	int id;
	char filename[200];
	char *binaryTexture;
	SDL_Texture *texture = NULL;
	int fileSize;
	texture_t( const char *myName, int myId = 0 )
	{
		id = myId;
		int i;
		for( i=0;myName[i]!='\0';i++ )
			filename[i] = myName[i];
		filename[i] = '\0';

#ifdef RENDER
		if( renderer != NULL )
			texture = IMG_LoadTexture( renderer, filename );
#endif
#ifdef SERVER
		std::ifstream file;
		file.open( filename, std::ifstream::binary );
		if( file.is_open() )
		{
			file.seekg( 0, file.end );
			fileSize = (int)file.tellg();
			file.seekg( 0, file.beg );
			binaryTexture = new char[fileSize];
			file.read( binaryTexture, fileSize );
			file.close();
		}
#endif
	}
	texture_t()
	{}
};
