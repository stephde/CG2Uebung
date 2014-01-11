#include "TreeNode.h"


TreeNode::TreeNode(TreeNode * parent, float x, float z, float extend, Children childType)
{
	m_parent = parent;

	m_x = x - extend / 2;
	m_z = z - extend / 2;
	m_extend = extend;
	m_childType = childType;

	for(int i = Tiles::N; i != Tiles::END; i++)
	{
		m_tileLods.insert(tileEntry(static_cast<Tiles>(i), (1)));
	}

	if(m_parent == nullptr)m_rekursionLevel = 0;
	else m_rekursionLevel = m_parent->rekursionLevel() + 1;

	m_positionNumber = pow(10, MAXIMUM_DEPTH-m_rekursionLevel) * m_childType + m_parent->positionNumber();
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

TreeNode * TreeNode::children(Children type)
{
	return m_children[type];
}


std::map<TreeNode::Tiles, TreeNode *> TreeNode::getAdj()
{
	std::map<TreeNode::Tiles, TreeNode *> adj;
	
	adj.insert(adjEntry( Tiles::N, getN() ));
	adj.insert(adjEntry( Tiles::E, getE() ));
	adj.insert(adjEntry( Tiles::S, getS() ));
	adj.insert(adjEntry( Tiles::W, getW() ));
}

TreeNode * TreeNode::getN()
{
	int lvl = MAXIMUM_DEPTH - m_rekursionLevel;
	int value;

	while((value % static_cast<int>(pow(10, lvl))) < 3)
	{
		if((value % static_cast<int>(pow(10, lvl))) == 0)
			return nullptr;

		value += 2 * pow(10, lvl);
		lvl++;
	}
	

	value -= 2 * pow(10, lvl);

	return /*m_root->*/getNodeForValue(value);
}
 
TreeNode * TreeNode::getE()
{
	int lvl = MAXIMUM_DEPTH - m_rekursionLevel;
	int value;

	while((value % static_cast<int>(pow(10, lvl))) % 2 == 0)
	{
		if((value % static_cast<int>(pow(10, lvl))) == 0)
			return nullptr;

		value -= pow(10, lvl);
		lvl++;
	}
	

	value += pow(10, lvl);

	return /*m_root->*/getNodeForValue(value);

}

TreeNode * TreeNode::getS()
{
	int lvl = MAXIMUM_DEPTH - m_rekursionLevel;
	int value;

	while((value % static_cast<int>(pow(10, lvl))) >= 3)
	{
		if((value % static_cast<int>(pow(10, lvl))) == 0)
			return nullptr;

		value -= 2 * pow(10, lvl);
		lvl++;
	}
	

	value += 2 * pow(10, lvl);

	return /*m_root->*/getNodeForValue(value);
}

TreeNode * TreeNode::getW()
{
	int lvl = MAXIMUM_DEPTH - m_rekursionLevel;
	int value;

	while((value % static_cast<int>(pow(10, lvl))) % 2 == 0)
	{
		if((value % static_cast<int>(pow(10, lvl))) == 0)
			return nullptr;

		value += pow(10, lvl);
		lvl++;
	}
	

	value -= pow(10, lvl);

	return /*m_root->*/getNodeForValue(value);
}
std::map<TreeNode::Children, TreeNode*> TreeNode::subdivide()
{
	//create all 4 children
	float e = m_extend/2;
	m_children.insert(childEntry(Children::NW, new TreeNode(this, m_x, m_z, e, Children::NW)));
	m_children.insert(childEntry(Children::NE, new TreeNode(this, m_x + e, m_z, e, Children::NE)));
	m_children.insert(childEntry(Children::SW, new TreeNode(this, m_x, m_z + e, e, Children::SW)));
	m_children.insert(childEntry(Children::SE, new TreeNode(this, m_x + e, m_z + e, e, Children::SE)));


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