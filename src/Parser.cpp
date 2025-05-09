#include"../include/Parser.h"
#include"../include/Token.h"

Parser::Parser(Lexer& lexer){
    //获取一组tokens
    Token token=lexer.NextToken();
    while(token.type!=END_OF_INPUT){
        tokens.push_back(token);
        token=lexer.NextToken();
    }
    tokens.push_back(token);//push back the END_OF_INPUT token
    currentTokenIndex=0;
    currentToken=tokens[currentTokenIndex];

    //for (auto& i : tokens)
    //{
    //    std::cout <<i.value<< std::endl;
    //}
}

AstNode* Parser::parseProgram(){
    AstNode* node=new AstNode();
    node->value="program";
    while(currentToken.type!= END_OF_INPUT)
    {
        AstNode* child = parseModuleDeclaration();
        if (child != nullptr) {
            node->children.push_back(child);
        }
        currentToken = tokens[++currentTokenIndex];
    }
  

    return node;
}

AstNode* Parser::parseModuleDeclaration(){
    //输入错误处理，传入空节点
    if (currentToken.type != TokenType::KEYWORD || currentToken.value != "module")
    {
        std::cout << "wrrong input:it is not module"<<std::endl;
        exit(1);
        return nullptr;
    }
    //读取下一个token
    currentToken = tokens[++currentTokenIndex];
    if (currentToken.type != TokenType::IDENTIFIER)
    {
        std::cout << "wrrong input:it is not a right moudule name" << std::endl;
        exit(1);
        return nullptr;
    }
    //该节点是Moudle
    AstNode* node = new AstNode();
    node->value = currentToken.value;

    //以上处理了MOUDLE关键字和moudle名

    //处理参数列表
    //子节点参数列表,参数列表在子节点并排表示
    node->children.push_back(parsePortList());
    //ct=";"
    currentToken = tokens[++currentTokenIndex];

  
    //子节点模块体
    while (currentToken.value != "endmodule")
    {
        node->children.push_back(parseModuleBody());
       
    }
    

    return node;
}

//TODO
AstNode* Parser::parseModuleBody()
{
    //获取模块体的每一行第一个
    if (currentToken.value == ";")
    {
        currentToken = tokens[++currentTokenIndex];
    }
    AstNode* node = new AstNode();
    if (currentToken.type != TokenType::KEYWORD)
    {
        std::cerr << "wrong keyword:" << currentToken.value<<std::endl;
        exit(1);
    }
   //声明模块
    if ( currentToken.value == "input"|| currentToken.value == "reg"|| currentToken.value == "output"|| currentToken.value == "integer"|| currentToken.value == "wire")
    {
        node=parseDeclaration();
    }


    else {
        node=parseStatement();
    }

    return node;
}

/*
声明语句
*/
AstNode* Parser::parseDeclaration()
{
    AstNode* node = new AstNode();
    if (currentToken.type == TokenType::KEYWORD)
    {
        node->value = currentToken.value;
        //获取声明变量
        currentToken = tokens[++currentTokenIndex];
        
        while (currentToken.value != ";")
        {
            //跳过逗号
            if (currentToken.value == ",")
            {
                currentToken = tokens[++currentTokenIndex];
            }

            AstNode* parm = new AstNode();
            parm->value = currentToken.value;
            node->children.push_back(parm);
            currentToken = tokens[++currentTokenIndex];
        }
    }
    return node;


}

//参数列表
AstNode* Parser::parsePortList()
{
    currentToken = tokens[++currentTokenIndex];
    if (currentToken.value != "(")
    {
        std::cerr << "wrong input ports" << std::endl;
        exit(1);
        return nullptr;
    }
    AstNode* node = new AstNode();
    node->value = "parsePortList";
    currentToken = tokens[++currentTokenIndex];
    //记得在树节点手动释放内存，该代码没有使用智能指针
    AstNode* childnode = new AstNode();
    childnode->value = currentToken.value;
    node->children.push_back(childnode);

    //调用下一级，参数在子节点并排表示
    while(currentToken.value!=")")
    node->children.push_back(parsePortListTail());
    
     return node;

}


AstNode* Parser::parsePortListTail()
{
    currentToken = tokens[++currentTokenIndex];
    //跳过逗号
    if (currentToken.value == ",")
    {
        currentToken = tokens[++currentTokenIndex];
    }
    //ct="参数"
    if (currentToken.value == ")")
    {
        return nullptr;
   }

    AstNode* node = new AstNode();
    node->value = currentToken.value;
    return node;

}



AstNode* Parser::parseAssignment() {
    AstNode* node = new AstNode();
    node->value = "assign";
    node->type = "statement";
    currentToken = tokens[++currentTokenIndex];
    if (currentToken.type == IDENTIFIER) {
        AstNode* child = new AstNode();
        child->value = currentToken.value;
        child->type = "variable";
        node->children.push_back(child);
        currentToken = tokens[++currentTokenIndex];
    }
    if (currentToken.type == OPERATOR && currentToken.value == "=") {
        currentToken = tokens[++currentTokenIndex];
    }
    else {
        std::cerr << "Error: Expected = in assignment" << std::endl;
        exit(1);
    }
    AstNode* child2 = parseExpression(0);
    if (child2 != nullptr) {
        node->children.push_back(child2);
    }
    if (currentToken.type == SEPARATOR && currentToken.value == ";") {
        currentToken = tokens[++currentTokenIndex];
    }
    else {
        std::cerr << "Error: Expected ; after assignment" << std::endl;
        exit(1);
    }
    return node;
}

