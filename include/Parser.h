#include "Token.h"
#include<iostream>
#include<string>
#include<vector>
#include "Ast.h"
#include "Lexer.h"
class Parser{

    private:
    std::vector<Token> tokens;
    size_t currentTokenIndex;
    Token currentToken;

    public:
        Parser(Lexer& lexer);
        AstNode* parseProgram();//瑙ｆ瀽鏁翠釜鏂囦欢
        AstNode* parseModuleDeclaration();
        AstNode* parseModuleBody();
        AstNode* parseDeclaration();
        AstNode* parsePortList();
        AstNode* parsePortListTail();
        AstNode* parseDataType();
        AstNode* parseIdentifier();
        AstNode* parseIdentifierTail();
        AstNode* parseStatement();
        AstNode* parseIfStatement();
        AstNode* parseElseStatement();
        AstNode* parseAssignment();
        AstNode* parseInitialStatement();
        AstNode* parseAlwaysStatement();
        AstNode* parseExpression(int level=0);
        AstNode* parseBinaryExpression();
        AstNode* parseUnaryExpression();
        AstNode* parseOperator();

};