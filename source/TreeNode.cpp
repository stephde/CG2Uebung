#include "TreeNode.h"


TreeNode::TreeNode(TreeNode * parent, float x, float z, float extend)
{
	m_parent = parent;

	m_x = x - extend / 2;
	m_z = z - extend / 2;
	m_extend = extend;

	for(int i = Tiles::N; i != Tiles::END; i++)
	{
		m_tileLods.insert(tileEntry(static_cast<Tiles>(i), (0)));
	}
}

TreeNode::~TreeNode()
{
	m_parent = nullptr;

	for(auto i : m_children)
	{
		delete i.second;
	}
}

bool TreeNode::hasChildren()
{
	if( m_children.empty() )return false;

	return true;
}

bool TreeNode::isRoot()
{
	if(m_parent == nullptr)return true;

	return false;
}
 
std::map<TreeNode::Children, TreeNode*> TreeNode::subdivide()
{
	//create all 4 children
	float e = m_extend/2;
	m_children.insert(childEntry(Children::NW, new TreeNode(this, m_x, m_z, e)));
	m_children.insert(childEntry(Children::NE, new TreeNode(this, m_x + e, m_z, e)));
	m_children.insert(childEntry(Children::SW, new TreeNode(this, m_x, m_z + e, e)));
	m_children.insert(childEntry(Children::SE, new TreeNode(this, m_x + e, m_z + e, e)));


	//return array of children

	return m_children;
}

bool TreeNode::canIncreaseLods()
{
	if(m_tileLods[Tiles::N] < 2)
		return true;

	return false;
}

void TreeNode::increaseLods()
{
	std::map<Tiles, int>::iterator it;
	for(it = m_tileLods.begin(); it!= m_tileLods.end(); it++)
	{
		it->second++;
	}
}

void TreeNode::setLod(Tiles t, int lod)
{
	if(0 <= lod && lod < 3)
		m_tileLods[t] = lod;
}

int TreeNode::avgLod()
{
	int avg = 0;
	std::map<Tiles, int>::iterator it;
	for(it = m_tileLods.begin(); it != m_tileLods.end(); it++)
	{
		avg += it->second;
	}

	return static_cast<int>(avg / m_tileLods.size());
}