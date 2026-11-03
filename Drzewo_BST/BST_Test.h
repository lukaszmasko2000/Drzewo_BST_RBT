#pragma once
#include "catch_amalgamated.hpp"
#include <vector>
#include <string>
#include "BST_Tree.h"



TEST_CASE("BST - Stan poczatkowy", "[BST][basic]") {
    BST<int> bst;

    SECTION("Nowe drzewo jest puste") {
        REQUIRE(bst.height() == 0);
        REQUIRE(bst.pre_order().empty());
        REQUIRE(bst.in_order().empty());
        REQUIRE(bst.find(10, GenericCompare{}) == nullptr);
    }
}

TEST_CASE("BST - Dodawanie elementow i przechodzenie drzewa", "[BST][add][traversal]") {
    BST<int> bst;

    SECTION("Dodanie jednego elementu") {
        bst.add(10, GenericCompare{});
        REQUIRE(bst.height() == 1);
        REQUIRE(bst.in_order() == std::vector<int>{10});
        REQUIRE(bst.pre_order() == std::vector<int>{10});
    }

    SECTION("Przejscie in-order zwraca elementy w kolejnosci posortowanej") {
        // Wstawianie elementów: 10, 5, 15, 3, 7, 12, 18
        bst.add(10, GenericCompare{});
        bst.add(5, GenericCompare{});
        bst.add(15, GenericCompare{});
        bst.add(3, GenericCompare{});
        bst.add(7, GenericCompare{});
        bst.add(12, GenericCompare{});
        bst.add(18, GenericCompare{});

        REQUIRE(bst.height() == 3);

        // In-Order MUSI byc idealnie posortowany
        REQUIRE(bst.in_order() == std::vector<int>{3, 5, 7, 10, 12, 15, 18});

        // Pre-Order sprawdza właściwą strukturę powiązań korzeń -> lewe -> prawe
        REQUIRE(bst.pre_order() == std::vector<int>{10, 5, 3, 7, 15, 12, 18});
    }
}

TEST_CASE("BST - Wyszukiwanie elementow (find)", "[BST][find]") {
    BST<int> bst;
    bst.add(20, GenericCompare{});
    bst.add(10, GenericCompare{});
    bst.add(30, GenericCompare{});

    SECTION("Odnajdywanie istniejacych elementow") {
        auto* node20 = bst.find(20, GenericCompare{});
        REQUIRE(node20 != nullptr);
        REQUIRE(node20->data == 20);

        auto* node10 = bst.find(10, GenericCompare{});
        REQUIRE(node10 != nullptr);
        REQUIRE(node10->data == 10);
    }

    SECTION("Wyszukiwanie elementu, ktorego nie ma w drzewie") {
        REQUIRE(bst.find(999, GenericCompare{}) == nullptr);
    }
}

TEST_CASE("BST - Usuwanie wezlow (remove)", "[BST][remove]") {
    BST<int> bst;
    /*
            Struktura testowa:
                   10
                  /  \
                 5    15
                / \   /
               3   7 12
    */
    bst.add(10, GenericCompare{});
    bst.add(5, GenericCompare{});
    bst.add(15, GenericCompare{});
    bst.add(3, GenericCompare{});
    bst.add(7, GenericCompare{});
    bst.add(12, GenericCompare{});

    SECTION("Usuwanie liscia (bez dzieci, wezel 3)") {
        auto* node3 = bst.find(3, GenericCompare{});
        REQUIRE(node3 != nullptr);

        bst.remove(node3);

        REQUIRE(bst.find(3, GenericCompare{}) == nullptr);
        REQUIRE(bst.in_order() == std::vector<int>{5, 7, 10, 12, 15});
    }

    SECTION("Usuwanie wezla z jednym dzieckiem (wezel 15)") {
        auto* node15 = bst.find(15, GenericCompare{});
        REQUIRE(node15 != nullptr);

        bst.remove(node15);

        REQUIRE(bst.find(15, GenericCompare{}) == nullptr);
        REQUIRE(bst.in_order() == std::vector<int>{3, 5, 7, 10, 12});
    }

    SECTION("Usuwanie wezla z dwojka dzieci (korzen 10)") {
        auto* node10 = bst.find(10, GenericCompare{});
        REQUIRE(node10 != nullptr);

        bst.remove(node10);

        REQUIRE(bst.find(10, GenericCompare{}) == nullptr);
        REQUIRE(bst.in_order() == std::vector<int>{3, 5, 7, 12, 15});
    }
}

TEST_CASE("BST - Zarzadzanie wskaznikami (int*) oraz czyszczenie", "[BST][pointers]") {
    auto* bst = new BST<int*>();

    bst->add(new int(50), GenericCompare{});
    bst->add(new int(25), GenericCompare{});
    bst->add(new int(75), GenericCompare{});

    SECTION("Wyszukiwanie wartosci pod wskaznikiem") {
        int key = 25;
        auto* result = bst->find(&key, GenericCompare{});
        REQUIRE(result != nullptr);
        REQUIRE(*(result->data) == 25);
    }

    SECTION("Czyszczenie drzewa (clear)") {
        bst->clear(true);
        REQUIRE(bst->height() == 0);
        REQUIRE(bst->in_order().empty());
    }

    delete bst;
}

TEST_CASE("BST - Praca ze zlozonym typem objects<Types...>", "[BST][custom_type]") {
    using Student = objects<int, std::string>;
    BST<Student> bst;

    bst.add(Student{ std::make_tuple(102, "Anna") }, GenericCompare{});
    bst.add(Student{ std::make_tuple(101, "Jan") }, GenericCompare{});
    bst.add(Student{ std::make_tuple(103, "Piotr") }, GenericCompare{});

    SECTION("In-order sortuje po krotce leksykograficznie") {
        auto res = bst.in_order();
        REQUIRE(res.size() == 3);
        REQUIRE(std::get<0>(res[0].pack) == 101); // Jan
        REQUIRE(std::get<0>(res[1].pack) == 102); // Anna
        REQUIRE(std::get<0>(res[2].pack) == 103); // Piotr
    }
}