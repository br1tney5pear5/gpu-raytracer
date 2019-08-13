#pragma once
#include <iostream>
#include <signal.h>
#define STR_HELPER(ARG) #ARG
#define STR(ARG) STR_HELPER(ARG)

#ifdef NO_COLOR
    #define RED(STR) STR
    #define YELLOW(STR) STR
    #define CYAN(STR) STR
    #define PURPLE(STR) STR
#else
        #define COL_NC "\e[0m"
        #define COL_RED "\e[1;31m"
        #define COL_YELLOW "\e[1;33m"
        #define COL_CYAN "\e[0;36m"
        #define COL_PURPLE "\e[0;35m"
        #define RED(STR) COL_RED STR COL_NC
        #define YELLOW(STR) COL_YELLOW STR COL_NC
        #define CYAN(STR) COL_CYAN STR COL_NC
        #define PURPLE(STR) COL_PURPLE STR COL_NC
#endif


#ifndef NO_COLOR
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
#endif


class Logger{
public:
    inline static size_t _indent;
    inline static size_t _secondary_indent;

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

    static std::string _header(int line, const char* file){
        std::string ret(std::string(file) + ":" + std::to_string(line) +  " ");
        _indent = ret.size();
        _secondary_indent = 0;
        return ret;
    }

    static std::string _error_header(int line, const char* file){
        std::string ret = _header(line, file);
        _indent = ret.size();
        ret += RED("error: ");
        _secondary_indent = 7;
        return ret;
    }

    static std::string _major_con_header(int, const char*){
        std::string ret(_indent + _secondary_indent, ' ');
        return ret;
    }

    static std::string _minor_con_header(int, const char*){
        std::string ret(_indent, ' ');
        return ret;
    }

};
#define LOG   Logger::_print(Logger::_header(__LINE__, __FILE__)); \
        Logger::_print<true, true>

#define SLOG  Logger::_print(Logger::_header(__LINE__, __FILE__)); \
        Logger::_print<true, true>

#define TLOG  Logger::_print(Logger::_header(__LINE__, __FILE__)); \
        Logger::_print<true, true>

#define ERROR  Logger::_print(Logger::_error_header(__LINE__, __FILE__)); \
        Logger::_print<true, true>

#define __CON  Logger::_print(Logger::_major_con_header(__LINE__, __FILE__)); \
    Logger::_print<true, true>

#define _CON  Logger::_print(Logger::_minor_con_header(__LINE__, __FILE__)); \
    Logger::_print<true, true>

#define CON Logger::_print<true, true>

//Logger::header(__LINE__, __FILE__); Logger::_print<true, true>
//#define err Logger::header(__LINE__, __FILE__); Logger::_print<true, true>

#ifndef NO_COLOR
    #pragma GCC diagnostic pop
#endif
