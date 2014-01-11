#ifndef TREENODE_H_INCLUDED
#define TREENODE_H_INCLUDED

#include <map>
#include <vector>


class TreeNode
{
public:
	static const int MAXIMUM_DEPTH = 4;

	enum Children{ LAST, NW, NE, SW, SE, ROOT };
	enum Tiles{ N, E, S, W, END};

	TreeNode(TreeNode * parent, float x, float z, float extend, Children childType);
	virtual ~TreeNode();

	bool hasChildren();
	bool isRoot();
	
	//ToDo write functions to get Patches Left, Right, Top, Down
	//give children their position (NW, NE, ...) as parameter

	std::map<TreeNode::Tiles, TreeNode *> getAdj();
	TreeNode * getN();
	TreeNode * getE();
	TreeNode * getS();
	TreeNode * getW();
		
	typedef std::pair<TreeNode::Children, TreeNode *> childEntry;
	typedef std::pair<TreeNode::Tiles, int> tileEntry;
	typedef std::pair<TreeNode::Tiles, TreeNode *> adjEntry;

	std::map<Children, TreeNode*> subdivide();

	std::map<Children, TreeNode*> children(){return m_children;}
	TreeNode* children(Children type);

	TreeNode * getNodeForValue(int value);

	bool canIncreaseLods();
	void increaseLods();
	void setLod(Tiles t, int lod);
	std::map<Tiles, int> tileLods(){return m_tileLods;}
	int avgLod();

	float x(){return m_x;}
	float z(){return m_z;}
	float extend(){return m_extend;}
	int rekursionLevel(){return m_rekursionLevel;}
	int positionNumber(){return m_positionNumber;}

private:
	TreeNode * m_parent;
	
	std::map<Children, TreeNode *> m_children;

	Children m_childType;

	float m_extend;
	float m_x;
	float m_z;
	int m_rekursionLevel;

	int m_positionNumber;

	/*** ToDo:  implement LODS!! ***/
	std::map<Tiles, int> m_tileLods;
};

#endif
