#ifndef TREENODE_H_INCLUDED
#define TREENODE_H_INCLUDED

#include <map>


class TreeNode
{
public:
	TreeNode(TreeNode * parent, float x, float z, float extend);
	virtual ~TreeNode();

	bool hasChildren();
	bool isRoot();
	
	//ToDo write functions to get Patches Left, Right, Top, Down
	//give children their position (NW, NE, ...) as parameter
	
	enum Children{ NW, NE, SW, SE };
	enum Tiles{ N, E, S, W};
	
	typedef std::pair<TreeNode::Children, TreeNode *> childEntry;

	std::map<Children, TreeNode*> subdivide();

	std::map<Children, TreeNode*> children(){return m_children;}

	float x(){return m_x;}
	float z(){return m_z;}
	float extend(){return m_extend;}

private:
	TreeNode * m_parent;
	
	std::map<Children, TreeNode *> m_children;

	float m_extend;
	float m_x;
	float m_z;

	/*** ToDo:  implement LODS!! ***/
	std::map<Tiles, int> m_tileLods;
};

#endif
