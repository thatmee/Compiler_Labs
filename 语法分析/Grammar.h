#pragma once
/**
 * @file Grammar.h
 * @author NanYafeng
 * @brief Class definition of Grammar
 * @version 1.0
 * @date 2021-11-16
 * @details 包含了 Grammar 类的所有定义
 *          为方便实验验收，实现部分划分了几个不同的 .cpp
 */
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <map>
#include <iostream>
#include <iomanip>

using namespace std;

using Symbol = std::string;                                                            // 符号类型
using SymbolSet = std::set<Symbol>;                                                    // 符号集合类型
using VecSymbol = std::vector<Symbol>;                                                 // 符号的 vector 类型
using RHS = std::vector<Symbol>;                                                       // 右部产生式 (right hand side) 类型
using VectorRHS = std::vector<RHS>;                                                    // 右部产生式的集合
using UpdateMap = std::unordered_map<Symbol, SymbolSet>;                               // 存储更新式的类型
using SymbolToRHS = std::unordered_map<Symbol, RHS>;                                   // 符号到右部产生式的映射类型
using SymbolTable = std::unordered_map<Symbol, std::unordered_map<Symbol, RHS>>;       // 行列表头为 Symbol 的二维表（预测分析表类型）
using Productions = std::unordered_map<Symbol, VectorRHS>;                             // 产生式集合类型
using ProdSplit = std::pair<Symbol, RHS>;                                              // 单个产生式类型
using ProdCnt = std::unordered_map<int, ProdSplit>;                                    // 带编号的产生式集合类型
using Project = std::pair<ProdSplit, SymbolSet>;                                       // 项目类型
using ProjectMap = std::map<ProdSplit, SymbolSet>;                                     // 项目集合类型
using ProjectCluster = std::unordered_map<int, ProjectMap>;                            // 项目族类型
using ActGotoTable = std::unordered_map<int, std::unordered_map<Symbol, std::string>>; // LR1 分析表类型

class Grammar
{
private:
    SymbolSet N;                                  // 非终结符集合
    SymbolSet T;                                  // 终结符集合
    Productions P;                                // 所有产生式
    Symbol S;                                     // 文法起始符号 S
    VecSymbol s;                                  // 待分析的串
    int pointer = -1;                             // 当前指向的位置
    Symbol ch = "";                               // 当前指向的字符
    std::unordered_map<Symbol, SymbolSet> first;  // first 集合
    std::unordered_map<Symbol, SymbolSet> follow; // follow 集合
    SymbolTable LL1AnaTable;                      // LL1 分析表
    ProdCnt extensionP;                           // 带编号的拓广文法
    ActGotoTable LR1AnaTable;                     // LR1 分析表
    ProjectCluster C;                             // 规范项目族

    void inputS();                                           // 读入待分析的串
    void forwardPointer();                                   // 指针前移，并更新 ch 中的值
    void error(int);                                         // 错误处理函数
    void procE();                                            // 非终结符 E 的处理程序
    void procF();                                            // 非终结符 F 的处理程序
    void procT();                                            // 非终结符 T 的处理程序
    void buildFirst();                                       // 构造 first
    void firstOfOneSymbol(Symbol, SymbolSet &);              // 找某一个符号的 first 集合
    void firstOfVecSymbol(std::vector<Symbol>, SymbolSet &); // 找一连串符号的 first 集合
    void buildFollow();                                      // 构建 follow
    void recurUpdFollow(Symbol, UpdateMap &);                // 根据更新式递归更新 follow
    void outputFirstFollow();                                // 输出 first 和 follow
    void buildLL1AnaTable();                                 // 构造预测分析表
    void outputLL1AnaTable();                                // 输出 LL1 分析表
    void extensionGrammar();                                 // 拓广文法并编号
    int getProductCnt(ProdSplit &);                          // 得到产生式的编号
    void outputExtentionG();                                 // 输出拓广后的文法
    void closure(ProjectMap &, ProjectMap &);                // 构造项目集 I 的闭包，放在 resultI 中
    void go(ProjectMap, Symbol, ProjectMap &, int);          // 转移函数 go
    void buildProjCluster();                                 // 构造项目集规范族
    void outputProjCluster();                                // 输出项目集规范族
    void outputLR1AnaTable();                                // 输出 LR1 分析表

public:
    static const int ERR_MISSING_R_BRACKET = 1; // 错误状态代码
    static const int ERR_MISSING_OBJECT = 2;    // 错误状态代码
    const RHS ERR = {"ERR"};                    // 错误状态
    static const int SPLIT_LINE_WIDTH = 60;     // 用于格式化输出

    Grammar();
    ~Grammar() {}
    void input();             // 输入文法
    void output() const;      // 输出文法
    void recursiveAnalysis(); // 自顶向下递归调用分析程序
    void LL1Analysis();       // 自顶向下 LL(1) 分析程序
    void LR1Analysis();       // 自底向上 LR(1) 分析程序
};
