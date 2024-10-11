#include <iostream>
#include "AVLTree.h"

void TestAvlTree() {
    BST_P::AvlTree<int> tree;

    std::cout << "Expected height: 0, Actual height: " << tree.GetHeight() << "\n";

    tree.Insert(5);

    std::cout << "Expected height: 1, Actual height: " << tree.GetHeight() << "\n";

    auto result = tree.Insert(5);

    std::cout << "Expected bool: false, Actual bool: " << (result.first ? "true" : "false") << "\n";

    result = tree.Insert(1);

    std::cout << "Expected bool: true, Actual bool: " << (result.first ? "true" : "false") << "\n";

    for (auto itr = tree.begin(); itr != tree.end(); ++itr) {
        std::cout << "[" << (*itr) << "] ";
    }

    std::cout << "\n";

    result = tree.Insert(7);

    std::cout << "Expected bool: true, Actual bool: " << (result.first ? "true" : "false") << "\n";

    for (auto itr = tree.begin(); itr != tree.end(); ++itr) {
        std::cout << "[" << (*itr) << "] ";
    }

    std::cout << "\n";

    result = tree.Insert(3);

    std::cout << "Expected bool: true, Actual bool: " << (result.first ? "true" : "false") << "\n";

    for (auto itr = tree.begin(); itr != tree.end(); ++itr) {
        std::cout << "[" << (*itr) << "] ";
    }

    std::cout << "\n";

    result = tree.Insert(6);

    std::cout << "Expected bool: true, Actual bool: " << (result.first ? "true" : "false") << "\n";

    for (auto itr = tree.begin(); itr != tree.end(); ++itr) {
        std::cout << "[" << (*itr) << "] ";
    }

    std::cout << "\n";

    result = tree.Insert(9);

    std::cout << "Expected bool: true, Actual bool: " << (result.first ? "true" : "false") << "\n";

    for (auto itr = tree.end(); --itr != tree.begin();) {
        std::cout << "[" << (*itr) << "] ";
    }

    std::cout << "[" << *(tree.begin()) << "] \n";
}

void TestAvlTreeAgain() {
    BST_P::AvlTree<int> tree;

    std::cout << "Expected height: 0, Actual height: " << tree.GetHeight() << "\n";

    tree.Insert(1);

    std::cout << "Expected height: 1, Actual height: " << tree.GetHeight() << "\n";

    for (auto itr = tree.begin(); itr != tree.end(); ++itr) {
        std::cout << "[" << (*itr) << "] ";
    }

    std::cout << "\n";

    tree.Insert(2);

    std::cout << "Expected height: 2, Actual height: " << tree.GetHeight() << "\n";

    for (auto itr = tree.begin(); itr != tree.end(); ++itr) {
        std::cout << "[" << (*itr) << "] ";
    }

    std::cout << "\n";

    tree.Insert(3);

    std::cout << "Expected height: 2, Actual height: " << tree.GetHeight() << "\n";

    for (auto itr = tree.begin(); itr != tree.end(); ++itr) {
        std::cout << "[" << (*itr) << "] ";
    }

    std::cout << "\n";

    tree.Insert(4);

    std::cout << "Expected height: 3, Actual height: " << tree.GetHeight() << "\n";

    for (auto itr = tree.begin(); itr != tree.end(); ++itr) {
        std::cout << "[" << (*itr) << "] ";
    }

    std::cout << "\n";

    tree.Insert(5);

    std::cout << "Expected height: 3, Actual height: " << tree.GetHeight() << "\n";

    for (auto itr = tree.begin(); itr != tree.end(); ++itr) {
        std::cout << "[" << (*itr) << "] ";
    }

    std::cout << "\n";

    tree.Insert(6);

    std::cout << "Expected height: 3, Actual height: " << tree.GetHeight() << "\n";

    for (auto itr = tree.begin(); itr != tree.end(); ++itr) {
        std::cout << "[" << (*itr) << "] ";
    }

    std::cout << "\n";

    tree.Insert(7);

    std::cout << "Expected height: 3, Actual height: " << tree.GetHeight() << "\n";

    for (auto itr = tree.begin(); itr != tree.end(); ++itr) {
        std::cout << "[" << (*itr) << "] ";
    }

    std::cout << "\n";

    tree.Insert(8);

    std::cout << "Expected height: 4, Actual height: " << tree.GetHeight() << "\n";

    for (auto itr = tree.begin(); itr != tree.end(); ++itr) {
        std::cout << "[" << (*itr) << "] ";
    }

    std::cout << "\n";
}

