#include "Quadtree.h"

Quadtree::Quadtree(Quadtree * parent, float x, float y, float extend)
{
	m_parent = parent;

	m_x = x;
	m_y = y;
	m_extend = extend;
}

bool Quadtree::isLast()
{
	if( m_children._Isnil)return true;

	return false;
}

std::map<Quadtree::Children, Quadtree*> Quadtree::subdivide()
{
	//create all 4 children
	float e = m_extend/2;
	m_children.insert(childEntry(Children::NW, new Quadtree(this, m_x, m_y, e)));
	m_children.insert(childEntry(Children::NE, new Quadtree(this, m_x + e, m_y, e)));
	m_children.insert(childEntry(Children::SW, new Quadtree(this, m_x, m_y + e, e)));
	m_children.insert(childEntry(Children::SE, new Quadtree(this, m_x + e, m_y + e, e)));


	//return array of children

	return m_children;
}
