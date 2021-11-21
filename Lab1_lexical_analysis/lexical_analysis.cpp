#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const int ERR_STATE = 35;
const int BUFFER_SIZE = 1000;
const int BUFFER_L_END = (BUFFER_SIZE / 2) - 1;
const int BUFFER_R_END = BUFFER_SIZE - 1;
const int SYMBOL_SIZE = 2000;
const int NUMBER_SIZE = 2000;
const int ERR_SIZE = 50;
const int KEY_CNT = 32;
const std::string keyTable[KEY_CNT] = { "int", "long", "short", "float", "double", "char", "unsigned", "signed", "const", "void", "volatile", "enum", "struct", "union", "if", "else", "goto", "switch", "case", "do", "while", "for", "continue", "break", "return", "default", "typedef", "auto" };
const std::string FILE_PATH = "";

ifstream fin;
char ch = '\0';
int state = 0;
int forwardIndex = BUFFER_R_END;
int isKey;
int lexemeBegin = 0;
int entry;
int lineCnt = 1;
int charCnt = 0;
int keyTotal[KEY_CNT] = { 0 };
int errList[ERR_SIZE] = { 0 };
char buffer[BUFFER_SIZE];
std::string token = "";
std::string symbol[SYMBOL_SIZE] = { "" };
std::string constant[NUMBER_SIZE] = { "" };


void getChar();
void cat();
bool isLetter();
bool isDigit();
bool isOctDigit();
bool isHexDigit();
int reserve();
int symbolInsert();
int numberInsert();
int error();
void lexicalAna();
void outputToken(std::string, std::string);
void outputData();

int main(void)
{
    //将待分析的文件读取到缓冲区
    fin.open(FILE_PATH, ios::in);
    if (fin.is_open())
    {
        //std::cout << "reading files:" << std::endl;
        // int a = 0;
        // char temp;
        // //将程序从文件读到缓冲区
        // while (!fin.eof()) {
        //     temp = fin.get();
        //     charCnt++;//计算字符总数
        //     if (temp == '\n') //计算程序行数
        //         lineCnt++;
        //     buffer.append(1, temp);
        // }
        // std::cout << buffer << std::endl;

        //进行词法分析
        std::cout << "\n\nlexical analysis result:\n";
        lexicalAna();

        //输出统计结果
        std::cout << "\n\ndata of this program:" << std::endl;
        outputData();

        fin.close();
    }
    else
        std::cerr << "failed to open file";

    return 0;
}

