#include "TestAVLTree.h"
#include "TreeTool.h"
#include <iostream>

void rapid::test_AVLTree_main()
{
    std::cout << "------------" << __func__ << "------------" << std::endl;
    AVLTree<int> avl;
    avl.insert({50, 45, 40, 48, 39, 43, 47, 49, 38, 42, 44, 46});
    avl.insert({100, 90, 200, 60, 95, 199, 202, 80, 94, 96, 201, 300});
    std::cout << "former iterator" << std::endl;
    for(auto it = avl.fbegin(); it != avl.fend(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    std::cout << "middle iterator" << std::endl;
    for(int i : avl)
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    std::cout << "after iterator" << std::endl;
    for(auto it = avl.abegin(); it != avl.aend(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
#ifdef QT_LIB
    MainWindow<int, AVLTree<int>> mw1(avl.to_ordinary_tree(),
                        [](const BTreeNode<int> *node) { return std::to_string(node->data()); },
                        [](const BTreeNode<int> *) { return 1; }
    );
    mw1.show();
#endif
    std::cout << "---------------erase-------------" << std::endl;
    avl.erase(100);
    std::cout << "former iterator" << std::endl;
    for(auto it = avl.fbegin(); it != avl.fend(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    std::cout << "middle iterator" << std::endl;
    for(int i : avl)
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    std::cout << "after iterator" << std::endl;
    for(auto it = avl.abegin(); it != avl.aend(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    std::cout << "---------------------------------" << std::endl;
    avl.erase(48);
    std::cout << "former iterator" << std::endl;
    for(auto it = avl.fbegin(); it != avl.fend(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    std::cout << "middle iterator" << std::endl;
    for(int i : avl)
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    std::cout << "after iterator" << std::endl;
    for(auto it = avl.abegin(); it != avl.aend(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    std::cout << "------------end------------" << std::endl;
#ifdef QT_LIB
    app->exec();
#endif
}
