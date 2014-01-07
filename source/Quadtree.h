#ifndef QUADTREE_H_INCLUDED
#define QUADTREE_H_INCLUDED

#include <map>


class Quadtree
{
public:
	Quadtree(Quadtree * parent, float x, float y, float extend);
	~Quadtree();

	bool isLast();
	
	//ToDo write functions to get Patches Left, Right, Top, Down
	//give children their position (NW, NE, ...) as parameter
	
	enum Children{ NW, NE, SW, SE };
	enum Tiles{ N, E, S, W};
	
	typedef std::pair<Quadtree::Children, Quadtree *> childEntry;

	std::map<Children, Quadtree*> subdivide();


private:
	Quadtree * m_parent;
	
	std::map<Children, Quadtree *> m_children;

	int m_extend;
	int m_x;
	int m_y;

	/*** ToDo:  implement LODS!! ***/
	std::map<Tiles, int> m_tileLods;
};

#endif