void lexicalAna()
{
    state = 0;
    getChar();
    while (ch != EOF && ch != '\0')
    {
        switch (state)
        {
        case 0: //入口状态
            token = "";
            switch (ch)
            {
            case '0':
                state = 2;
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                state = 8;
                break;
            case '.':
                state = 14;
                break;
            case '+':
                state = 15;
                break;
            case '-':
                state = 16;
                break;
            case '*':
                state = 17;
                break;
            case '/':
                state = 18;
                break;
            case '%':
                state = 22;
                break;
            case '=':
                state = 23;
                break;
            case '!':
                state = 24;
                break;
            case '>':
                state = 25;
                break;
            case '<':
                state = 27;
                break;
            case '&':
                state = 29;
                break;
            case '|':
                state = 30;
                break;
            case '^':
                state = 31;
                break;
            case '~':
            case '(':
            case ')':
            case '[':
            case ']':
            case '{':
            case '}':
            case ',':
            case ';':
            case ':':
            case '\\':
            case '#':
                outputToken("op", string("").append(1, ch));
                state = 0;
                getChar();
                break;
            case '\'':
                state = 32;
                break;
            case '\"':
                state = 34;
                break;
            case '\n':
            case '\t':
            case ' ':
                getChar();
                state = 0;
                break;
            default:
                if (isLetter() || ch == '_')
                    state = 1;
                else
                    state = ERR_STATE;
                break;
            }
            break;
        case 1: //识别标识符状态
            cat();
            getChar();
            if (isLetter() || isDigit() || ch == '_')
                state = 1;
            else
            {
                //retract();
                state = 0;
                isKey = reserve();
                if (isKey != -1)
                {
                    outputToken(keyTable[isKey], "-");
                    keyTotal[isKey]++;
                }
                else
                {
                    entry = symbolInsert();
                    outputToken("symbol", "index:" + to_string(entry));
                }
            }
            break;
        case 2:
            cat();
            getChar();
            switch (ch)
            {
            case 'x':
            case 'X':
                state = 3;
                break;
            case '.':
                state = 9;
                break;
            default:
                if (isOctDigit())
                    state = 7;
                else
                {
                    //retract();
                    state = 0;
                    entry = numberInsert();
                    outputToken("num", "index:" + to_string(entry));
                }
            }
            break;
        case 3:
            cat();
            getChar();
            if (isHexDigit())
                state = 4;
            else
                state = ERR_STATE; //不合法的16进制数字
            break;
        case 4:
            cat();
            getChar();
            if (isHexDigit())
                state = 4;
            else if (ch == 'u' || ch == 'U')
                state = 5;
            else if (ch == 'l' || ch == 'L')
                state = 6;
            else
            {
                //retract();
                state = 0;
                entry = numberInsert();
                outputToken("num", "index:" + to_string(entry));
            }
            break;
        case 5:
            cat();
            getChar();
            if (ch == 'l' || ch == 'L'){
                cat();
                getChar();
            }                
            entry = numberInsert();
            outputToken("num", "index:" + to_string(entry));
            state = 0;
            break;
        case 6:
            cat();
            getChar();
            if (ch == 'u' || ch == 'U'){
                cat();
                getChar();
            }
            entry = numberInsert();
            outputToken("num", "index:" + to_string(entry));
            state = 0;
            break;
        case 7:
            cat();
            getChar();
            if (isOctDigit())
                state = 7;
            else if (ch == 'u' || ch == 'U')
                state = 5;
            else if (ch == 'l' || ch == 'L')
                state = 6;
            else
            {
                //retract();
                state = 0;
                entry = numberInsert();
                outputToken("num", "index:" + to_string(entry));
            }
            break;
        case 8:
            cat();
            getChar();
            if (isDigit())
                state = 8;
            else if (ch == 'u' || ch == 'U')
                state = 5;
            else if (ch == 'l' || ch == 'L')
                state = 6;
            else if (ch == '.')
                state = 9;
            else if (ch == 'e' || ch == 'E')
                state = 11;
            else
            {
                //retract();
                state = 0;
                entry = numberInsert();
                outputToken("num", "index:" + to_string(entry));
            }
            break;
        case 9:
            cat();
            getChar();
            if (isDigit())
                state = 10;
            else
                state = ERR_STATE;
            break;
        case 10:
            cat();
            getChar();
            if (isDigit())
                state = 10;
            else if (ch == 'e' || ch == 'E')
                state = 11;
            else{
                if (ch == 'f' || ch == 'F' || ch == 'l' || ch == 'L'){
                    cat();
                    getChar();
                }
                state = 0;
                entry = numberInsert();
                outputToken("num", "index:" + to_string(entry));
            }
            break;
        case 11:
            cat();
            getChar();
            if (ch == '+' || ch == '-')
                state = 12;
            else if (isDigit())
                state = 13;
            else
                state = ERR_STATE;
            break;
        case 12:
            cat();
            getChar();
            if (isDigit())
                state = 13;
            else
                state = ERR_STATE;
            break;
        case 13:
            cat();
            getChar();
            if (isDigit())
                state = 13;
            else
            {
                if (ch == 'f' || ch == 'F' || ch == 'l' || ch == 'L'){
                    cat();
                    getChar();
                }
                state = 0;
                entry = numberInsert();
                outputToken("num", "index:" + to_string(entry));
            }
            break;
        case 14:
            cat();
            getChar();
            if (isDigit())
                state = 10;
            else
            {
                //retract();
                state = 0;
                outputToken("op", ".");
            }
            break;
        case 15:
            cat();
            getChar();
            if (ch == '+')
            {
                cat();
                getChar();
                outputToken("op", "++");
            }
            else if (ch == '=')
            {
                cat();
                getChar();
                outputToken("op", "+=");
            }
            else
            {
                outputToken("op", "+");
            }
            state = 0;
            break;
        case 16:
            cat();
            getChar();
            if (ch == '-')
            {
                cat();
                getChar();
                outputToken("op", "--");
            }
            else if (ch == '=')
            {
                cat();
                getChar();
                outputToken("op", "-=");
            }
            else if (ch == '>')
            {
                cat();
                getChar();
                outputToken("op", "->");
            }
            else
            {
                //retract();
                outputToken("op", "-");
            }
            state = 0;
            break;
        case 17:
            cat();
            getChar();
            if (ch == '=')
            {
                cat();
                getChar();
                outputToken("op", "*=");
            }
            else
            {
                //retract();
                outputToken("op", "*");
            }
            state = 0;
            break;
        case 18:
            cat();
            getChar();
            if (ch == '=')
            {
                cat();
                getChar();
                outputToken("op", "/=");
                state = 0;
            }
            else if (ch == '/')
                state = 19;
            else if (ch == '*')
                state = 20;
            else
            {
                //retract();
                outputToken("op", "/");
                state = 0;
            }
            break;
        case 19:
            getChar();
            if (ch != '\n')
                state = 19;
            else
            {
                getChar();
                state = 0;
                std::cout << "读取到行注释\n";
            }
            break;
        case 20:
            getChar();
            if (ch == '*')
                state = 21;
            else
                state = 20;
            break;
        case 21:
            getChar();
            if (ch == '*')
                state = 21;
            else if (ch == '/')
            {
                getChar();
                state = 0;
                std::cout << "读取到块注释\n";
            }
            else
                state = 20;
            break;
        case 22:
            cat();
            getChar();
            if (ch == '=')
            {
                cat();                
                getChar();
                outputToken("op", "%=");
            }
            else
            {
                //retract();
                outputToken("op", "%");
            }
            state = 0;
            break;
        case 23:
            cat();
            getChar();
            if (ch == '=')
            {
                cat();
                getChar();
                outputToken("op", "==");
            }
            else
            {
                //retract();
                outputToken("op", "=");
            }
            state = 0;
            break;
        case 24:
            cat();
            getChar();
            if (ch == '=')
            {
                cat();
                getChar();
                outputToken("op", "!=");
            }
            else
            {
                //retract();
                outputToken("op", "!");
            }
            state = 0;
            break;
        case 25:
            cat();
            getChar();
            if (ch == '=')
            {
                cat();
                getChar();
                outputToken("op", ">=");
                state = 0;
            }
            else if (ch == '>')
                state = 26;
            else
            {
                //retract();
                outputToken("op", ">");
                state = 0;
            }
            break;
        case 26:
            cat();
            getChar();
            if (ch == '=')
            {
                cat();
                getChar();
                outputToken("op", ">>=");
            }
            else
            {
                //retract();
                outputToken("op", ">>");
            }
            state = 0;
            break;
        case 27:
            cat();
            getChar();
            if (ch == '=')
            {
                cat();
                getChar();
                outputToken("op", "<=");
                state = 0;
            }
            else if (ch == '<')
                state = 26;
            else
            {
                //retract();
                outputToken("op", "<");
                state = 0;
            }
            break;
        case 28:
            cat();
            getChar();
            if (ch == '=')
            {
                cat();
                getChar();
                outputToken("op", "<<=");
            }
            else
            {
                //retract();
                outputToken("op", "<<");
            }
            state = 0;
            break;
        case 29:
            cat();
            getChar();
            if (ch == '&')
            {
                cat();
                getChar();
                outputToken("op", "&&");
            }
            else if (ch == '=')
            {
                cat();
                getChar();
                outputToken("op", "&=");
            }
            else
            {
                //retract();
                outputToken("op", "&");
            }
            state = 0;
            break;
        case 30:
            cat();
            getChar();
            if (ch == '|')
            {
                cat();
                getChar();
                outputToken("op", "||");
            }
            else if (ch == '=')
            {
                cat();
                getChar();
                outputToken("op", "|=");
            }
            else
            {
                //retract();
                outputToken("op", "|");
            }
            state = 0;
            break;
        case 31:
            cat();
            getChar();
            if (ch == '=')
            {
                cat();
                getChar();
                outputToken("op", "^=");
            }
            else
            {
                //retract();
                outputToken("op", "^");
            }
            state = 0;
            break;
        case 32:
            cat();
            getChar();
            if (ch == '\'') {
                cat();
                getChar();
                state = 0;
                entry = numberInsert();
                outputToken("num", "index:" + to_string(entry));
            }
            else
                state = 33;
            break;
        case 33:
            cat();
            getChar();
            if (ch == '\'') {
                cat();
                state = 0;
                entry = numberInsert();
                outputToken("num", "index:" + to_string(entry));
                getChar();
            }
            else
                state = ERR_STATE;
            break;
        case 34:
            cat();
            getChar();
            if (ch == '\"') {
                cat();
                state = 0;
                entry = numberInsert();
                outputToken("num", "index:" + to_string(entry));
                getChar();
            }
            else
                state = 34;
            break;
        case ERR_STATE:
            error();
            getChar();
            break;
        default:
            state = ERR_STATE;
            break;
        }
    }
}

