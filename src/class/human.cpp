#include "../class.hpp"

class humanBlueprint_t
{
	public:

		int id;
		int textureID;
		std::string name;

		dimensions_t dimensions;

		float maxSpeed;
		int maxHp;

		dimensions_t textureFrame;

		bool playable;


};
