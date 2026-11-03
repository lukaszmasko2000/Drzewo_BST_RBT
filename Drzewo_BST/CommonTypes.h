#pragma once
#include <iostream>
#include <string>
#include <tuple>
#include <format>
#include <type_traits>
#include <compare>


//Struktura z paczką dowolnych typów (jest zapisana na krotce), dla std::cout
template <class... Types>
struct objects {
    std::tuple<Types...> pack;

    auto operator<=>(const objects&) const = default;

    friend std::ostream& operator<<(std::ostream& os, const objects<Types...>& obj) {
        os << "(";
        std::apply([&os](const auto&... args) {     //std::apply to funkcja dla tuple która dodaje objekty do siebie
            auto n{ 0uz };
            ((os << args << (++n != sizeof...(args) ? ", " : "")), ...);
            }, obj.pack);
        return os << ")";
    }
};

//Struktura z paczką dowolnych typów, dla std::print
template <class... Types>
struct std::formatter<objects<Types...>> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }
    auto format(const objects<Types...>& obj, std::format_context& ctx) const {
        auto out = ctx.out();
        std::format_to(out, "(");
        std::apply([&out](const auto&... args) {
            auto n{ 0uz };
            ((std::format_to(out, "{}{}", args, (++n != sizeof...(args) ? ", " : ""))), ...);
            }, obj.pack);
        return std::format_to(out, ")");
    }
};

//Komparator który zwraca odpowiednią wartość zależnie od wyniku porównania.
struct GenericCompare {
    template <typename T1, typename T2>
    int operator()(const T1& a, const T2& b) const {
        if constexpr (std::is_pointer_v<T1> && std::is_pointer_v<T2>) {     // czy to jest wskaźnik typu złożonego
            if (!a || !b) {                                                   
                if (a == b) return 0;
                return (a < b) ? -1 : 1;
            }
            auto cmp = *a <=> *b;                                           // "spaceship operator", kompilator sam określa jak będzie to sprawdzał                                 
            if (cmp < 0) return -1;                                         // cmp jest true jeśli A<B (zwraca -1)
            if (cmp > 0) return 1;                                          // cmp jest true jeśli A>B (zwraca 1)
            return 0;                                                       // jeśli jest równe to zwraca 0
        }
        else {                                                              // czy to jest zwykły typ
            auto cmp = a <=> b;                                             // tutaj identycznie jak powyżej tylko bez wskaźnika
            if (cmp < 0) return -1;
            if (cmp > 0) return 1;
            return 0;
        }
    }
};