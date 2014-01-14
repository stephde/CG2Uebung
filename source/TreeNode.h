#ifndef TREENODE_H_INCLUDED
#define TREENODE_H_INCLUDED

#include <map>
#include <vector>


class TreeNode
{
public:
	static const int MAXIMUM_DEPTH = 4;

	//types
	enum Children{ LAST, NW, NE, SW, SE, ROOT };
	enum Tiles{ N, E, S, W, END};
	typedef std::pair<TreeNode::Children, TreeNode *> ChildEntry;
	typedef std::pair<TreeNode::Tiles, int> TileEntry;
	typedef std::pair<TreeNode::Tiles, std::vector <TreeNode *>> AdjEntry;

	TreeNode(TreeNode * parent, float x, float z, float extend, Children childType);
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
	TreeNode * getN(int value, int lvl, std::vector <TreeNode *> nodevect);
	TreeNode * getE(int value, int lvl, std::vector <TreeNode *> nodevect);
	TreeNode * getS(int value, int lvl, std::vector <TreeNode *> nodevect);
	TreeNode * getW(int value, int lvl, std::vector <TreeNode *> nodevect);
	TreeNode * getNodeForValue(int value);

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
	int positionNumber(){return m_positionNumber;}

	//static methods
	static void correctTree(TreeNode * node);

private:
	TreeNode * m_parent;
	
	std::map<Children, TreeNode *> m_children;
	std::map<Tiles, int> m_tileLods;

	Children m_childType;
	float m_extend;
	float m_x;
	float m_z;
	int m_rekursionLevel;
	int m_positionNumber;
};

#endif
