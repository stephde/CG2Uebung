#include "TreeNode.h"


TreeNode::TreeNode(TreeNode * parent, TreeNode * root, float x, float z, float extend, Children childType)
{
	m_parent = parent;
	m_root = root;

	m_x = x - extend / 2;
	m_z = z - extend / 2;
	m_extend = extend;
	m_childType = childType;

	for(int i = Tiles::N; i != Tiles::END; i++)
	{
		m_tileLods.insert(TileEntry(static_cast<Tiles>(i), (1)));
	}

	if(m_childType == Children::ROOT)
	{
		m_rekursionLevel = 0;
		m_positionNumber[0] = 0;
	}else{
		m_rekursionLevel = m_parent->rekursionLevel() + 1;
		m_positionNumber[m_rekursionLevel] = m_childType;
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
	if( m_children.empty() )
		return false;

	return true;
}

bool TreeNode::isRoot()
{
	if(m_childType == Children::ROOT /* && m_parent == nullptr*/)
		return true;

	return false;
}

TreeNode * TreeNode::children(const Children type)
{
	return m_children[type];
}


std::map<TreeNode::Tiles,  std::vector <TreeNode *>> TreeNode::getAdj()
{
	std::map<TreeNode::Tiles, std::vector <TreeNode *>> adj;
	
	std::vector <TreeNode *> node = getN();
	if(node.back() != nullptr) 
		adj.insert(AdjEntry( Tiles::N, getN() ));

	node = getE();
	if(node.back() != nullptr) 
		adj.insert(AdjEntry( Tiles::E, getE() ));

	node = getS();
	if(node.back() != nullptr) 
		adj.insert(AdjEntry( Tiles::S, getS() ));

	node = getW();
	if(node.back() != nullptr) 
		adj.insert(AdjEntry( Tiles::W, getW() ));

	return adj;
}

std::vector <TreeNode *> TreeNode::getN()
{
	//init
	std::vector <TreeNode *> nodevect;
	int lvl = MAXIMUM_DEPTH;
	char value[MAXIMUM_DEPTH];
	strcpy(value, m_positionNumber);
	
	//check if mult neighbors
	if(value[lvl] == 0)
	{
		value[lvl] = 3;
		getN(value, lvl-1, nodevect);
		value[lvl] = 4;
		getN(value, lvl-1, nodevect);

		return nodevect;
	}

	//calc neighbor address
	while(value[lvl] < 3 && lvl > 0)
	{
		value[lvl] += 2;
		--lvl;
	}
	
	//cancel if no neighbor
	if(lvl == 0)
	{
		nodevect.push_back(nullptr);
		return nodevect;
	}

	value[lvl] -= 2;
	nodevect.push_back(getNodeForValue(value));

	return nodevect;
}

int TreeNode::getN(char* location, int level, std::vector <TreeNode *> nodevect)
{
	int lvl = level;
	char value[MAXIMUM_DEPTH];
	strcpy(value, location);
	
	//check if multiple neighbors
	if(value[lvl] == 0)
	{
		value[lvl] = 3;
		getN(value, lvl-1, nodevect);
		value[lvl] = 4;
		getN(value, lvl-1, nodevect);
		return 0;
	}
	
	//while digit < 3 increase by 2
	while(value[lvl] < 3 && lvl > 0)
	{
		value[lvl] += 2;
		--lvl;	
	}

	//cancel if no neighbor
	if(lvl == 0)
	{
		nodevect.push_back(nullptr);
		return 1; //return ok so?
	}

	//first digit > 3 decrease by 2
	value[lvl] -= 2;

	//get neighboring node and pushback
	nodevect.push_back(getNodeForValue(value));
	return 0;
}

std::vector <TreeNode *> TreeNode::getE()
{
	//init
	std::vector <TreeNode *> nodevect;
	int lvl = MAXIMUM_DEPTH;
	char value[MAXIMUM_DEPTH];
	strcpy(value, m_positionNumber);
	
	//check if mult neighbors
	if(value[lvl] == 0)
	{
		value[lvl] = 1;
		getN(value, lvl-1, nodevect);
		value[lvl] = 3;
		getN(value, lvl-1, nodevect);

		return nodevect;
	}

	//calc neighbor address
	while(value[lvl] % 2 == 0 && lvl > 0)
	{
		value[lvl]--;
		--lvl;
	}

	//cancel if no neighbor
	if(lvl == 0)
	{
		nodevect.push_back(nullptr);
		return nodevect;
	}

	value[lvl]++;
	nodevect.push_back(getNodeForValue(value));

	return nodevect;
}

int TreeNode::getE(char* location, int level, std::vector <TreeNode *> nodevect)
{
	int lvl = level;
	char value[MAXIMUM_DEPTH];
	strcpy(value, location);

	//check for multiple neighbors
	if(value[lvl] == 0)
	{
		value[lvl] = 1;
		getN(value, lvl-1, nodevect);
		value[lvl] = 3;
		getN(value, lvl-1, nodevect);
		return 0;
	}
	
	//while digit even decrease by 1
	while(value[lvl] % 2 == 0  && lvl > 0)
	{
		value[lvl]--;
		--lvl;	
	}

	//cancel if no neighbor
	if(lvl == 0)
	{
		nodevect.push_back(nullptr);
		return 1; //return ok so?
	}

	//first uneven digit increase by 1
	value[lvl]++;

	//get neighboring node and pushback
	nodevect.push_back(getNodeForValue(value));
	return 0;
}

std::vector <TreeNode *> TreeNode::getS()
{
	//init
	std::vector <TreeNode *> nodevect;
	int lvl = MAXIMUM_DEPTH;
	char value[MAXIMUM_DEPTH];
	strcpy(value, m_positionNumber);
	
	//check if mult neighbors
	if(value[lvl] == 0)
	{
		value[lvl] = 1;
		getN(value, lvl-1, nodevect);
		value[lvl] = 2;
		getN(value, lvl-1, nodevect);

		return nodevect;
	}

	//calc neighbor address
	while(value[lvl] >= 3 && lvl > 0)
	{
		value[lvl] -= 2;
		--lvl;
	}

	//cancel if no neighbor
	if(lvl == 0)
	{
		nodevect.push_back(nullptr);
		return nodevect;
	}

	value[lvl] += 2;
	nodevect.push_back(getNodeForValue(value));

	return nodevect;
}

int TreeNode::getS(char* location, int level, std::vector <TreeNode *> nodevect)
{
	int lvl = level;
	char value[MAXIMUM_DEPTH];
	strcpy(value, location);
	
	//check for multiple neighbors
	if(value[lvl] == 0)
	{
		value[lvl] = 1;
		getN(value, lvl-1, nodevect);
		value[lvl] = 2;
		getN(value, lvl-1, nodevect);
		return 0;
	}
	
	//while digit >= 3 decrease by 2
	while(value[lvl] >= 3 && lvl > 0)
	{
		value[lvl] += 2;
		--lvl;	
	}

	//cancel if no neighbor
	if(lvl == 0)
	{
		nodevect.push_back(nullptr);
		return 1; //return ok so?
	}

	//first digit > 3 decrease by 2
	value[lvl] -= 2;

	//get neighboring node and pushback
	nodevect.push_back(getNodeForValue(value));
	return 0;
}

std::vector <TreeNode *> TreeNode::getW()
{
	//init
	std::vector <TreeNode *> nodevect;
	int lvl = MAXIMUM_DEPTH;
	char value[MAXIMUM_DEPTH];
	strcpy(value, m_positionNumber);
	
	//check for mult neighbors
	if(value[lvl] == 0)
	{
		value[lvl] = 3;
		getN(value, lvl-1, nodevect);
		value[lvl] = 4;
		getN(value, lvl-1, nodevect);

		return nodevect;
	}

	//calc neighbor address
	while(value[lvl] % 2 == 1 && lvl > 0)
	{
		value[lvl]++;
		--lvl;
	}

	//cancel if no neighbor
	if(lvl == 0)
	{
		nodevect.push_back(nullptr);
		return nodevect;
	}

	value[lvl]--;
	nodevect.push_back(getNodeForValue(value));

	return nodevect;
}

int TreeNode::getW(char* location, int level, std::vector <TreeNode *> nodevect)
{
	int lvl = level;
	char value[MAXIMUM_DEPTH];
	strcpy(value, location);
	
	//check for multiple neighbors
	if(value[lvl] == 0)
	{
		value[lvl] = 3;
		getN(value, lvl-1, nodevect);
		value[lvl] = 4;
		getN(value, lvl-1, nodevect);
		return 0;
	}
	
	//while digit even increase by 1
	while(value[lvl] % 2 == 1  && lvl > 0)
	{
		value[lvl]++;
		--lvl;	
	}

	//cancel if no neighbor
	if(lvl == 0)
	{
		nodevect.push_back(nullptr);
		return 1; //return ok so?
	}

	//first even digit decrease by 1
	value[lvl]--;

	//get neighboring node and pushback
	nodevect.push_back(getNodeForValue(value));
	return 0;
}

std::map<TreeNode::Children, TreeNode*> TreeNode::subdivide()
{
	//create all 4 children
	float e = m_extend/2;
	if(m_root == nullptr)
	{
		m_children.insert(ChildEntry(Children::NW, new TreeNode(this, this, m_x, m_z, e, Children::NW)));
		m_children.insert(ChildEntry(Children::NE, new TreeNode(this, this, m_x + e, m_z, e, Children::NE)));
		m_children.insert(ChildEntry(Children::SW, new TreeNode(this, this, m_x, m_z + e, e, Children::SW)));
		m_children.insert(ChildEntry(Children::SE, new TreeNode(this, this, m_x + e, m_z + e, e, Children::SE)));
	}else{
		m_children.insert(ChildEntry(Children::NW, new TreeNode(this, m_root, m_x, m_z, e, Children::NW)));
		m_children.insert(ChildEntry(Children::NE, new TreeNode(this, m_root, m_x + e, m_z, e, Children::NE)));
		m_children.insert(ChildEntry(Children::SW, new TreeNode(this, m_root, m_x, m_z + e, e, Children::SW)));
		m_children.insert(ChildEntry(Children::SE, new TreeNode(this, m_root, m_x + e, m_z + e, e, Children::SE)));
	}

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

TreeNode * TreeNode::getNodeForValue(char *value)
{
	TreeNode * target = m_root;
	int lvl = 0;

	std::map<TreeNode::Children, TreeNode *> children;
	while(target->hasChildren())
	{
		children = target->children();

		if(value[lvl] == 1)
			target = children[Children::NW];
		if(value[lvl] == 2)
			target = children[Children::NE];
		if(value[lvl] == 3)
			target = children[Children::SW];
		if(value[lvl] == 4)
			target = children[Children::SE];

		lvl++;

		if(lvl < MAXIMUM_DEPTH)
			return nullptr;
	}
	
	return target;
}

void TreeNode::correctTree(TreeNode * node)
{
	//if Patch not last
	if(node->hasChildren())
	{
		auto children = node->children();
		for(auto i : children)
		{
			correctTree(i.second);
		}
	}else{
		//get patches left, right ...
		auto neighbors = node->getAdj();
		for(auto i : neighbors)
		{
			//check if neighbor lod is compatible with nodes Lod
			switch(i.first)
			{
				case Tiles::N:
				case Tiles::E:
				case Tiles::S:
				case Tiles::W:
					break;
			}
		}
	}
}