#include <cmath>
#include <string>
#include <print>
#include <format>
#include <random>
#include <chrono>
#include <type_traits>
#include <compare>
#include <vector>
#include <locale>

#ifdef _WIN32
#include <windows.h>
#endif

#include "RBTree.h"

struct SomeObject {
    size_t key;
    std::string value;

    SomeObject(size_t k, std::string v) : key(k), value(std::move(v)) {}

    auto operator<=>(const SomeObject&) const = default;
};

struct GenericCompare {
    template <typename T1, typename T2>
    int operator()(const T1& a, const T2& b) const {
        if constexpr (std::is_pointer_v<T1> && std::is_pointer_v<T2>) {
            if (!a || !b) {
                if (a == b) return 0;
                return (a < b) ? -1 : 1;
            }

            auto cmp = *a <=> *b;
            if (cmp < 0) return -1;
            if (cmp > 0) return 1;
            return 0;
        }
        else {
            auto cmp = a <=> b;
            if (cmp < 0) return -1;
            if (cmp > 0) return 1;
            return 0;
        }
    }
};

std::string format_some_object(SomeObject* obj) {
    if (obj == nullptr) return "NULL";
    return std::format("{}, {}", obj->key, obj->value);
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    std::locale::global(std::locale("pl_PL.UTF-8"));

    const std::vector<std::string> polish_alphabet = {
        "a", "ą", "b", "c", "ć", "d", "e", "ę", "f", "g", "h", "i", "j", "k", "l", "ł",
        "m", "n", "ń", "o", "ó", "p", "r", "s", "ś", "t", "u", "w", "y", "z", "ź", "ż"
    };

    std::mt19937_64 rng{ std::random_device{}() };
    std::uniform_int_distribution<size_t> dist_key;
    std::uniform_int_distribution<size_t> dist_char(0, polish_alphabet.size() - 1);

    constexpr int MAX_ORDER = 7;

    RedBlackTree<SomeObject*> rbt;

    std::println("--- Rozpoczęcie eksperymentu ---\n");

    for (int o = 1; o <= MAX_ORDER; o++) {
        const int n = static_cast<int>(std::pow(10, o));

        std::println(">>> Rząd wielkości danych: 10^{} (n = {}) <<<", o, n);

        auto t1 = std::chrono::steady_clock::now();
        for (int i = 0; i < n; i++) {
            auto* so = new SomeObject(dist_key(rng), polish_alphabet[dist_char(rng)]);
            rbt.add(so, GenericCompare{});
        }
        auto t2 = std::chrono::steady_clock::now();

        std::chrono::duration<double> add_time_dur = t2 - t1;
        double add_time = add_time_dur.count();

        std::print("{}", rbt.to_string(format_some_object));
        std::println("  - Wysokość drzewa: {}", rbt.getHeight());

        std::println("  - Czas dodawania całkowity: {:.6f} s", add_time);
        std::println("  - Czas dodawania średni:    {:.9f} s\n", add_time / n);

        const int m = static_cast<int>(std::pow(10, 4));
        int hits = 0;

        t1 = std::chrono::steady_clock::now();
        for (int i = 0; i < m; i++) {
            auto* so = new SomeObject(dist_key(rng), "ą");

            if (auto* result = rbt.find(so, GenericCompare{}); result != nullptr) {
                hits++;
            }
            delete so;
        }
        t2 = std::chrono::steady_clock::now();

        std::chrono::duration<double> search_time_dur = t2 - t1;
        double search_time = search_time_dur.count();

        std::println("  - Próby wyszukiwania: {}, Trafienia: {}", m, hits);
        std::println("  - Czas wyszukiwania całkowity: {:.6f} s", search_time);
        std::println("  - Czas wyszukiwania średni:    {:.9f} s", search_time / m);
        std::println("----------------------------------------\n");

        rbt.clear(true);
    }

    std::println("Eksperyment zakończony.");
    return 0;
}