#pragma once
#include "AVLTree.h"
#include <exception>

namespace BST_P {
    template<class T> typename AvlTree<T>::Height AvlTree<T>::Node::FindHeight() const {
        if (IsEmpty()) {
            return 0U;
        }

        Height rightChildHeight = IsRightParent() ? _rightChild->FindHeight() : 0U;
        Height leftChildHeight = IsLeftParent() ? _leftChild->FindHeight() : 0U;
        return 1U + std::max(rightChildHeight, leftChildHeight);
    }

    template<class T> typename AvlTree<T>::Height AvlTree<T>::Node::FindHeight(SubtreeHeightMap & subtreeHeightMap) const {
        if (IsEmpty()) {
            return 0U;
        }

        std::uint64_t key = reinterpret_cast<std::uint64_t>(this);
        auto foundThis = subtreeHeightMap.find(key);

        if (foundThis != subtreeHeightMap.end()) {
            return foundThis->second;
        }

        Height rightChildHeight = IsRightParent() ? _rightChild->FindHeight(subtreeHeightMap) : 0U;
        Height leftChildHeight = IsLeftParent() ? _leftChild->FindHeight(subtreeHeightMap) : 0U;
        Height thisHeight = 1U + std::max(rightChildHeight, leftChildHeight);

        subtreeHeightMap.insert(std::make_pair(key, thisHeight));

        return thisHeight;
    }

    template<class T> void AvlTree<T>::Node::RemoveFromSubtreeHeightMap(SubtreeHeightMap & subtreeHeightMap, bool isAlsoRemovingAncestors) const {
        std::uint64_t key = reinterpret_cast<std::uint64_t>(this);
        subtreeHeightMap.erase(key);

        if (isAlsoRemovingAncestors && _parent) {
            _parent->RemoveFromSubtreeHeightMap(subtreeHeightMap, true);
        }
    }

    template<class T> std::uint64_t AvlTree<T>::Node::ReleaseChildren(bool isCreatingEmptyNode) {
        std::uint64_t releasedHeight = std::max(ReleaseRightChildren(isCreatingEmptyNode), ReleaseLeftChildren(isCreatingEmptyNode));
        _isRequestingFullTreeRebalance = false;
        _balanceFactor = 0;
        return releasedHeight;
    }

    template<class T> std::uint64_t AvlTree<T>::Node::ReleaseRightChildren(bool isCreatingEmptyNode) {
        std::uint64_t releasedHeight = IsRightParent() ? (1U + _rightChild->ReleaseChildren(false)) : 0U;

        if (isCreatingEmptyNode) {
            _rightChild = std::make_shared<Node, AvlTree const &, std::weak_ptr<Node>>(*_tree, _rightChild->_parent);
        } else {
            _rightChild.reset();
        }

        if ((releasedHeight > static_cast<std::uint64_t>(RIGHT_MAX - LEFT_MAX)) || (_balanceFactor > RIGHT_MAX) || (_balanceFactor <= LEFT_MAX)) {
            _isRequestingFullTreeRebalance = true;
        } else {
            BalanceFactor newBalanceFactor = _balanceFactor - static_cast<BalanceFactor>(releasedHeight);
            _isRequestingFullTreeRebalance = newBalanceFactor < LEFT_MAX;

            // only update the balance factor if it's being set to a value we can actually work with.
            _balanceFactor = _isRequestingFullTreeRebalance ? _balanceFactor : newBalanceFactor;
        }

        return releasedHeight;
    }

