#pragma once
#include <cassert>
#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>

namespace BST_P {
    template<class T>
    struct subtract {
        int operator()(T const & a, T const & b) const { return a - b; }
    };

    template<class T>
    class AvlTree final {
    public:
        class MutableIterator;
        class ConstIterator;
        class NodeTraverser;

        friend class Node;
        friend class __IteratorImpl;

        typedef std::uint64_t Height;
        typedef std::unordered_map<std::uint64_t, Height> SubtreeHeightMap;
        typedef std::int8_t BalanceFactor;
        static const BalanceFactor LEFT_IMBALANCE = -2;
        static const BalanceFactor LEFT_MAX = -1;
        static const BalanceFactor RIGHT_MAX = 1;
        static const BalanceFactor RIGHT_IMBALANCE = 2;

    private:
        class Node final {
        public:
            friend AvlTree;
            friend class BST_P::AvlTree<T>::__IteratorImpl;

            Node() = delete;
            Node(Node const &) = delete;
            Node(Node &&) noexcept = delete;
            Node & operator=(Node const &) = delete;
            Node & operator=(Node &&) noexcept = delete;
            inline ~Node() { ReleaseChildren(false); }

            template<class... Args> inline explicit Node(AvlTree const & tree, std::weak_ptr<Node> parent, Args&&... args)
                : _tree{&tree}
                , _data{std::make_unique<T>(std::forward<Args>(args)...)}
                , _parent{std::move(parent.lock())}
                , _isRequestingFullTreeRebalance{false}
                , _balanceFactor{0} {}
            template<class... Args> inline explicit Node(AvlTree const & tree, std::shared_ptr<Node> const * parent, Args&&... args)
                : Node{tree, (parent == nullptr) ? std::weak_ptr<Node>{} : *parent, std::forward<Args>(args)...} {}
            inline explicit Node(AvlTree const & tree, std::weak_ptr<Node> parent)
                : _tree{&tree}
                , _data{nullptr}
                , _parent{std::move(parent.lock())}
                , _isRequestingFullTreeRebalance{false}
                , _balanceFactor{0} {}
            inline explicit Node(AvlTree const & tree, std::shared_ptr<Node> const * parent) : Node{tree, (parent == nullptr) ? std::weak_ptr<Node>{} : *parent} {}
            inline explicit Node(AvlTree const & tree) : Node{tree, std::weak_ptr<Node>{}} {}

            [[nodiscard]] inline bool IsNodeOfTree(AvlTree const * const tree) const { return _tree == tree; }
            [[nodiscard]] inline T const * const GetData() const { return _data.get(); }
            inline T * const GetData() { return _data.get(); }
            [[nodiscard]] inline bool IsEmpty() const { return _data.get() == nullptr; }
            [[nodiscard]] inline BalanceFactor GetBalanceFactor() const { return _balanceFactor; }
            [[nodiscard]] inline bool IsImbalanced() const { return (_balanceFactor >= RIGHT_IMBALANCE) || (_balanceFactor <= LEFT_IMBALANCE); }
            [[nodiscard]] inline bool IsRightParent() const { return !!_rightChild && !(_rightChild->IsEmpty()); }
            [[nodiscard]] inline bool IsLeftParent() const { return !!_leftChild && !(_leftChild->IsEmpty()); }
            [[nodiscard]] inline bool IsRightChild() const { return !!_parent && (_parent->_rightChild.get() == this); }
            [[nodiscard]] inline bool IsLeftChild() const { return !!_parent && (_parent->_leftChild.get() == this); }
            [[nodiscard]] inline std::weak_ptr<Node> GetParent() const { return _parent; }
            [[nodiscard]] inline std::weak_ptr<Node> GetRightChild() const { return _rightChild; }
            [[nodiscard]] inline std::weak_ptr<Node> GetLeftChild() const { return _leftChild; }
            [[nodiscard]] inline bool IsRequestingFullTreeRebalance() const { return _isRequestingFullTreeRebalance; }

            [[nodiscard]] Height FindHeight() const;

        private:
            [[nodiscard]] Height FindHeight(SubtreeHeightMap & subtreeHeightMap) const;
            void RemoveFromSubtreeHeightMap(SubtreeHeightMap & subtreeHeightMap, bool isAlsoRemovingAncestors = false) const;

            std::uint64_t ReleaseChildren(bool isCreatingEmptyNode);
            std::uint64_t ReleaseRightChildren(bool isCreatingEmptyNode);
            std::uint64_t ReleaseLeftChildren(bool isCreatingEmptyNode);

            void BeginUpdatingTreePointer(AvlTree const & tree);
            void ContinueUpdatingTreePointer(AvlTree const & tree);
            void ClearSelfAndChildrenIsRequestingFullTreeRebelance();

