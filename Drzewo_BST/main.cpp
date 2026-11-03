#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <cmath>
#include <limits>
#include <print>

#include "BST_Tree.h"
#include "RBT_Tree.h"

#include "BST_Test.h"
#include "RBT_Test.h"



// Typ złożony na potrzeby demonstracji
using PersonData = objects<int, std::string, std::string, size_t>;

int main(int argc, char* argv[])
{
    auto test_result = Catch::Session().run(argc, argv);

    if (test_result != 0) {
        return test_result;
    }

    std::random_device rd;
    std::mt19937 gen(rd());

    // ===================================================================================
    // 1. DEMO: Prezentacja generyczności na złożonych danych {ID, Imie, Nazwisko, Wiek}
    // ===================================================================================
    std::println("=== DEMO: Drzewa BST i RBT ze zlozonym typem PersonData ===");

    const std::vector<std::string> imiona = { "Jan", "Anna", "Piotr", "Maria", "Krzysztof" };
    const std::vector<std::string> nazwiska = { "Kowalski", "Nowak", "Wisniewski", "Wojcik" };

    std::uniform_int_distribution<int> dist_id(1000, 9999);
    std::uniform_int_distribution<size_t> dist_age(15, 99);
    std::uniform_int_distribution<std::size_t> dist_fn(0, imiona.size() - 1);
    std::uniform_int_distribution<std::size_t> dist_ln(0, nazwiska.size() - 1);

    auto* demo_bst = new BST<PersonData*>();
    auto* demo_rbt = new RBT<PersonData*>();

    // Dodanie 5 identycznych obiektów do obu drzew
    for (auto i = 0uz; i < 5; ++i) {
        auto id = dist_id(gen);
        std::string fn = imiona[dist_fn(gen)];
        std::string ln = nazwiska[dist_ln(gen)];
        size_t age = dist_age(gen);

        // Alokujemy osobne instancje dla obu drzew, ponieważ drzewa same zwalniają pamięć
        demo_bst->add(new PersonData{ { id, fn, ln, age } }, GenericCompare{});
        demo_rbt->add(new PersonData{ { id, fn, ln, age } }, GenericCompare{});
    }

    std::println("\n--- Reprezentacja BST ---");
    std::println("{}", demo_bst->to_string());

    std::println("\n--- Reprezentacja RBT ---");
    std::println("{}", demo_rbt->to_string());

    delete demo_bst;
    delete demo_rbt;

    // =======================================
    // 2. EKSPERYMENT GLOWNY: Pomiary czasowe 
    // =======================================
    std::println("\n=== EKSPERYMENT GLOWNY: Pomiary wydajnosciowe (od 10^1 do 10^7) ===");

    std::uniform_int_distribution<int> dist_int(0, std::numeric_limits<int>::max());
    constexpr auto MAX_ORDER = 7uz; // Rzędy wielkości 10^1 do 10^7

    auto* bst = new BST<int*>();
    auto* rbt = new RBT<int*>();

    for (auto o = 1uz; o <= MAX_ORDER; ++o) {
        const auto n = static_cast<std::size_t>(std::pow(10, o));
        auto log_n = std::log2(n);

        std::println("----------------------------------------");
        std::println("Rzad wielkosci danych: 10^{} (n = {}) | log2(n) = {:.2f}", o, n, log_n);

        // Generujemy wspólne dane wejściowe dla danej iteracji, aby warunki były równe
        std::vector<int> random_data(n);
        for (auto i = 0uz; i < n; ++i) {
            random_data[i] = dist_int(gen);
        }

        // --- POMIARY WSTAWIANIA DLA BST ---
        auto start_add_bst = std::chrono::steady_clock::now();
        for (auto i = 0uz; i < n; ++i) {
            bst->add(new int(random_data[i]), GenericCompare{});
        }
        auto end_add_bst = std::chrono::steady_clock::now();
        std::chrono::duration<double> dur_add_bst = end_add_bst - start_add_bst;

        auto h_bst = bst->height();
        auto h_to_n_bst = static_cast<double>(h_bst) / static_cast<double>(n);
        auto h_to_log_bst = static_cast<double>(h_bst) / log_n;

        // --- POMIARY WSTAWIANIA DLA RBT ---
        auto start_add_rbt = std::chrono::steady_clock::now();
        for (auto i = 0uz; i < n; ++i) {
            rbt->add(new int(random_data[i]), GenericCompare{});
        }
        auto end_add_rbt = std::chrono::steady_clock::now();
        std::chrono::duration<double> dur_add_rbt = end_add_rbt - start_add_rbt;

        auto h_rbt = rbt->height();
        auto h_to_n_rbt = static_cast<double>(h_rbt) / static_cast<double>(n);
        auto h_to_log_rbt = static_cast<double>(h_rbt) / log_n;

        // --- WYPISYWANIE WYNIKÓW WSTAWIANIA ---
        std::println("\n[ WSTAWIANIE - BST ]");
        std::println("Calkowity czas: {} s", dur_add_bst.count());
        std::println("Sredni czas:    {} us", (dur_add_bst.count() / static_cast<double>(n)) * 1e6);
        std::println("Wysokosc (h):   {} | Stosunek h/n: {} | Stosunek h/log2(n): {}", h_bst, h_to_n_bst, h_to_log_bst);

        std::println("\n[ WSTAWIANIE - RBT ]");
        std::println("Calkowity czas: {} s", dur_add_rbt.count());
        std::println("Sredni czas:    {} us", (dur_add_rbt.count() / static_cast<double>(n)) * 1e6);
        std::println("Wysokosc (h):   {} | Stosunek h/n: {} | Stosunek h/log2(n): {}", h_rbt, h_to_n_rbt, h_to_log_rbt);

        // --- POMIARY WYSZUKIWANIA ---
        constexpr auto m = 10000uz;

        // Szukanie w BST
        auto hits_bst = 0uz;
        auto start_find_bst = std::chrono::steady_clock::now();
        for (auto i = 0uz; i < m; ++i) {
            auto* so = new int(dist_int(gen));
            if (bst->find(so, GenericCompare{}) != nullptr) hits_bst++;
            delete so;
        }
        auto end_find_bst = std::chrono::steady_clock::now();
        std::chrono::duration<double> dur_find_bst = end_find_bst - start_find_bst;

        // Szukanie w RBT
        auto hits_rbt = 0uz;
        auto start_find_rbt = std::chrono::steady_clock::now();
        for (auto i = 0uz; i < m; ++i) {
            auto* so = new int(dist_int(gen));
            if (rbt->find(so, GenericCompare{}) != nullptr) hits_rbt++;
            delete so;
        }
        auto end_find_rbt = std::chrono::steady_clock::now();
        std::chrono::duration<double> dur_find_rbt = end_find_rbt - start_find_rbt;

        // --- WYPISYWANIE WYNIKÓW WYSZUKIWANIA ---
        std::println("\n[ WYSZUKIWANIE ({} prob) ]", m);
        std::println("BST -> Czas calkowity: {} s | Sredni: {} us | Trafienia: {}",
            dur_find_bst.count(), (dur_find_bst.count() / static_cast<double>(m)) * 1e6, hits_bst);
        std::println("RBT -> Czas calkowity: {} s | Sredni: {} us | Trafienia: {}\n",
            dur_find_rbt.count(), (dur_find_rbt.count() / static_cast<double>(m)) * 1e6, hits_rbt);

        bst->clear(true);
        rbt->clear(true);
    }

    delete bst;
    delete rbt;

    return 0;
}