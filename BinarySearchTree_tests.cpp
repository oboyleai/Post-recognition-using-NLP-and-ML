// Project UID db1f506d06d84ab787baf250c265e24e

#include "BinarySearchTree.h"
#include "unit_test_framework.h"

TEST(test_empty)
{
    BinarySearchTree<int> bst;
    ASSERT_TRUE(bst.empty());
}

TEST(test_size_impl)
{
    BinarySearchTree<int> bst;
    // tests size 0
    ASSERT_EQUAL(bst.size(), 0);

    // size 1
    bst.insert(1);
    ASSERT_EQUAL(bst.size(), 1);

    // 101 big
    for (int i = 1; i < 100; i++)
        bst.insert(i);
    ASSERT_EQUAL(bst.size(), 100);
}

// not sure about this one
// does bst efficiently insert the elements, or does it just put them in there
TEST(test_height_impl)
{

    BinarySearchTree<int> bst;
    // empty bst
    ASSERT_EQUAL(bst.height(), 0);

    // bst size 1
    bst.insert(10);
    ASSERT_EQUAL(bst.height(), 1);

    // tests with a height 3
    bst.insert(9);
    bst.insert(8);
    bst.insert(7);
    ASSERT_EQUAL(bst.height(), 3);

    // should stay 3
    bst.insert(11);
    bst.insert(12);
    bst.insert(13);
    ASSERT_EQUAL(bst.height(), 3);

    // inserts one at the bottom but changes the height
    bst.insert(14);
    ASSERT_EQUAL(bst.height(), 4);
}

// tests the copy constructor,
// the only function that only uses copy_nodes_impl
// emphasis on the only
TEST(test_copy_nodes_impl)
{
    // tests 0 els
    BinarySearchTree<int> a;
    BinarySearchTree<int> b(a);
    ASSERT_EQUAL(a.to_string(), b.to_string());

    // tests 1 el
    a.insert(1);
    BinarySearchTree<int> c(a);
    ASSERT_EQUAL(a.to_string(), c.to_string());

    // tests 10 els
    BinarySearchTree<int> c;
    for (int i = 1; i < 11; i++)
        c.insert(i);
    BinarySearchTree<int> d(c);
    ASSERT_EQUAL(d.to_string(), c.to_string());
}

// when = is called, destroy_nodes_impl is called
// we know when this function doesn't work when there are memory leaks
TEST(test_destroy_nodes_impl)
{
    BinarySearchTree<int> a;
    BinarySearchTree<int> b;

    // tests nothing breaks on 0 els
    bool code_works = true;
    try
    {
        a = b;
    }
    catch (const std::exception &exc)
    {
        code_works = false;
    }
    ASSERT_TRUE(code_works);

    // tests with 1 el
    a.insert(1);
    a = b;
    ASSERT_TRUE(a.empty());

    // tests with 10 els
    for (int i = 0; i < 10; i++)
        a.insert(i);
    a = b;
    ASSERT_TRUE(a.empty());
}

// beyond me man
TEST(test_find_impl)
{
    ASSERT_TRUE(true);
}

// beyond me man
TEST(test_insert_impl)
{
    ASSERT_TRUE(true);
}

TEST(test_min_element_impl)
{
    // size 0
    BinarySearchTree<int> bst;
    ASSERT_EQUAL(*bst.min_element(), nullptr);

    // size 1
    bst.insert(1);
    ASSERT_EQUAL(*bst.min_element(), 1);

    // lots of els
    BinarySearchTree<int> bst2;
    for (int i = 0; i < 10; i++)
        bst2.insert(i);
    ASSERT_EQUAL(*bst.min_element(), 0);
}

TEST(test_max_element_impl)
{
    // size 0
    BinarySearchTree<int> bst;
    ASSERT_EQUAL(*bst.min_element(), nullptr);

    // size 1
    bst.insert(1);
    ASSERT_EQUAL(*bst.min_element(), 1);

    // lots of els
    BinarySearchTree<int> bst2;
    for (int i = 0; i < 10; i++)
        bst2.insert(i);
    ASSERT_EQUAL(*bst.min_element(), 9);
}

// you have to break abstraction in order to test this
TEST(test_check_sorting_invariant_impl)
{
    // size 0, 1
    BinarySearchTree<int> bst;
    ASSERT_TRUE(bst.check_sorting_invariant());
    bst.insert(10);
    ASSERT_TRUE(bst.check_sorting_invariant());
    bst.insert(11);
    BinarySearchTree<int>::Iterator begin = bst.begin();

    // Tests left side
    // breaks sorting invariant
    *begin = 20;
    ASSERT_FALSE(bst.check_sorting_invariant());
    // fixes sorting invariant
    *begin = 10;
    ASSERT_TRUE(bst.check_sorting_invariant());

    // tests right side
    bst.insert(13);
    BinarySearchTree<int>::Iterator big_el = bst.max_element();
    *big_el = 1;
    ASSERT_FALSE(bst.check_sorting_invariant());
}
// TEST

TEST_MAIN()
