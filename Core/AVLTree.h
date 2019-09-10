#ifndef AVLTREE_H
#define AVLTREE_H

#include "BinaryTree.h"
#include <initializer_list>

namespace rapid
{

template<typename T>
struct Compare
{
    int operator()(const T &arg1, const T &arg2) const
    {
        if(arg1 < arg2) return 1;
        if(arg2 < arg1) return -1;
        return 0;
    }
};

template<typename _DataType, typename _CompareType = Compare<_DataType>>
class AVLTree
{
private:
    using DataType = _DataType;
    using CompareType = _CompareType;
    using TreeType = BinaryTree<DataType>;
    using Self = AVLTree;
    using TreeNode = typename BinaryTree<DataType>::TreeNode;
public:
    using ValueType = DataType;
    using Reference = ValueType &;
    using ConstReference = const ValueType &;
    using RvalueReference = ValueType &&;
    using SizeType = size_type;

    using iterator = typename TreeType::iterator;
    using const_iterator = typename TreeType::const_iterator;

private:
    BinaryTree<DataType> _M_tree;

    void _F_adjust(TreeNode *node);
    iterator _F_insert(ConstReference arg);
    iterator _F_find(ConstReference arg) const;
    void _F_erase(TreeNode *node)
    { _F_adjust(_M_tree.erase(node)); }
public:
    AVLTree() { }
    AVLTree(const Self &tree)
        : _M_tree(TreeType(tree._M_tree)) { }
    AVLTree(Self &&tree)
        : _M_tree(TreeType(forward<Self>(tree)._M_tree)) { }
    AVLTree(std::initializer_list<ValueType> arg_list)
    { insert(arg_list); }

    void swap(const Self &tree)
    { _M_tree.swap(tree._M_tree); }
    void swap(Self &&tree)
    { _M_tree.swap(forward<Self>(tree)._M_tree); }

    iterator begin()
    { return _M_tree.begin(); }
    iterator end()
    { return _M_tree.end(); }
    const_iterator begin() const
    { return _M_tree.begin(); }
    const_iterator end() const
    { return _M_tree.end(); }
    const_iterator cbegin() const
    { return _M_tree.cbegin(); }
    const_iterator cend() const
    { return _M_tree.cend(); }

    typename TreeType::fiterator fbegin()
    { return _M_tree.fbegin(); }
    typename TreeType::fiterator fend()
    { return _M_tree.fend(); }
    typename TreeType::aiterator abegin()
    { return _M_tree.abegin(); }
    typename TreeType::aiterator aend()
    { return _M_tree.aend(); }

    SizeType size() const
    { return _M_tree.size(); }
    bool empty() const
    { return size() == 0; }
    SizeType depth() const
    { return _M_tree.depth(); }

    iterator find(ConstReference arg) const
    { return _F_find(arg); }
    iterator find(RvalueReference arg) const
    { return _F_find(forward<ValueType>(arg)); }

    iterator insert(std::initializer_list<ValueType> arg_list)
    {
        iterator result;
        for(auto it = arg_list.begin(); true; )
        {
            result = _F_insert(*it);
            if(++it == arg_list.end())
            {
                return result;
            }
        }
    }
    iterator insert(ConstReference arg)
    { return _F_insert(arg); }
    iterator insert(RvalueReference arg)
    { return _F_insert(forward<ValueType>(arg)); }
    void erase(ConstReference arg)
    { erase(find(arg)); }
    void erase(RvalueReference arg)
    { erase(find(forward<ValueType>(arg))); }
    void erase(iterator it)
    { _F_erase(_M_tree.tree_node(it)); }
};

//-----------------------impl-----------------------//
//-----------------------impl-----------------------//
//-----------------------impl-----------------------//
//-----------------------impl-----------------------//
//-----------------------impl-----------------------//
template<typename _DataType, typename _CompareType>
    void AVLTree<_DataType, _CompareType>::_F_adjust(TreeNode *node)
{
    TreeNode *visit = node;
    while(visit != nullptr)
    {
        SizeType left_dep = _M_tree.depth(_M_tree.left_child(visit));
        SizeType right_dep = _M_tree.depth(_M_tree.right_child(visit));
        if(left_dep > right_dep && left_dep - right_dep > 1)
        {
            TreeNode *temp = _M_tree.left_child(visit);
            if(_M_tree.depth(_M_tree.left_child(temp)) < _M_tree.depth(_M_tree.right_child(temp)))
            {
                _M_tree.left_rotate(temp);
            }
            visit = _M_tree.right_rotate(visit);
        }
        else if(right_dep > left_dep && right_dep - left_dep > 1)
        {
            TreeNode *temp = _M_tree.right_child(visit);
            if(_M_tree.depth(_M_tree.right_child(temp)) < _M_tree.depth(_M_tree.left_child(temp)))
            {
                _M_tree.right_rotate(temp);
            }
            visit = _M_tree.left_rotate(visit);
        }
        visit = _M_tree.parent(visit);
    }
}

template<typename _DataType, typename _CompareType>
typename AVLTree<_DataType, _CompareType>::iterator
    AVLTree<_DataType, _CompareType>::_F_find(ConstReference arg) const
{
    TreeNode *node = _M_tree.root();
    iterator result;
    while(true)
    {
        int res = CompareType()(arg, node->data());
        if(res == 0)
        {
            return result = node;
        }
        if(res > 0)
        {
            node = _M_tree.left_child(node);
        }
        else
        {
            node = _M_tree.right_child(node);
        }
    }
    return result;
}

template<typename _DataType, typename _CompareType>
typename AVLTree<_DataType, _CompareType>::iterator
    AVLTree<_DataType, _CompareType>::_F_insert(ConstReference arg)
{
    if(empty())
    {
        return iterator(_M_tree.append_root(arg));
    }
    iterator result;
    TreeNode *node = _M_tree.root();
    while(true)
    {
        int res = CompareType()(arg, node->data());
        if(res == 0)
        {
            node->data() = arg;
            return result = node;
        }
        if(res > 0)
        {
            if(_M_tree.left_child(node) == nullptr)
            {
                node = _M_tree.append_left(node, arg);
                break;
            }
            node = _M_tree.left_child(node);
        }
        else
        {
            if(_M_tree.right_child(node) == nullptr)
            {
                node = _M_tree.append_right(node, arg);
                break;
            }
            node = _M_tree.right_child(node);
        }
    }
    _F_adjust(node);
    return result = node;
}

}; // end namespace rapid
#endif // AVLTREE_H