    template<class T> std::uint64_t AvlTree<T>::Node::ReleaseLeftChildren(bool isCreatingEmptyNode) {
        std::uint64_t releasedHeight = IsLeftParent() ? (1U + _leftChild->ReleaseChildren(false)) : 0U;

        if (isCreatingEmptyNode) {
            _leftChild = std::make_shared<Node, AvlTree<T> const &, std::weak_ptr<Node>>(*_tree, _leftChild->_parent);
        } else {
            _leftChild.reset();
        }

        if ((releasedHeight > static_cast<std::uint64_t>(RIGHT_MAX - LEFT_MAX)) || (_balanceFactor >= RIGHT_MAX) || (_balanceFactor < LEFT_MAX)) {
            _isRequestingFullTreeRebalance = true;
        } else {
            BalanceFactor newBalanceFactor = _balanceFactor + static_cast<BalanceFactor>(releasedHeight);
            _isRequestingFullTreeRebalance = newBalanceFactor > RIGHT_MAX;

            // only update the balance factor if it's being set to a value we can actually work with.
            _balanceFactor = _isRequestingFullTreeRebalance ? _balanceFactor : newBalanceFactor;
        }

        return releasedHeight;
    }

    template<class T> void AvlTree<T>::Node::BeginUpdatingTreePointer(AvlTree const & tree) {
        if (!!_parent) {
            _parent->BeginUpdatingTreePointer(tree);
            return;
        }

        _tree = &tree;

        if (!!_rightChild) {
            _rightChild->ContinueUpdatingTreePointer(tree);
        }

        if (!!_leftChild) {
            _leftChild->ContinueUpdatingTreePointer(tree);
        }
    }

    template<class T> void AvlTree<T>::Node::ContinueUpdatingTreePointer(AvlTree const & tree) {
        if (!!_parent) {
            if (_parent->_tree != &tree) {
                _parent->BeginUpdatingTreePointer(tree);
                return;
            }
        } else {
            BeginUpdatingTreePointer(tree);
            return;
        }

        _tree = &tree;

        if (!!_rightChild) {
            _rightChild->ContinueUpdatingTreePointer(tree);
        }

        if (!!_leftChild) {
            _leftChild->ContinueUpdatingTreePointer(tree);
        }
    }

    template<class T> void AvlTree<T>::Node::ClearSelfAndChildrenIsRequestingFullTreeRebelance() {
        _isRequestingFullTreeRebalance = false;

        if (!!_rightChild) {
            _rightChild->ClearSelfAndChildrenIsRequestingFullTreeRebelance();
        }

        if (!!_leftChild) {
            _leftChild->ClearSelfAndChildrenIsRequestingFullTreeRebelance();
        }
    }

    template<class T> bool AvlTree<T>::__IteratorImpl::Traverse(bool isTraversingLeft) {
        if (_node.expired()) {
            return false;
        }

        std::shared_ptr<Node> node = _node.lock();
        std::shared_ptr<Node> current = node;

        if (isTraversingLeft ? current->IsLeftParent() : current->IsRightParent()) {
            current = isTraversingLeft ? current->_leftChild : current->_rightChild;

            while (isTraversingLeft ? current->IsRightParent() : current->IsLeftParent()) {
                current = isTraversingLeft ? current->_rightChild : current->_leftChild;
            }

            _node = current;
            return true;
        }

        if (isTraversingLeft ? current->IsRightChild() : current->IsLeftChild()) {
            _node = current->_parent;
            return true;
        }

        if (isTraversingLeft ? current->IsLeftChild() : current->IsRightChild()) {
            current = current->_parent;

            while (!!(current->_parent)) {
                if (isTraversingLeft ? current->IsRightChild() : current->IsLeftChild()) {
                    _node = current->_parent;
                    return true;
                }

                current = current->_parent;
            }
        }

        // TraverseRight only - forward iteration is allowed to go "just outside" the chain.
        if (!isTraversingLeft && !(node->IsEmpty()) && !!(node->_rightChild)) {
            _node = node->_rightChild;
            return true;
        }

        return false;
    }

    template<class T> T & AvlTree<T>::MutableIterator::operator*() const {
        if (_impl._node.expired()) {
            throw std::exception{"Cannot dereference null node!"};
        }

        std::shared_ptr<Node> node = _impl._node.lock();

        if (node->IsEmpty()) {
            throw std::exception{"Cannot dereference null data at node!"};
        }

        return *(node->GetData());
    }

    template<class T> T const & AvlTree<T>::ConstIterator::operator*() const {
        if (_impl._node.expired()) {
            throw std::exception{"Cannot dereference null node!"};
        }

        std::shared_ptr<Node> node = _impl._node.lock();

        if (node->IsEmpty()) {
            throw std::exception{"Cannot dereference null data at node!"};
        }

        return *(node->GetData());
    }

