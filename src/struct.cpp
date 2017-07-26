const int POS_MSG_TEXTUREID = 0;
const int POS_MSG_SIZE_W = 1;
const int POS_MSG_SIZE_H = 2;
const int POS_MSG_SPEED = 3;
const int POS_MSG_HP = 4;
const int POS_MSG_PTEMPLATE = 5;
const int POS_MSG_FRAME_W = 6;
const int POS_MSG_FRAME_H = 7;

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
struct objSize_t
{
	int w;
	int h;

	void Set( int mw, int mh )
	{
		w = mw;
		h = mh;
	}
	void Update( int uw, int uh )
	{
		w += uw;
		h += uh;
	}
	objSize_t( int mw, int mh )
	{
		Set( mw, mh );
	}
	objSize_t()
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
/*
struct human_t
{
	point_t pos;
	objSize_t size;
	
	float curSpeed;
	float normSpeed;

	int curHp;
	int maxHp;

	std::string name;
	std::string playerName;
	int type = -1;

	int id;
	int textureID;
	int netID = -1;

	SDL_Rect screenPos;
	SDL_Rect frame;

	char movDirection[2] = {0, 0};
	char drawDirection = 0;
	char attDirection = 0;
	char prevDrawDirection = 's';
	int frameDirection = 1;

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

	//TCPsocket socket;
	//bool active = false;
	
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

		size.Set( info[0], info[1] );
		maxHp = info[2];
		curHp = maxHp;
		normSpeed = info[3];
		curSpeed = normSpeed;
		screenPos = { 0, 0, info[4], info[5] };
		frame = { info[6], info[7], info[8], info[9] };
	}
	human_t()
	{}
};
*/
struct humanTemplate_t
{
	std::string name;
	objSize_t size;
	int textureID;
	char drawDirection;
	float maxSpeed;
	int maxHp;
	SDL_Rect frame;
	bool playerTemplate = false;

	// Copies info from int array to make a template
	void CreateFromInfo( int *info )
	{
		textureID = *(info+POS_MSG_TEXTUREID );
		size.Set( *(info+POS_MSG_FRAME_W), *(info+POS_MSG_FRAME_H) );
		maxSpeed = *(info+POS_MSG_SPEED);
		maxHp = *(info+POS_MSG_HP);
		playerTemplate = *(info+POS_MSG_PTEMPLATE);
		frame.w = *(info+POS_MSG_FRAME_W);
		frame.h = *(info+POS_MSG_FRAME_H);
	}

	// Copies info from the template to make an int array
	void MakeMesage( int *msg )
	{
		*(msg+POS_MSG_TEXTUREID) = textureID;
		*(msg+POS_MSG_SIZE_W) = size.w;
		*(msg+POS_MSG_SIZE_H) = size.h;
		*(msg+POS_MSG_SPEED) = maxSpeed;
		*(msg+POS_MSG_HP) = maxHp;
		*(msg+POS_MSG_PTEMPLATE) = playerTemplate;
		*(msg+POS_MSG_FRAME_W) = frame.w;
		*(msg+POS_MSG_FRAME_H) = frame.h;
	}

	humanTemplate_t( int *info )
	{
		CreateFromInfo( info );
	}
	humanTemplate_t()
	{}
};
struct human_t
{
	std::string name;
	humanTemplate_t *humanTemplate;
	point_t pos;
	objSize_t *size;
	int curHp;
	float curSpeed;

	char movDirection[2];
	char attDirection;

	void Deploy( humanTemplate_t *myTemplate, int x, int y, int w = -1, int h = -1 )
	{
		pos.Set( x, y );
		humanTemplate = myTemplate;
		size = &(humanTemplate->size);
		if( w != -1 )
			size->w = w;
		if( h != -1 )
			size->h = h;
	}
};

struct obsticle_t
{
	point_t pos;
	objSize_t size;

	std::string name;

	int textureID;
	int id;

	SDL_Rect screenPos;
	SDL_Rect frame;

	int curHealth;

	bool destroyable;
	bool stopsSpells;
	bool stopsHumans;

	obsticle_t( std::string myName, int *info, int myID, int myTextureID )
	{
		name = myName;
		id = myID;
		textureID = myTextureID;

		pos.Set( info[0], info[1] );
		size.Set( info[2], info[3] );

		curHealth = info[4];
		destroyable = info[5];
		stopsHumans = info[6];
		stopsSpells = info[7];

		screenPos = {0, 0, info[8], info[9]};
		frame = {info[10], info[11], info[12], info[13]};
	}
	obsticle_t()
	{}
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

struct texture_t
{
	int id;
	char filename[200];
	char *binaryTexture = nullptr;
	unsigned char hash[ SHA256_DIGEST_LENGTH ];
	SDL_Texture *texture = NULL;
	int fileSize;
	texture_t( const char *myName, bool makeHash = true, int myId = 0 )
	{
		id = myId;
		int i;
		for( i=0;myName[i]!='\0';i++ )
			filename[i] = myName[i];
		filename[i] = '\0';

		if( makeHash )
		{
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

			SHA256_CTX sha256ctx;
			SHA256_Init( &sha256ctx );
			SHA256_Update( &sha256ctx, binaryTexture, fileSize );
			SHA256_Final( hash, &sha256ctx );
		}
	}
	texture_t()
	{}
};
