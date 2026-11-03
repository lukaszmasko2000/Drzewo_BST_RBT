#pragma once
#pragma once
#include "catch_amalgamated.hpp"
#include <vector>
#include <string>
#include <tuple>
#include "RBT_Tree.h"
#include "CommonTypes.h"

TEST_CASE("RBT - Stan poczatkowy", "[RBT][basic]") {
    RBT<int> rbt;

    SECTION("Nowe drzewo czerwono-czarne jest puste") {
        REQUIRE(rbt.height() == 0);
        REQUIRE(rbt.pre_order().empty());
        REQUIRE(rbt.in_order().empty());
        REQUIRE(rbt.find(10, GenericCompare{}) == nullptr);
    }
}

TEST_CASE("RBT - Dodawanie elementow i zachowanie balansu", "[RBT][add][traversal]") {
    RBT<int> rbt;

    SECTION("Dodanie jednego elementu (korzen jest czarny)") {
        rbt.add(10, GenericCompare{});
        REQUIRE(rbt.height() == 1);
        REQUIRE(rbt.in_order() == std::vector<int>{10});
        REQUIRE(rbt.pre_order() == std::vector<int>{10});
    }

    SECTION("Dodawanie rosnacych elementow uruchamia rotacje i utrzymuje zbalansowanie") {
        // W zwykłym BST dodanie 10, 20, 30 dałoby wysokość 3 (skośna lista).
        // W RBT rotacja spłaszcza drzewo (korzeń 20, dzieci 10 i 30) -> wysokość 2.
        rbt.add(10, GenericCompare{});
        rbt.add(20, GenericCompare{});
        rbt.add(30, GenericCompare{});

        REQUIRE(rbt.height() == 2);
        REQUIRE(rbt.in_order() == std::vector<int>{10, 20, 30});
        REQUIRE(rbt.pre_order() == std::vector<int>{20, 10, 30});
    }

    SECTION("Przejscie in-order zwraca elementy w kolejnosci posortowanej dla wiekszej liczby danych") {
        // Wstawianie: 10, 5, 15, 3, 7, 12, 18
        rbt.add(10, GenericCompare{});
        rbt.add(5, GenericCompare{});
        rbt.add(15, GenericCompare{});
        rbt.add(3, GenericCompare{});
        rbt.add(7, GenericCompare{});
        rbt.add(12, GenericCompare{});
        rbt.add(18, GenericCompare{});

        // In-Order MUSI być idealnie posortowany
        REQUIRE(rbt.in_order() == std::vector<int>{3, 5, 7, 10, 12, 15, 18});

        // Wysokość zbalansowanego RBT dla 7 elementów nie przekracza logarytmicznego progu
        REQUIRE(rbt.height() <= 4);
    }
}

TEST_CASE("RBT - Wyszukiwanie elementow (find)", "[RBT][find]") {
    RBT<int> rbt;
    rbt.add(20, GenericCompare{});
    rbt.add(10, GenericCompare{});
    rbt.add(30, GenericCompare{});

    SECTION("Odnajdywanie istniejacych elementow") {
        auto* node20 = rbt.find(20, GenericCompare{});
        REQUIRE(node20 != nullptr);
        REQUIRE(node20->data == 20);

        auto* node10 = rbt.find(10, GenericCompare{});
        REQUIRE(node10 != nullptr);
        REQUIRE(node10->data == 10);
    }

    SECTION("Wyszukiwanie elementu, ktorego nie ma w drzewie") {
        REQUIRE(rbt.find(999, GenericCompare{}) == nullptr);
    }
}

TEST_CASE("RBT - Usuwanie wezlow (remove) i naprawa balansu", "[RBT][remove]") {
    RBT<int> rbt;

    rbt.add(10, GenericCompare{});
    rbt.add(5, GenericCompare{});
    rbt.add(15, GenericCompare{});
    rbt.add(3, GenericCompare{});
    rbt.add(7, GenericCompare{});
    rbt.add(12, GenericCompare{});

    SECTION("Usuwanie liscia") {
        auto* node3 = rbt.find(3, GenericCompare{});
        REQUIRE(node3 != nullptr);

        rbt.remove(node3);

        REQUIRE(rbt.find(3, GenericCompare{}) == nullptr);
        REQUIRE(rbt.in_order() == std::vector<int>{5, 7, 10, 12, 15});
    }

    SECTION("Usuwanie wezla ze szczatkowym poddrzewem") {
        auto* node15 = rbt.find(15, GenericCompare{});
        REQUIRE(node15 != nullptr);

        rbt.remove(node15);

        REQUIRE(rbt.find(15, GenericCompare{}) == nullptr);
        REQUIRE(rbt.in_order() == std::vector<int>{3, 5, 7, 10, 12});
    }

    SECTION("Usuwanie korzenia") {
        auto* root_node = rbt.find(10, GenericCompare{});
        REQUIRE(root_node != nullptr);

        rbt.remove(root_node);

        REQUIRE(rbt.find(10, GenericCompare{}) == nullptr);
        REQUIRE(rbt.in_order() == std::vector<int>{3, 5, 7, 12, 15});
    }
}

TEST_CASE("RBT - Zarzadzanie wskaznikami (int*) oraz czyszczenie", "[RBT][pointers]") {
    auto* rbt = new RBT<int*>();

    rbt->add(new int(50), GenericCompare{});
    rbt->add(new int(25), GenericCompare{});
    rbt->add(new int(75), GenericCompare{});

    SECTION("Wyszukiwanie wartosci pod wskaznikiem") {
        int key = 25;
        auto* result = rbt->find(&key, GenericCompare{});
        REQUIRE(result != nullptr);
        REQUIRE(*(result->data) == 25);
    }

    SECTION("Czyszczenie drzewa (clear)") {
        rbt->clear(true);
        REQUIRE(rbt->height() == 0);
        REQUIRE(rbt->in_order().empty());
    }

    delete rbt;
}

TEST_CASE("RBT - Praca ze zlozonym typem objects<Types...>", "[RBT][custom_type]") {
    using Student = objects<int, std::string>;
    RBT<Student> rbt;

    rbt.add(Student{ std::make_tuple(102, "Anna") }, GenericCompare{});
    rbt.add(Student{ std::make_tuple(101, "Jan") }, GenericCompare{});
    rbt.add(Student{ std::make_tuple(103, "Piotr") }, GenericCompare{});

    SECTION("In-order sortuje po krotce leksykograficznie") {
        auto res = rbt.in_order();
        REQUIRE(res.size() == 3);
        REQUIRE(std::get<0>(res[0].pack) == 101); // Jan
        REQUIRE(std::get<0>(res[1].pack) == 102); // Anna
        REQUIRE(std::get<0>(res[2].pack) == 103); // Piotr
    }
}