void getChar()
{
    if (forwardIndex == BUFFER_L_END){
        char temp;
        //读入字符串填充右半区
        for (int i = BUFFER_L_END + 1; i < BUFFER_R_END; i++){
            if (fin.eof())
            {
                buffer[i] = EOF;
                break;
            }
            temp = fin.get();
            charCnt++;
            if (temp == '\n') //计算程序行数
                lineCnt++;
            buffer[i] = temp;
        }
        ch = buffer[forwardIndex];
        forwardIndex++;
    }
    else if (forwardIndex == BUFFER_R_END){
        char temp;
        //读入字符串填充左半区
        for (int i = 0; i < BUFFER_L_END + 1; i++){
            if (fin.eof())
            {
                buffer[i] = EOF;
                break;
            }
            temp = fin.get();
            charCnt++;
            if (temp == '\n') //计算程序行数
                lineCnt++;
            buffer[i] = temp;
        }
        //注意这里的赋值顺序，不可以调转，不可以把ch的赋值提到函数最开头
        //一方面，缓冲区最初始的读入从这里开始
        //另一方面，BUFFER_R_END是没有存储内容的。
        forwardIndex = 0;
        ch = buffer[forwardIndex];
    }
    else{
        ch = buffer[forwardIndex];
        forwardIndex++;
    }        
}