    template<class T> T const & AvlTree<T>::NodeTraverser::operator*() const {
        if (_node.expired()) {
            throw std::exception{ "Cannot dereference null node!" };
        }

        std::shared_ptr<Node> node = _node.lock();

        if (node->IsEmpty()) {
            throw std::exception{ "Cannot dereference null data at node!" };
        }

        return *(node->GetData());
    }

    template<class T> bool AvlTree<T>::NodeTraverser::GoToParent() {
        std::shared_ptr<Node> node;

        if (!IsAbleToGoToParent(node)) {
            return false;
        }

        _node = node->_parent;
        return true;
    }

    template<class T> bool AvlTree<T>::NodeTraverser::IsAbleToGoToParent(std::shared_ptr<Node> & node) const {
        if (_node.expired()) {
            return false;
        }

        node = _node.lock();
        return !!(node->_parent);
    }

    template<class T> bool AvlTree<T>::NodeTraverser::GoToChild(bool isTraversingLeft) {
        std::shared_ptr<Node> node;

        if (!IsAbleToGoToChild(isTraversingLeft, node)) {
            return false;
        }

        _node = isTraversingLeft ? node->_leftChild : node->_rightChild;
        return true;
    }

    template<class T> bool AvlTree<T>::NodeTraverser::IsAbleToGoToChild(bool isLookingLeft, std::shared_ptr<Node> & node) const {
        if (_node.expired()) {
            return false;
        }

        node = _node.lock();
        return !(node->IsEmpty()) && ((isLookingLeft && !!(node->_leftChild)) || (!isLookingLeft && !!(node->_rightChild)));
    }

    template<class T> AvlTree<T>::AvlTree(CompareFunctor defaultCompare) : _root{std::make_shared<Node, AvlTree const &>(*this)}, _rightmost{_root}, _leftmost{_root}, _height{0U}, _DefaultCompare{defaultCompare} {
        _root->_rightChild = std::make_shared<Node, AvlTree const &, std::weak_ptr<Node>>(*this, _root);
        _root->_leftChild = std::make_shared<Node, AvlTree const &, std::weak_ptr<Node>>(*this, _root);
    }

    template<class T> AvlTree<T>::AvlTree(AvlTree && other) noexcept
        : _root{std::move(other._root)}
        , _rightmost{std::move(other._rightmost)}
        , _leftmost{std::move(other._leftmost)}
        , _height{other._height}
        , _DefaultCompare{std::move(other._DefaultCompare)}
    {
        if (!!_root) {
            _root->BeginUpdatingTreePointer(*this);
        }

        other._root.reset();
        other._rightmost.reset();
        other._leftmost.reset();
        other._height = 0U;
    }

    template<class T> AvlTree<T> & AvlTree<T>::operator=(AvlTree && other) noexcept {
        if (this != &other) {
            _root = std::move(other._root);
            _rightmost = std::move(other._rightmost);
            _leftmost = std::move(other._leftmost);
            _height = other._height;
            _DefaultCompare = std::move(other._DefaultCompare);

            if (!!_root) {
                _root->BeginUpdatingTreePointer(*this);
            }
        }

        other._root.reset();
        other._rightmost.reset();
        other._leftmost.reset();
        other._height = 0U;

        return *this;
    }

    template<class T> std::weak_ptr<typename AvlTree<T>::Node> AvlTree<T>::FindNodeWithData(const_reference dataToFind, CompareFunctor Compare) const {
        std::shared_ptr<Node> node = _root;

        while (!(node->IsEmpty())) {
            int comparison = Compare(dataToFind, *(node->GetData()));

            if (comparison == 0) {
                return node;
            }

            if (comparison > 0) {
                node = node->_rightChild;
            } else {
                node = node->_leftChild;
            }
        }

        // equivalent to end()
        return _rightmost->GetRightChild();
    }

