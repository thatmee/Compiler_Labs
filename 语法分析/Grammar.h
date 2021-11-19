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

using namespace std;

using Symbol = std::string;
using SymbolSet = std::set<Symbol>; // 符号集合类型
using RHS = std::vector<Symbol>;    // 右部产生式 (right hand side) 类型
using VecSymbol = std::vector<Symbol>;
using VectorRHS = std::vector<RHS>; // 右部产生式的集合
using Productions = std::unordered_map<Symbol, VectorRHS>; // 产生式集合类型
using UpdateMap = std::unordered_map<Symbol, SymbolSet>;
using SymbolToRHS = std::unordered_map<Symbol, RHS>;
using SymbolTable = std::unordered_map<Symbol, std::unordered_map<Symbol, RHS>>; // 行列表头为 Symbol 的二维表（预测分析表类型）

using ProdSplit = std::pair<Symbol, RHS>;
using ProdCnt = std::unordered_map<int, ProdSplit>;
using Project = std::pair<ProdSplit, VecSymbol>;
using ProjectMap = std::unordered_map<ProdSplit, VecSymbol>;
//using ProjectSet = std::set<Project>;
using ProjectCluster = std::unordered_map<int, ProjectMap>;
using ActGotoTable = std::unordered_map<int, std::unordered_map<Symbol, std::string>>;

class Grammar
{
private:
    SymbolSet N; // 非终结符集合
    SymbolSet T; // 终结符集合
    Productions P;       // 所有产生式
    Symbol S;        // 文法起始符号 S
    VecSymbol s; // 待分析的串
    int pointer = -1;   // 当前指向的位置
    Symbol ch = "";            // 当前指向的字符

    std::unordered_map<Symbol, SymbolSet> first; // first 集合
    std::unordered_map<Symbol, SymbolSet> follow; // follow 集合
    SymbolTable LL1AnaTable;

    ProdCnt extensionP;

    static const int ERR_MISSING_R_BRACKET = 1; // 错误状态代码
    static const int ERR_MISSING_OBJECT = 2;    // 错误状态代码
    const RHS ERR = { "ERR" };


    void inputS();
    void forwardPointer();
    void error(int);
    void procE();
    void procF();
    void procT();


    int removDirectRecur();   // 消除直接左递归
    int extractFactor();      // 提取左公因子
    void rewriteGrammar();    // 改写文法
    void buildFirst();
    void firstOfOneSymbol(Symbol, SymbolSet&);
    void firstOfVecSymbol(std::vector<Symbol>, SymbolSet&);
    void buildFollow();
    void recurUpdFollow(Symbol, UpdateMap&);
    void buildLL1AnaTable(); // 构造预测分析表

    void closure(ProjectMap&, ProjectMap&);
    void go(ProjectMap, Symbol, ProjectMap&);
    void extensionGrammar();
    void buildProjCluster();
    void buildLR1AnaTable();


public:
    Grammar();
    //Grammar(SymbolSet _N, SymbolSet _T, Productions _P, std::string _S) : N(_N), T(_T), P(_P), S(_S) {};
    ~Grammar() {}
    void input();
    void output() const;      // 输出文法
    void recursiveAnalysis(); // 自顶向下递归调用分析程序
    void LL1Analysis();       // 自顶向下 LL(1) 分析程序
    void LR1Analysis();       // 自底向上 LR(1) 分析程序
};
