#ifndef TREENODE_H_INCLUDED
#define TREENODE_H_INCLUDED

#include <map>
#include <vector>
#include <array>


class TreeNode
{
public:
	static const int MAXIMUM_DEPTH = 5;

	//types
	enum Children{ LAST, NW, NE, SW, SE, ROOT };
	enum Tiles{ N, E, S, W, END};
	typedef std::pair<TreeNode::Children, TreeNode *> ChildEntry;
	typedef std::pair<TreeNode::Tiles, int> TileEntry;
	typedef std::pair<TreeNode::Tiles, std::vector <TreeNode *>> AdjEntry;

	TreeNode(TreeNode * parent, TreeNode * root, float x, float z, float extend, Children childType);
	virtual ~TreeNode();

	//methods for children
	std::map<Children, TreeNode*> subdivide();
	std::map<Children, TreeNode*> children(){return m_children;}
	TreeNode* children(const Children type);
	
	//methods for neighbors
	std::map<TreeNode::Tiles, std::vector <TreeNode *>> getAdj();
	std::vector <TreeNode *> getN();
	std::vector <TreeNode *> getE();
	std::vector <TreeNode *> getS();
	std::vector <TreeNode *> getW();
	std::vector <TreeNode *> getN(std::array<int, MAXIMUM_DEPTH+1> value, int lvl);
	std::vector <TreeNode *> getE(std::array<int, MAXIMUM_DEPTH+1> value, int lvl);
	std::vector <TreeNode *> getS(std::array<int, MAXIMUM_DEPTH+1> value, int lvl);
	std::vector <TreeNode *> getW(std::array<int, MAXIMUM_DEPTH+1> value, int lvl);
	TreeNode * getNodeForValue(std::array<int, MAXIMUM_DEPTH+1> value);

	//methods for lods
	bool canIncreaseLods();
	void increaseLods();
	void setLod(Tiles t, int lod);
	std::map<Tiles, int> tileLods(){return m_tileLods;}
	int avgLod();
	
	//accessors
	bool hasChildren();
	bool isRoot();	
	float x(){return m_x;}
	float z(){return m_z;}
	float extend(){return m_extend;}
	int rekursionLevel(){return m_rekursionLevel;}
	std::array<int, MAXIMUM_DEPTH+1> positionNumber(){return m_positionNumber;}

	//static methods
	static void correctTree(TreeNode * node);

private:
	TreeNode * m_parent;
	TreeNode * m_root;
	
	std::map<Children, TreeNode *> m_children;
	std::map<Tiles, int> m_tileLods;

	Children m_childType;
	float m_extend;
	float m_x;
	float m_z;
	int m_rekursionLevel;
	std::array<int, MAXIMUM_DEPTH+1> m_positionNumber;
};

#endif