            AvlTree const * _tree;
            std::unique_ptr<T> _data;
            std::shared_ptr<Node> _parent;
            std::shared_ptr<Node> _rightChild;
            std::shared_ptr<Node> _leftChild;
            bool _isRequestingFullTreeRebalance;
            BalanceFactor _balanceFactor;
        };

        class __IteratorImpl final {
            friend AvlTree;
            friend MutableIterator;
            friend ConstIterator;

        public:
            typedef std::ptrdiff_t difference_type;
            typedef T value_type;
            typedef std::bidirectional_iterator_tag iterator_category;
            typedef value_type const * const_pointer;
            typedef value_type const & const_reference;

            __IteratorImpl(__IteratorImpl const &) = default;
            __IteratorImpl(__IteratorImpl &&) noexcept = default;
            __IteratorImpl & operator=(__IteratorImpl const &) = default;
            __IteratorImpl & operator=(__IteratorImpl &&) noexcept = default;
            inline ~__IteratorImpl() = default;

            inline bool TraverseRight() { return Traverse(false); }
            inline bool TraverseLeft() { return Traverse(true); }
            [[nodiscard]] inline bool IsEqualTo(__IteratorImpl const & other) const {
                bool isExpired = _node.expired();
                bool isTreePtrEqual = (_tree == other._tree);
                bool isExpiredEqual = (isExpired == other._node.expired());
                bool isSharedPtrEqual = (isExpired || (_node.lock() == other._node.lock()));
                return isTreePtrEqual && isExpiredEqual && isSharedPtrEqual;
            }

        private:
            inline explicit __IteratorImpl(AvlTree const & tree, std::weak_ptr<Node> node) : _tree{&tree}, _node{node} {}

            bool Traverse(bool isTraversingLeft);

            AvlTree const * _tree;
            std::weak_ptr<Node> _node;
        };

    public:
        class MutableIterator final {
            friend AvlTree;
            friend ConstIterator;

        public:
            typedef std::ptrdiff_t difference_type;
            typedef T value_type;
            typedef value_type * pointer;
            typedef value_type & reference;
            typedef std::bidirectional_iterator_tag iterator_category;
            typedef value_type const * const_pointer;
            typedef value_type const & const_reference;

            inline explicit MutableIterator(AvlTree const & tree, std::weak_ptr<Node> node) : _impl{tree, node} {}
            inline MutableIterator(MutableIterator const &) = default;
            inline MutableIterator(MutableIterator &&) noexcept  = default;
            inline MutableIterator & operator=(MutableIterator const &) = default;
            inline MutableIterator & operator=(MutableIterator &&) noexcept = default;
            inline ~MutableIterator() = default;

            inline explicit MutableIterator(ConstIterator const & other) : _impl{other._impl} {}
            inline explicit MutableIterator(ConstIterator&& other) : _impl{std::move(other._impl)} {}

            [[nodiscard]] inline bool operator==(MutableIterator const & other) const { return _impl.IsEqualTo(other._impl); }
            [[nodiscard]] inline bool operator!=(MutableIterator const & other) const { return !operator==(other); }
            [[nodiscard]] inline bool operator==(ConstIterator const & other) const { return _impl.IsEqualTo(other._impl); }
            [[nodiscard]] inline bool operator!=(ConstIterator const & other) const { return !operator==(other); }
            inline pointer const operator->() const { return _impl._node.expired() ? nullptr : _impl._node.lock()->GetData(); }
            inline MutableIterator& operator++() { _impl.TraverseRight(); return *this; }
            [[nodiscard]] inline MutableIterator operator++(int) { MutableIterator copy{*this}; operator++(); return copy; }
            inline MutableIterator& operator--() { _impl.TraverseLeft(); return *this; }
            [[nodiscard]] inline MutableIterator operator--(int) { MutableIterator copy{*this}; operator--(); return copy; }

            [[nodiscard]] reference operator*() const;

        private:
            __IteratorImpl _impl;
        };

        class ConstIterator final {
            friend AvlTree;
            friend MutableIterator;

        public:
            typedef std::ptrdiff_t difference_type;
            typedef T value_type;
            typedef value_type const * pointer;
            typedef value_type const & reference;
            typedef std::bidirectional_iterator_tag iterator_category;
            typedef value_type * const_pointer;
            typedef value_type & const_reference;

            inline explicit ConstIterator(AvlTree const & tree, std::weak_ptr<Node> node) : _impl{tree, node} {}
            inline ConstIterator(ConstIterator const &) = default;
            inline ConstIterator(ConstIterator &&) noexcept = default;
            inline ConstIterator & operator=(ConstIterator const &) = default;
            inline ConstIterator & operator=(ConstIterator &&) noexcept = default;
            inline ~ConstIterator() = default;

            inline explicit ConstIterator(MutableIterator const & other) : _impl{other._impl} {}
            inline explicit ConstIterator(MutableIterator && other) : _impl{std::move(other._impl)} {}