void TestAvlTreeFromWikipedia() {
    BST_P::AvlTree<char> tree;

    tree.Insert('M');
    tree.Insert('N');
    tree.Insert('O');
    tree.Insert('L');
    tree.Insert('K');
    tree.Insert('Q');
    tree.Insert('P');
    tree.Insert('H');
    tree.Insert('I');
    tree.Insert('A');

    std::cout << "Expected height: 4, Actual height: " << tree.GetHeight() << "\n";

    for (auto itr = tree.begin(); itr != tree.end(); ++itr) {
        std::cout << "[" << (*itr) << "] ";
    }

    std::cout << "\n";
}

void TestAvlTreeRemoveAndEmplace() {
    BST_P::AvlTree<int> tree;

    tree.Insert(1);
    tree.Insert(2);

    std::cout << "Expected height: 2, Actual height: " << tree.GetHeight() << "\n";

    for (auto itr = tree.begin(); itr != tree.end(); ++itr) {
        std::cout << "[" << (*itr) << "] ";
    }

    std::cout << "\nAttempting to remove 3. Expected result: false, Actual result: " << (tree.Remove(3) ? "true" : "false") << "\n";

    std::cout << "Expecting no changes from the previous prints...\nExpected height: 2, Actual height: " << tree.GetHeight() << "\n";

    for (auto itr = tree.begin(); itr != tree.end(); ++itr) {
        std::cout << "[" << (*itr) << "] ";
    }

    std::cout << "\nAttempting to remove 1. Expected result: true, Actual result: " << (tree.Remove(1) ? "true" : "false") << "\n";

    std::cout << "Expected height: 1, Actual height: " << tree.GetHeight() << "\n";

    for (auto itr = tree.begin(); itr != tree.end(); ++itr) {
        std::cout << "[" << (*itr) << "] ";
    }

    std::cout << "\nAttempting to remove 2. Expected result: true, Actual result: " << (tree.Remove(2) ? "true" : "false") << "\n";

    std::cout << "Expected height: 0, Actual height: " << tree.GetHeight() << "\nExpecting an empty list below...\n";

    for (auto itr = tree.begin(); itr != tree.end(); ++itr) {
        std::cout << "[" << (*itr) << "] ";
    }

    std::cout << "\n";

    tree.Insert(1);
    tree.Insert(2);
    tree.Insert(4);
    tree.Insert(8);
    tree.Insert(0);

    tree.Insert(3);
    tree.Insert(5);
    tree.Insert(7);
    tree.Insert(9);
    tree.Insert(6);

    std::cout << "Expected height: 4, Actual height: " << tree.GetHeight() << "\n";

    for (auto itr = tree.begin(); itr != tree.end(); ++itr) {
        std::cout << "[" << (*itr) << "] ";
    }

    std::cout << "\n";

    auto eitr = tree.end();
    for (--eitr; eitr != tree.begin(); --eitr) {
        std::cout << "[" << (*eitr) << "] ";
    }

    std::cout << "[" << (*eitr) << "] \n";

    tree.Insert(10);
    tree.Insert(11);
    tree.Insert(12);
    tree.Insert(13);
    tree.Insert(14);
    tree.Insert(100);

    std::cout << "Expected height: 5, Actual height: " << tree.GetHeight() << "\n";

    for (auto itr = tree.begin(); itr != tree.end(); ++itr) {
        std::cout << "[" << (*itr) << "] ";
    }

    std::cout << "\n";

    eitr = tree.end();
    for (--eitr; eitr != tree.begin(); --eitr) {
        std::cout << "[" << (*eitr) << "] ";
    }

    std::cout << "[" << (*eitr) << "] \n";

    tree.Remove(3);

    std::cout << "Expected height: 5, Actual height: " << tree.GetHeight() << "\n";

    for (auto itr = tree.begin(); itr != tree.end(); ++itr) {
        std::cout << "[" << (*itr) << "] ";
    }

    std::cout << "\n";

    eitr = tree.end();
    for (--eitr; eitr != tree.begin(); --eitr) {
        std::cout << "[" << (*eitr) << "] ";
    }

    std::cout << "[" << (*eitr) << "] \n";

    tree.Remove(12);

    std::cout << "Expected height: 4, Actual height: " << tree.GetHeight() << "\n";

    for (auto itr = tree.begin(); itr != tree.end(); ++itr) {
        std::cout << "[" << (*itr) << "] ";
    }

    std::cout << "\n";

    eitr = tree.end();
    for (--eitr; eitr != tree.begin(); --eitr) {
        std::cout << "[" << (*eitr) << "] ";
    }

    std::cout << "[" << (*eitr) << "] \n";
}