    template<class T> template<class... Args> std::pair<bool, typename AvlTree<T>::iterator> AvlTree<T>::Emplace(CompareFunctor Compare, Args&&... args) {
        std::shared_ptr<Node> emplaced = std::make_shared<Node, AvlTree const&, std::shared_ptr<Node> const*, Args...>(*this, nullptr, std::forward<Args>(args)...);

        assert(!(emplaced->IsEmpty()));
        if (emplaced->IsEmpty()) {
            return std::make_pair(false, end());
        }

        emplaced->_rightChild = std::make_shared<Node, AvlTree const&, std::weak_ptr<Node>>(*this, emplaced);
        emplaced->_leftChild = std::make_shared<Node, AvlTree const&, std::weak_ptr<Node>>(*this, emplaced);

        if (_height == 0) {
            assert(_root->GetData() == nullptr);
            assert(_rightmost->GetData() == nullptr);
            assert(_leftmost->GetData() == nullptr);

            _root->ReleaseChildren(false);
            _root = emplaced;
            _rightmost = _root;
            _leftmost = _root;
            _height = 1U;

            return std::make_pair(true, iterator{*this, _root});
        }

        std::shared_ptr<Node> current = _root;
        assert(!(current->IsEmpty()));
        assert(!(current->_parent));

        Height heightEmplacedAt = 1U;

        while (!(current->IsEmpty())) {
            int comparison = Compare(*(emplaced->GetData()), *(current->GetData()));

            if (comparison == 0) {
                return std::make_pair(false, iterator{*this, current});
            }

            current = (comparison > 0) ? current->_rightChild : current->_leftChild;
            ++heightEmplacedAt;
        }

        assert(!!(current->_parent));
        bool isLeftChild = current->IsLeftChild();
        current = current->_parent;

        if (isLeftChild) {
            current->_leftChild = emplaced;

            if (_leftmost == current) {
                _leftmost = emplaced;
            }
        } else {
            current->_rightChild = emplaced;

            if (_rightmost == current) {
                _rightmost = emplaced;
            }
        }

        emplaced->_parent = current;
        bool previousIsLeftChild = isLeftChild;

        bool isNotRotated = true;
        do {
            BalanceFactor currentBalanceFactor = current->_balanceFactor + (previousIsLeftChild ? -1 : 1);
            current->_balanceFactor = currentBalanceFactor;
            if (current->IsImbalanced()) {
                if (Rotate(current)) {
                    isNotRotated = false;

                    assert(heightEmplacedAt > 0U);
                    --heightEmplacedAt;

                    break;
                } else {
                    throw std::exception{"Bad rotation in Emplace!"};
                }
            } else if (currentBalanceFactor == 0) {
                // If our parent's balance factor got set to 0 as a result of this insertion, we don't need to update any more balance factors because the parent is now balanced.
                break;
            }

            previousIsLeftChild = current->IsLeftChild();
            current = current->_parent;
        } while (!!current);

        _height = std::max(_height, heightEmplacedAt);
        return std::make_pair(true, iterator{*this, emplaced});
    }