            [[nodiscard]] inline bool operator==(ConstIterator const & other) const { return _impl.IsEqualTo(other._impl); }
            [[nodiscard]] inline bool operator!=(ConstIterator const & other) const { return !operator==(other); }
            [[nodiscard]] inline bool operator==(MutableIterator const & other) const { return _impl.IsEqualTo(other._impl); }
            [[nodiscard]] inline bool operator!=(MutableIterator const & other) const { return !operator==(other); }
            inline pointer const operator->() const { return _impl._node.expired() ? nullptr : _impl._node.lock()->GetData(); }
            inline ConstIterator& operator++() { _impl.TraverseRight(); return *this; }
            [[nodiscard]] inline ConstIterator operator++(int) { ConstIterator copy{*this}; operator++(); return copy; }
            inline ConstIterator& operator--() { _impl.TraverseLeft(); return *this; }
            [[nodiscard]] inline ConstIterator operator--(int) { ConstIterator copy{*this}; operator--(); return copy; }

            [[nodiscard]] reference operator*() const;

        private:
            __IteratorImpl _impl;
        };

        class NodeTraverser final {
            friend AvlTree;

        public:
            NodeTraverser(NodeTraverser const&) = default;
            NodeTraverser(NodeTraverser &&) noexcept = default;
            NodeTraverser & operator=(NodeTraverser const &) = default;
            NodeTraverser & operator=(NodeTraverser &&) noexcept = default;
            inline ~NodeTraverser() = default;

            [[nodiscard]] inline bool operator==(NodeTraverser const & other) const {
                bool isExpired = _node.expired();
                bool isTreePtrEqual = (_tree == other._tree);
                bool isExpiredEqual = (isExpired == other._node.expired());
                bool isSharedPtrEqual = (isExpired || (_node.lock() == other._node.lock()));
                return isTreePtrEqual && isExpiredEqual && isSharedPtrEqual;
            }
            [[nodiscard]] inline bool operator!=(NodeTraverser const & other) const { return !operator==(other); }
            inline T const * operator->() const { return _node.expired() ? nullptr : _node.lock()->GetData(); }

            [[nodiscard]] T const & operator*() const;

            [[nodiscard]] inline bool IsNotEmpty() const { return !(_node.expired()) && !(_node.lock()->IsEmpty()); }
            [[nodiscard]] inline bool operator!() const { return !IsNotEmpty(); }
            [[nodiscard]] inline explicit operator bool() const { return !operator!(); }

            bool GoToParent();
            [[nodiscard]] inline bool IsAbleToGoToParent() const { std::shared_ptr<Node> _; return IsAbleToGoToParent(_); }

            inline bool GoToRightChild() { return GoToChild(false); }
            inline bool GoToLeftChild() { return GoToChild(true); }
            [[nodiscard]] inline bool IsAbleToGoToRightChild() { return IsAbleToGoToChild(false); }
            [[nodiscard]] inline bool IsAbleToGoToLeftChild() { return IsAbleToGoToChild(true); }

        private:
            inline explicit NodeTraverser(AvlTree const & tree, std::weak_ptr<Node> node) : _tree{&tree}, _node{node} {}

            [[nodiscard]] bool GoToChild(bool isTraversingLeft);
            [[nodiscard]] bool IsAbleToGoToParent(std::shared_ptr<Node> & outputNode) const;
            [[nodiscard]] bool IsAbleToGoToChild(bool isLookingLeft, std::shared_ptr<Node> & outputNode) const;
            [[nodiscard]] inline bool IsAbleToGoToChild(bool isLookingLeft) const { std::shared_ptr<Node> _; return IsAbleToGoToChild(isLookingLeft, _); }

            AvlTree const * _tree;
            std::weak_ptr<Node> _node;
        };

        typedef T value_type;
        typedef value_type * pointer;
        typedef value_type & reference;
        typedef MutableIterator iterator;
        typedef value_type const * const_pointer;
        typedef value_type const & const_reference;
        typedef ConstIterator const_iterator;
        typedef std::function<int(const_reference, const_reference)> CompareFunctor;

        explicit AvlTree(CompareFunctor defaultCompare = subtract<value_type>{});
        AvlTree(AvlTree const &) = delete;
        AvlTree(AvlTree &&) noexcept;
        AvlTree & operator=(AvlTree const &) = delete;
        AvlTree & operator=(AvlTree &&) noexcept;
        inline ~AvlTree() { if (!!_root) { _root->ReleaseChildren(false); } }