void cat()
{
    token.append(1, ch);
}

bool isLetter()
{
    if ((ch >= 'a' && ch <= 'z') ||
        (ch >= 'A' && ch <= 'Z'))
        return true;
    else
        return false;
}

bool isDigit()
{
    if (ch >= '0' && ch <= '9')
        return true;
    else
        return false;
}

bool isOctDigit()
{
    if (ch >= '0' && ch <= '7')
        return true;
    else
        return false;
}

bool isHexDigit()
{
    if ((ch >= '0' && ch <= '9') ||
        (ch >= 'a' && ch <= 'f') ||
        (ch >= 'Z' && ch <= 'F'))
        return true;
    else
        return false;
}


int reserve()
{
    for (int i = 0; i < KEY_CNT; i++) {
        if (keyTable[i] == token) {
            return i;
        }
    }
    return -1;
}

int symbolInsert() {
    for (int i = 0; i < SYMBOL_SIZE; i++) {
        if (symbol[i] == "") {
            symbol[i] = token;
            return i;
        }
    }
    return -1;
}

int numberInsert() {
    for (int i = 0; i < NUMBER_SIZE; i++) {
        if (constant[i] == "") {
            constant[i] = token;
            return i;
        }
    }
    return -1;
}

int error() {
    int i;
    cat();
    for (i = 0; i < ERR_SIZE; i++) {
        if (errList[i] == 0) {
            errList[i] = forwardIndex - token.length();
            break;
        }
    }

    std::cout << "error " << i << " in index "
        << errList[i] << ": " << token << std::endl;
    state = 0;
    return i;
}

void outputToken(std::string token, std::string attr) {
    std::cout << "< " << token << " , " << attr << " >" << std::endl;
}

void outputData() {
    int i;
    //输出函数总行数
    std::cout << "\n\ntotal count of lines is: " << lineCnt << std::endl;
    std::cout << std::endl << std::endl;

    //输出字符总数
    std::cout << "\n\ntotal count of characters is: " << charCnt << std::endl;
    std::cout << std::endl << std::endl;

    //输出关键字个数
    std::cout << "  key\t|\tcount" << std::endl;
    for (i = 0; i < KEY_CNT; i++) {
        if (keyTotal[i] != 0) {
            std::cout << keyTable[i] << "\t:\t" << keyTotal[i] << std::endl;
        }
    }
    std::cout << std::endl << std::endl;

    //输出自定义标识符列表
    std::cout << " index\t|\tsymbol   " << std::endl;
    for (i = 0; i < SYMBOL_SIZE; i++) {
        if (symbol[i] == "")
            break;
        std::cout << i << "\t:\t" << symbol[i] << std::endl;
    }
    std::cout << "total count of symbol: " << i << std::endl;
    std::cout << std::endl << std::endl;

    //输出常量列表
    std::cout << " index\t|\tconstant   " << std::endl;
    for (i = 0; i < SYMBOL_SIZE; i++) {
        if (constant[i] == "")
            break;
        std::cout << i << "\t:\t" << constant[i] << std::endl;
    }
    std::cout << "total count of constant: " << i << std::endl;
    std::cout << std::endl << std::endl;
}
