#include "../include/Lexer.h"
#include "../include/helper.h"

#include <iostream>
/**
 * 
 * 
 * @brier 灏嗗綋鍓嶅瓧绗︾Щ鍔ㄤ竴浣�
 */
void Lexer::advance(){
    position++;
    if(position < input.size()){
        currentChar = input[position];
    }else{
        currentChar = '\0';
    }
}

Token Lexer::parseIdentifier(){
    std::string identifier;
    while(isalnum(currentChar)||currentChar == '_'||currentChar == '$'){
        identifier += currentChar;
        advance();
    }
    if(keywords.count(identifier)){
        return Token(KEYWORD,identifier);
    }else{
        return Token(IDENTIFIER,identifier);
    }
}

Token Lexer::parseNumber(){
    std::string number;
    while(isdigit(currentChar)){
        number +=currentChar;
        advance();
    }
    return Token(NUMBER,number);
}

Token Lexer::parseOperator(){
    std::string op;
    while(is_operator(currentChar)){
        op+=currentChar;
        advance();
    }
    if(!operators.count(op)){
        std::cerr<<"Unknown operator: "<<op<<std::endl;
        exit(1);
    }
    return Token(OPERATOR,op);
}

Token Lexer::parseSeparator(){
    std::string sep;
    sep += currentChar;
    advance();
    return Token(SEPARATOR,sep);
}

Token Lexer::parseBracket(){
    std::string bracket;
    bracket += currentChar;
    advance();
    return Token(BRACKET,bracket);
}

void Lexer::skipWhitespace(){
    while(isspace(currentChar)){
        advance();
    }
}

void Lexer::skipComment(){
    if(currentChar == '/'&&input[position+1]=='/'){
        while(currentChar != '\n'||currentChar!='\0'){
            advance();
        }
    }
    if(currentChar == '/'&&input[position+1]=='*'){
        while(currentChar !='*'&&input[position+1]!='/'){
            advance();
        }
        advance();
        advance();
    }
}


/**
 * 
 * @brief 鑾峰彇涓嬩竴涓猼oken
 * 
 * @return token
 *
 */
Token Lexer::NextToken(){
    skipWhitespace();
    skipComment();

    if(currentChar=='\0'){
        return Token(END_OF_INPUT,"");
    }

    if(isalpha(currentChar)||currentChar == '_'||currentChar == '$'){
        return parseIdentifier();
    }else if(isdigit(currentChar)){
        return parseNumber();
    }else if(is_operator(currentChar)){
        return parseOperator();
    }else if(currentChar == ';'||currentChar == ','){
        return parseSeparator();
    }else if(currentChar == '['||currentChar == ']'||currentChar == '('||currentChar == ')'){
        return parseBracket();
    }

    std::cerr<<"Unknown character: "<<currentChar<<std::endl;
    exit(1);
}