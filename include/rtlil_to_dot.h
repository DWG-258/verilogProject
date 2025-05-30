#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <regex>
#include "../include/Lexer.h"
#include "../include/Token.h"
// Simple RTLIL to DOT converter (limited to basic RTLIL like add/sub and connections)



class RtlilTrans
{
private:
    std::string moudle_name;

    struct Cell {
        std::string name;
        std::string type;
        std::map<std::string, std::string> ports; // port_name -> signal
    };

    struct Connection {
        std::string src;
        std::string dst;
        std::string label;
    };

    std::ifstream rtlil;
    std::ofstream dot;

 
    std::vector<std::string> inputs, outputs;
    std::vector<Connection*> connections;
    std::vector<Cell*> cells;
    bool inCell = false;
    int moduleID=0;
    //用于判断cell是否存在中间值
    bool has_temp = false;
    //用于记录级联表达式的参数个数
    int and_var_num = 0;
    int or_var_num = 0;

    std::vector<Token> tokens;
    size_t currentTokenIndex;
    Token currentToken;
public:
    void read_file();
    void gen_input();
    void gen_output();
    void gen_cell();
    void gen_connection();
    void gen_module();
    void Generate_DOT();
    void genrate_blif();
    Cell* find_temp(Cell* cell);

    std::string get_rid_of_str(std::string& s)
    {
        if (!s.empty() && s[0] == '\\') {
            s = s.substr(1);
        }
        return s;
    }

    RtlilTrans(Lexer& lexer);
};