AstNode* Parser::parseAlwaysStatement() {
    AstNode* node = new AstNode();
    node->value = "always";
    node->type = "statement";
    currentToken = tokens[++currentTokenIndex];
    if (currentToken.type == OPERATOR && currentToken.value == "@") {
        currentToken = tokens[++currentTokenIndex];
    }
    else {
        std::cerr << "Error: Expected @ in always statement" << std::endl;
        exit(1);
    }
    if (currentToken.type == BRACKET && currentToken.value == "(") {
        currentToken = tokens[++currentTokenIndex];
        while (currentToken.type != BRACKET && currentToken.value != ")") {
            AstNode* child = new AstNode();
            child->value = currentToken.value;
            child->type = "variable";
            node->children.push_back(child);
            currentToken = tokens[++currentTokenIndex];
            if (currentToken.type == SEPARATOR && currentToken.value == ",") {
                currentToken = tokens[++currentTokenIndex];
            }
        }
        currentToken = tokens[++currentTokenIndex];
        AstNode* child2 = parseStatement();
        if (child2 != nullptr) {
            node->children.push_back(child2);
        }
        return node;
    }
}

AstNode* Parser::parseInitialStatement() {
    AstNode* node = new AstNode();
    node->value = "initial";
    node->type = "statement";
    currentToken = tokens[++currentTokenIndex];

    while (currentToken.value != "end")
    {
        AstNode* child = parseStatement();
        if(child!=nullptr)
        node->children.push_back(child);
    }
    currentToken = tokens[++currentTokenIndex];
    currentToken = tokens[++currentTokenIndex];
    return node;
}


AstNode* Parser::parseExpression(int minPrec) {
    AstNode* lhs = nullptr;

    // 初始：处理数字、变量或一元表达式
    if (currentToken.type == OPERATOR) {
        lhs = parseUnaryExpression();  // 如 -a
    }
    else if (currentToken.type == IDENTIFIER || currentToken.type == NUMBER) {
        lhs = new AstNode();
        lhs->value = currentToken.value;
        lhs->type = (currentToken.type == IDENTIFIER) ? "variable" : "number";
        currentToken = tokens[++currentTokenIndex];
    }
    else {
        std::cerr << "Unexpected token in expression" << std::endl;
        exit(1);
    }

    // 接下来：处理操作符（可能是 binary）
    while (currentTokenIndex < tokens.size()
        && currentToken.type == OPERATOR
        && precedence[currentToken.value] >= minPrec) {

        std::string op = currentToken.value;
        int opPrec = precedence[op];
        bool rightAssoc = isRightAssociative[op];
        int nextMinPrec = rightAssoc ? opPrec : opPrec + 1;

        currentToken = tokens[++currentTokenIndex]; // consume operator

        AstNode* rhs = parseExpression(nextMinPrec);

        AstNode* newNode = new AstNode();
        newNode->value = op;
        newNode->type = "operator";
        newNode->children.push_back(lhs);
        newNode->children.push_back(rhs);
        lhs = newNode;
    }

    return lhs;
}

AstNode* Parser::parseUnaryExpression() {
    AstNode* node = new AstNode();
    node->value = currentToken.value;
    node->type = "operator";
    currentToken = tokens[++currentTokenIndex];
    AstNode* child = parseExpression(0);
    if (child != nullptr) {
        node->children.push_back(child);
    }
    return node;
}

AstNode* Parser::parseStatement() {
    if (currentToken.type == KEYWORD) {
        if (currentToken.value == "if") {
            return parseIfStatement();
        }
        else if (currentToken.value == "assign") {
            return parseAssignment();
        }
        else if (currentToken.value == "initial") {
            return parseInitialStatement();
        }
        else if (currentToken.value == "always") {
            return parseAlwaysStatement();
        }
        else {
            std::cerr << "Error: unknown keyword " << currentToken.value << std::endl;
            exit(1);
        }
    }
}
AstNode* Parser::parseIfStatement() {
    AstNode* node = new AstNode();
    node->value = "if";
    node->type = "statement";
    currentToken = tokens[++currentTokenIndex];
    if (currentToken.type == BRACKET && currentToken.value == "(") {
        currentToken = tokens[++currentTokenIndex];
        AstNode* child = parseExpression(0);
        if (child != nullptr) {
            node->children.push_back(child);
        }
        if (currentToken.type == BRACKET && currentToken.value == ")") {
            currentToken = tokens[++currentTokenIndex];
        }
        else {
            std::cerr << "Error: Expected ) after if expression" << std::endl;
            exit(1);
        }
        AstNode* child2 = parseStatement();
        if (child2 != nullptr) {
            node->children.push_back(child2);
        }
        if (currentToken.type == KEYWORD && currentToken.value == "else") {
            currentToken = tokens[++currentTokenIndex];
            AstNode* child3 = parseStatement();
            if (child3 != nullptr) {
                node->children.push_back(child3);
            }
        }
        return node;
    }
    else {
        std::cerr << "Error: Expected ( after if" << std::endl;
        exit(1);
    }
}
