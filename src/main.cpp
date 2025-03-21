#include"../include/Lexer.h"
#include"../include/Parser.h"
int main(){
    Lexer lexer("module carry(a,b,c,cout); input a, b, c;output cout ;wire x; assign x = a & b;  assign cout = x | c; endmodule");
     Parser parser(lexer);
    AstNode* node=parser.parseProgram();

    node->print(0);
    return 0;
}