    template<class T> bool AvlTree<T>::Rotate(std::weak_ptr<Node> grandparentWeakptr, SubtreeHeightMap & subtreeHeightMap) {
        if (grandparentWeakptr.expired()) {
            return false;
        }

        std::shared_ptr<Node> grandparent = grandparentWeakptr.lock();

        if (!(grandparent->IsImbalanced())) {
            return false;
        }

        bool isUpperRotationLeft = grandparent->_balanceFactor < 0;

        assert(isUpperRotationLeft ? grandparent->IsLeftParent() : grandparent->IsRightParent());
        if (isUpperRotationLeft ? !(grandparent->IsLeftParent()) : !(grandparent->IsRightParent())) {
            return false;
        }

        std::shared_ptr<Node> parent = isUpperRotationLeft ? grandparent->_leftChild : grandparent->_rightChild;

        // It might seem like we should never have a parent balance factor of 0, but this can happen if we're removing a node.
        bool isLowerRotationLeft = (parent->_balanceFactor == 0) ? isUpperRotationLeft : (parent->_balanceFactor < 0);

        assert(isLowerRotationLeft ? parent->IsLeftParent() : parent->IsRightParent());
        if (isLowerRotationLeft ? !(parent->IsLeftParent()) : !(parent->IsRightParent())) {
            return false;
        }

        std::shared_ptr<Node> child = isLowerRotationLeft ? parent->_leftChild : parent->_rightChild;

        if (isUpperRotationLeft != isLowerRotationLeft) {
            // It's a complex rotation (either LeftRight or RightLeft). Our approach will be to resolve the complexity now and finish with the simple rotation algorithm later.

            // Sanity check, neither `parent` nor `child` should be the root node. It should be impossible for that to happen since we have a grandparent.
            assert(_root != parent);
            assert(_root != child);
            if ((_root == parent) || (_root == child)) {
                return false;
            }

            // Invalidate the subtree heights of every node which will be affected by the following moves.
            child->RemoveFromSubtreeHeightMap(subtreeHeightMap, true);

            // In each case, the child must become the parent, and the parent must become the child.
            if (isLowerRotationLeft) {

                parent->_leftChild = child->_rightChild;
                parent->_leftChild->_parent = parent;

                if (parent->IsRightChild()) {
                    parent->_parent->_rightChild = child;
                } else { // parent->IsLeftChild() is true, because we're not the root node.
                    assert(parent->IsLeftChild());
                    parent->_parent->_leftChild = child;
                }

                child->_parent = parent->_parent;
                child->_rightChild = parent;
                child->_rightChild->_parent = child;
            } else {
                parent->_rightChild = child->_leftChild;
                parent->_rightChild->_parent = parent;

                if (parent->IsRightChild()) {
                    parent->_parent->_rightChild = child;
                } else { // parent->IsLeftChild() is true, because we're not the root node.
                    assert(parent->IsLeftChild());
                    parent->_parent->_leftChild = child;
                }

                child->_parent = parent->_parent;
                child->_leftChild = parent;
                child->_leftChild->_parent = child;
            }

            child = parent;
            parent = child->_parent;

            // This is the only time we need to change the balance factor of the new child.
            Height childRightChildHeight = child->_rightChild->FindHeight(subtreeHeightMap);
            Height childLeftChildHeight = child->_leftChild->FindHeight(subtreeHeightMap);
            if (childRightChildHeight == childLeftChildHeight) {
                child->_balanceFactor = 0;
            } else {
                child->_balanceFactor = (childRightChildHeight > childLeftChildHeight) ? 1 : -1;
            }

            // The new parent's balance factor will be accounted for in the next section.
        }

        // Invalidate the subtree heights of every node which will be affected by the following moves.
        parent->RemoveFromSubtreeHeightMap(subtreeHeightMap, true);

        // The parent must now become the parent of the grandparent.
        if (isUpperRotationLeft) {
            grandparent->_leftChild = parent->_rightChild;
            grandparent->_leftChild->_parent = grandparent;

            if (grandparent->IsRightChild()) {
                grandparent->_parent->_rightChild = parent;
            } else if (grandparent->IsLeftChild()) {
                grandparent->_parent->_leftChild = parent;
            }

            parent->_parent = grandparent->_parent;
            parent->_rightChild = grandparent;
            parent->_rightChild->_parent = parent;
        } else {
            grandparent->_rightChild = parent->_leftChild;
            grandparent->_rightChild->_parent = grandparent;

            if (grandparent->IsRightChild()) {
                grandparent->_parent->_rightChild = parent;
            } else if (grandparent->IsLeftChild()) {
                grandparent->_parent->_leftChild = parent;
            }

            parent->_parent = grandparent->_parent;
            parent->_leftChild = grandparent;
            parent->_leftChild->_parent = parent;
        }

        // Update the root node if we need to. _rightmost and _leftmost are lucky enough to be unaffected by rotations.
        if (_root == grandparent) {
            _root = parent;
        }

        // The parent and grandparent balance factors get accounted for in the same way.
        Height parentRightChildHeight = parent->_rightChild->FindHeight(subtreeHeightMap);
        Height parentLeftChildHeight = parent->_leftChild->FindHeight(subtreeHeightMap);
        if (parentRightChildHeight == parentLeftChildHeight) {
            parent->_balanceFactor = 0;
        } else {
            parent->_balanceFactor = (parentRightChildHeight > parentLeftChildHeight) ? 1 : -1;
        }

        Height grandparentRightChildHeight = grandparent->_rightChild->FindHeight(subtreeHeightMap);
        Height grandparentLeftChildHeight = grandparent->_leftChild->FindHeight(subtreeHeightMap);
        if (grandparentRightChildHeight == grandparentLeftChildHeight) {
            grandparent->_balanceFactor = 0;
        } else {
            grandparent->_balanceFactor = (grandparentRightChildHeight > grandparentLeftChildHeight) ? 1 : -1;
        }

        return true;
    }

