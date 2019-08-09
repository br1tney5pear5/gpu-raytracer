#pragma once
#include <iostream>
#include <signal.h>
class Logger{
public:
    template<bool FLUSH = false, bool NEWLINE = false, typename ...Ts> 
    constexpr static void _print(Ts... ts){
        (void) std::initializer_list<int>{ (std::cout << ts, 0)...};
        if constexpr (FLUSH)    std::cout << '\n';
        if constexpr (NEWLINE)  std::cout << std::flush;
    }
    template<bool FLUSH = false, bool NEWLINE = false, typename ...Ts> 
    constexpr static void _sprint(Ts... ts){
        (void) std::initializer_list<int>{ (std::cout << ts << " ", 0)...};
        if constexpr (FLUSH)    std::cout << '\n';
        if constexpr (NEWLINE)  std::cout << std::flush;
    }
    template<bool FLUSH = false, bool NEWLINE = false, typename ...Ts> 
    constexpr static void _tprint(Ts... ts){
        (void) std::initializer_list<int>{ (std::cout << ts << "\t", 0)...};
        if constexpr (FLUSH)    std::cout << '\n';
        if constexpr (NEWLINE)  std::cout << std::flush;
    }

    constexpr static void header(int line, const char* file){
        _print(line, ":", file, " ");
    }
};

#define LOG Logger::header(__LINE__, __FILE__); Logger::_print<true, true>
#define SLOG Logger::header(__LINE__, __FILE__); Logger::_sprint<true, true>
#define TLOG Logger::header(__LINE__, __FILE__); Logger::_tprint<true, true>

//Logger::header(__LINE__, __FILE__); Logger::_print<true, true>
//#define err Logger::header(__LINE__, __FILE__); Logger::_print<true, true>

