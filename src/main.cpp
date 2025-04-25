#include"../include/Lexer.h"
#include"../include/Parser.h"
#include"../include/RTLILGen.h"
#include"../include/rtlil_to_dot.h"

std::string  readFileAsString(const std::string& filename)
{
    std::ifstream file(filename);             // 打开文件
    if (!file) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();                   // 读取整个文件流到缓冲区
    return buffer.str();                      // 返回字符串
    file.close();
    std::cout << "成功转为字符串" << std::endl;
}
int main(){

    std::string VerilogInput = "VerilogInput.txt";
    std::string VerilogInputString = readFileAsString(VerilogInput);

 /*   Lexer lexer("module carry(a, b, c, d, e, cout);  input a, b, c;  output cout;  assign cout = a - b + d + e - c; endmodule");*/
    Lexer lexer(VerilogInputString);

     Parser parser(lexer);
    AstNode* node=parser.parseProgram();

    node->print(0);

    RTLILGen rtlil(node);
    std::string result = rtlil.generateRTLIL();
    std::cout << result << std::endl;
    std::ofstream outFile("RTLILoutput.txt");  // 打开文件（默认 trunc 清空）
    if (outFile.is_open()) {
        outFile << result;
  
        outFile.close();
    }
    else {
        std::cerr << "无法打开文件写入。\n";
    }
    std::string RTLILinput = "RTLILoutput.txt";
    
    std::string RTLILInputString=readFileAsString(RTLILinput);

    Lexer lexer1(RTLILInputString);
    RTLILToDot RTLILTODOT(lexer1);
    RTLILTODOT.read_file();
    RTLILTODOT.Generate_DOT();
    return 0;

}