    template<class T> bool AvlTree<T>::Remove(iterator && nodeToRemoveItr, std::unique_ptr<value_type> & outputRemovedData) {
        if ((nodeToRemoveItr._impl._tree != this) || (nodeToRemoveItr._impl._node.expired())) {
            return false;
        }

        std::shared_ptr<Node> nodeToRemove = nodeToRemoveItr._impl._node.lock();

        if (!nodeToRemove || (nodeToRemove->_tree != this) || nodeToRemove->IsEmpty()) {
            return false;
        }

        // We still might need a iterator, but now it's time to invalidate the passed-in iterator.
        iterator movedNodeToRemoveItr{std::move(nodeToRemoveItr)};

        // It's also time to move the data to the output variable. We'll still keep the node intact for now, but it won't have any data anymore.
        outputRemovedData = std::move(nodeToRemove->_data);

        // Let's also update our _rightmost and _leftmost nodes while we're at it.
        if (_rightmost == nodeToRemove) {
            assert(!(_rightmost->IsRightParent()));
            _rightmost = _rightmost->IsLeftParent() ? _rightmost->_leftChild : _rightmost->_parent;
        }
        if (_leftmost == nodeToRemove) {
            assert(!(_leftmost->IsLeftParent()));
            _leftmost = _leftmost->IsRightParent() ? _leftmost->_rightChild : _leftmost->_parent;
        }

        // If we have both a right and left child, use the iterator to find a node with 1 or 0 children, then swap data with that node and delete it.
        if (nodeToRemove->IsRightParent() && nodeToRemove->IsLeftParent()) {
            // Since we're a node with a right and left child, traversing once in either direction is guaranteed to get us a node with 1 or 0 children.
            if (nodeToRemove->GetBalanceFactor() > 0) {
                ++movedNodeToRemoveItr;
            } else {
                --movedNodeToRemoveItr;
            }

            assert(!(movedNodeToRemoveItr._impl._node.expired()));
            std::shared_ptr<Node> nodeToSwap = movedNodeToRemoveItr._impl._node.lock();
            assert(!!nodeToSwap && !(nodeToSwap->IsEmpty()));
            assert(!(nodeToSwap->IsRightParent()) || !(nodeToSwap->IsLeftParent()));

            // move the data into the current "node to remove" - we're actually going to remove the node we found with the iterator.
            nodeToRemove->_data = std::move(nodeToSwap->_data);
            nodeToRemove = nodeToSwap;
        }

        // By this point we should be looking at a node that has 1 or 0 children.
        assert(!(nodeToRemove->IsRightParent()) || !(nodeToRemove->IsLeftParent()));

        // One child will stay in the tree, the other will be released.
        std::shared_ptr<Node> child = nodeToRemove->IsLeftParent() ? nodeToRemove->_leftChild : nodeToRemove->_rightChild;
        std::shared_ptr<Node> parent = nodeToRemove->_parent;

        // Even if the child is empty, it shouldn't be null.
        assert(!!child);

        if (_root == nodeToRemove) {
            assert(!parent);

            // We just established `nodeToRemove` has 1 or 0 children. If it's the root node, that means our tree only has 1 or 2 elements in it.
            if (child->IsEmpty()) {
                // If the child is empty, we're the only element in the tree. Reset all pointers.
                assert(_height == 1U);
                _root->ReleaseChildren(true);
                _root->_data.reset();
                _rightmost = _root;
                _leftmost = _root;
                _height = 0U;
            } else {
                // If the child is not empty, it will now become the only element in the tree. Update all pointers.
                assert(_height == 2U);
                _rightmost = child;
                _leftmost = child;
                child->_parent = _root->_parent;
                _root->_rightChild.reset(); // Reminder: All we're doing here is reseting the *removed node's* reference to its children. This should not, in any way, affect `child`, or the new root.
                _root->_leftChild.reset(); // Reminder: All we're doing here is reseting the *removed node's* reference to its children. This should not, in any way, affect `child`, or the new root.
                _root = child;
                _height = 1U;
            }

            return true;
        }

        // Now we've established we're not removing the root node. Therefore, we must have a parent.
        assert(!!parent);

        // The child's parent should become the removed node's parent, and the parent's child should become this child.
        child->_parent = parent;

        bool isRemovedNodeLeftChild = nodeToRemove->IsLeftChild();
        if (isRemovedNodeLeftChild) {
            parent->_leftChild = child;
        } else {
            parent->_rightChild = child;
        }

        // Now let's reset this node's child references (again, shouldn't affect the nodes themselves), and then finally reset this node.
        nodeToRemove->_tree = nullptr;
        // No need to reset _data, it was already reset with the move operation earlier.
        nodeToRemove->_parent.reset();
        nodeToRemove->_rightChild.reset();
        nodeToRemove->_leftChild.reset();
        nodeToRemove.reset();

        // The last step is to deal with balance factor and height. We just deleted a row in a branch of a subtree, so we at least need to update the immediate parent's balance factor.
        SubtreeHeightMap subtreeHeightMap;
        bool previousIsLeftChild = isRemovedNodeLeftChild;
        for (std::shared_ptr<Node> current = std::move(parent); !!current; current = current->_parent) {
            if (current->_balanceFactor == static_cast<BalanceFactor>(0)) {
                // If the current node was at 0 before making adjustments, then we're done. We know this node is not a leaf node, so this can only mean it has another child.
                // Therefore, the tree's height is unaffected, and we can early return.
                current->_balanceFactor = previousIsLeftChild ? 1 : -1;
                return true;
            }

            current->_balanceFactor += previousIsLeftChild ? 1 : -1;

            if (current->IsImbalanced()) {
                // Record the current height of current. If we have a different height after rotation, we need to keep going.
                Height oldCurrentHeight = current->FindHeight(subtreeHeightMap);

                if (Rotate(current, subtreeHeightMap)) {
                    // Unlike with Emplace, we may have to keep going even after rotating once.
                    // Remember, whenever we rotate, the "grandparent" (the node we started the rotation from) is now the child of its former child.
                    // Let's go straight to our new parent so we don't end up with screwy balance factors.
                    current = current->_parent;

                    // We could've just rotated the root node, so make sure we're not about to dereference null. If we would, we're done, so early break.
                    if (!current) {
                        break;
                    }

                    // Now, we compare the current height with the height before rotation. MOST of the time the height has changed, but if it hasn't, there's no need to keep going, we're done.
                    Height newCurrentHeight = current->FindHeight(subtreeHeightMap);
                    if (newCurrentHeight == oldCurrentHeight) {
                        break;
                    }

                    // If we've somehow increased height after a rotation, that's Bad News (TM).
                    assert(newCurrentHeight < oldCurrentHeight);
                    if (newCurrentHeight > oldCurrentHeight) {
                        throw std::exception{"Increased height after rotation, what's going on???"};
                    }
                } else {
                    throw std::exception{"Bad rotate in Remove!"};
                }
            }

            previousIsLeftChild = current->IsLeftChild();
        }

        // If by the end of all of this, the root node's balance factor is 0, then we must've lost a row of height.
        if (_root->_balanceFactor == 0) {
            assert(_height > 0U);
            --_height;
        }

        return true;
    }
}
