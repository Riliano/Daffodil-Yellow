#include "../class.hpp"
class point_t
{
	public:
		float x;
		float y;

		void Set( float myX, float myY )
		{
			x = myX;
			y = myY;
		}
		void Update( float upX, float upY )
		{
			x += upX;
			y += upY;
		}

		point_t()
		{
			x = 0;
			y = 0;
		}
		point_t( float myX, float myY )
		{
			Set( myX, myY );
		}
};

class dimensions_t
{
	public:
		float width;
		float height;

		void Set( float myW, float myH )
		{
			width = myW;
			height = myH;
		}
		
		dimensions_t()
		{
			width = 0;
			height = 0;
		}
};
