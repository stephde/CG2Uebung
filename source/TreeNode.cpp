#include "TreeNode.h"


TreeNode::TreeNode(TreeNode * parent, float x, float z, float extend)
{
	m_parent = parent;

	m_x = x - extend / 2;
	m_z = z - extend / 2;
	m_extend = extend;
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