        [[nodiscard]] inline iterator begin() { return iterator{cbegin()}; }
        [[nodiscard]] inline const_iterator cbegin() const { return const_iterator{*this, _leftmost}; }
        [[nodiscard]] inline const_iterator begin() const { return cbegin(); }
        [[nodiscard]] inline iterator end() { return iterator{cend()}; }
        [[nodiscard]] inline const_iterator cend() const { return const_iterator{*this, (_height == 0U) ? _root : _rightmost->GetRightChild()}; }
        [[nodiscard]] inline const_iterator end() const { return cend(); }
        [[nodiscard]] inline iterator Find(const_reference dataToFind) { return iterator{cFind(dataToFind)}; }
        [[nodiscard]] inline const_iterator cFind(const_reference dataToFind) const { return const_iterator{*this, FindNodeWithData(dataToFind, _DefaultCompare)}; }
        [[nodiscard]] inline const_iterator Find(const_reference dataToFind) const { return cFind(dataToFind); }
        [[nodiscard]] inline iterator Find(const_reference dataToFind, CompareFunctor specializedCompareFunctor) { return iterator{cFind(dataToFind, specializedCompareFunctor)}; }
        [[nodiscard]] inline const_iterator cFind(const_reference dataToFind, CompareFunctor specializedCompareFunctor) const {
            return const_iterator{*this, FindNodeWithData(dataToFind, specializedCompareFunctor)};
        }
        [[nodiscard]] inline const_iterator Find(const_reference dataToFind, CompareFunctor specializedCompareFunctor) const { return cFind(dataToFind, specializedCompareFunctor); }

        [[nodiscard]] inline NodeTraverser CreateNodeTraverser() const { return NodeTraverser{*this, _root}; }
        [[nodiscard]] inline NodeTraverser CreateNodeTraverser(iterator const & itr) const { return NodeTraverser{*(itr._impl._tree), itr._impl._node}; }
        [[nodiscard]] inline NodeTraverser CreateNodeTraverser(const_iterator const & itr) const { return NodeTraverser{*(itr._impl._tree), itr._impl._node}; }

        [[nodiscard]] inline Height GetHeight() const { return _height; }

        CompareFunctor const & GetDefaultCompare() const { return _DefaultCompare; }

        template<class... Args> std::pair<bool, iterator> Emplace(CompareFunctor emplaceCompareFunctor, Args&&...);
        template<class... Args> inline std::pair<bool, iterator> DefaultEmplace(Args&&... args) { return Emplace(_DefaultCompare, std::forward<Args>(args)...); }
        inline std::pair<bool, iterator> Insert(const_reference dataToCopyAndInsert, CompareFunctor specializedInsertionCompareFunctor) {
            return Emplace(specializedInsertionCompareFunctor, dataToCopyAndInsert);
        }
        inline std::pair<bool, iterator> Insert(const_reference dataToCopyAndInsert) { return Emplace(_DefaultCompare, dataToCopyAndInsert); }
        inline std::pair<bool, iterator> Insert(value_type && dataToMoveAndInsert, CompareFunctor specializedInsertionCompareFunctor) {
            return Emplace(specializedInsertionCompareFunctor, std::move(dataToMoveAndInsert));
        }
        inline std::pair<bool, iterator> Insert(value_type && dataToMoveAndInsert) { return Emplace(_DefaultCompare, std::move(dataToMoveAndInsert)); }

        bool Remove(iterator && nodeToRemove, std::unique_ptr<value_type> & outputRemovedData);
        inline bool Remove(iterator && nodeToRemove) { std::unique_ptr<value_type> _; return Remove(std::move(nodeToRemove), _); }
        inline bool Remove(const_reference dataToRemove, std::unique_ptr<value_type> & outputRemovedData, CompareFunctor specializedCompareFunctor) {
            return Remove(Find(dataToRemove, specializedCompareFunctor), outputRemovedData);
        }
        inline bool Remove(const_reference dataToRemove, CompareFunctor specializedCompareFunctor) { std::unique_ptr<value_type> _; return Remove(dataToRemove, _, specializedCompareFunctor); }
        inline bool Remove(const_reference dataToRemove, std::unique_ptr<value_type> & outputRemovedData) { return Remove(dataToRemove, outputRemovedData, _DefaultCompare); }
        inline bool Remove(const_reference dataToRemove) { std::unique_ptr<value_type> _; return Remove(dataToRemove, _, _DefaultCompare); }

    private:
        std::weak_ptr<Node> FindNodeWithData(const_reference dataToFind, CompareFunctor Compare) const;
        bool Rotate(std::weak_ptr<Node> grandparent, SubtreeHeightMap & subtreeHeightMap);

        inline bool Rotate(std::weak_ptr<Node> grandparent) { SubtreeHeightMap _; return Rotate(grandparent, _); }

        std::shared_ptr<Node> _root;
        std::shared_ptr<Node> _rightmost;
        std::shared_ptr<Node> _leftmost;
        Height _height;

        CompareFunctor _DefaultCompare;
    };
}

#include "AVLTree.inl"
