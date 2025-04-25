#include"../include/rtlil_to_dot.h"

RTLILToDot::RTLILToDot(Lexer& lexer)
{
    //获取一组tokens
    Token token = lexer.NextToken();
    while (token.type != END_OF_INPUT) {
        tokens.push_back(token);
        token = lexer.NextToken();
    }
    tokens.push_back(token);//push back the END_OF_INPUT token
    currentTokenIndex = 0;
    currentToken = tokens[currentTokenIndex];

    //for (auto& i : tokens)
    //{
    //    std::cout << i.value << std::endl;
    //}
}




//TO DO 修改以匹配RTLIL格式文件
void RTLILToDot::read_file()
{
    while (currentToken.type!=END_OF_INPUT)
    {
        gen_module();
        //解析完模组，解析下一个
        currentToken = tokens[++currentTokenIndex];
        ++moduleID;
    }
 
}
void RTLILToDot::gen_module()
{
    while(currentToken.value!="endmodule")
    {
        if (currentToken.value == "input") {
            gen_input();
            currentToken = tokens[++currentTokenIndex];
        }
        else if (currentToken.value == "output") {
            gen_output();
            currentToken = tokens[++currentTokenIndex];
        }
        else if (currentToken.value == "cell") {

            inCell = true;
            gen_cell();
            currentToken = tokens[++currentTokenIndex];
            inCell = false;
        }
        else if (currentToken.value == "connect") {
            if (!inCell)
                gen_connection();
            currentToken = tokens[++currentTokenIndex];
        }
        else
        {
            currentToken = tokens[++currentTokenIndex];
        }
    }

}

void RTLILToDot::gen_input()
{
    //should be name
    currentToken = tokens[currentTokenIndex + 3];
    inputs.push_back(currentToken.value + "_" + std::to_string(moduleID));
}

void RTLILToDot::gen_output()
{
    //should be name
    currentToken = tokens[currentTokenIndex + 3];
    outputs.push_back(currentToken.value + "_" + std::to_string(moduleID));
}

void RTLILToDot::gen_cell()
{
    //should be type
    currentToken = tokens[++currentTokenIndex];
    Cell* cell = new Cell();
    cell->type = currentToken.value + "_" + std::to_string(moduleID);

    currentToken = tokens[++currentTokenIndex];
    cell->name= currentToken.value + "_" + std::to_string(moduleID);
    
    //输入
    currentTokenIndex = currentTokenIndex + 3;
    currentToken = tokens[currentTokenIndex];
    cell->ports["A"] = currentToken.value+"_"+std::to_string(moduleID);

    currentTokenIndex = currentTokenIndex + 3;
    currentToken = tokens[currentTokenIndex];
    cell->ports["B"] = currentToken.value + "_" + std::to_string(moduleID);
    //输出
    currentTokenIndex = currentTokenIndex + 3;
    currentToken = tokens[currentTokenIndex];
    cell->ports["Y"] = currentToken.value + "_" + std::to_string(moduleID);

    cells.push_back(cell);
}

void RTLILToDot::gen_connection()
{
    currentToken = tokens[++currentTokenIndex];
    Connection* connection = new Connection();
    connection->dst = currentToken.value + "_" + std::to_string(moduleID);

    currentToken = tokens[++currentTokenIndex];
    connection->src = currentToken.value + "_" + std::to_string(moduleID);

    connection->label = "=";

    connections.push_back(connection);
}

void RTLILToDot::Generate_DOT()
{
    std::ofstream dotFile("output.txt");
    dotFile << "digraph G {\n";
    dotFile << "  rankdir=LR;\n";
    dotFile << "  node [shape=box, style=rounded];\n";

    // 输入输出节点
    for (auto& in : inputs)
        dotFile << "  \"" << in << "\" [shape=ellipse, color=blue];\n";
    for (auto& out : outputs)
        dotFile << "  \"" << out << "\" [shape=doublecircle, color=green];\n";

    // 单元节点
    for (auto& cell : cells)
    {
        dotFile << "  \"" << cell->ports["Y"] << "\" [label=\"" << cell->ports["Y"] << "\"shape=ellipse, color=blue];\n";
        dotFile << "  \"" << cell->name << "\" [label=\"" << cell->type << "\", shape=box, style=filled, fillcolor=lightblue];\n";
        dotFile << "  \"" << cell->ports["A"] << "\" -> \"" << cell->name << "\" ;\n";
        dotFile << "  \"" << cell->ports["B"] << "\" -> \"" << cell->name << "\" ;\n";
        dotFile << "  \"" << cell->name << "\" -> \"" << cell->ports["Y"] << "\" ;\n";
        
    }

    // 连接
    for (auto& conn : connections)
        dotFile << "  \"" << conn->src << "\" -> \"" << conn->dst << "\" [label=\"" << conn->label << "\"];\n";

    dotFile << "}\n";

    dotFile.close();

    std::cout << "DOT 文件已生成: carry.dot\n";


}

