#include "Grammar.h"

/// @brief LR(1) 分析程序
void Grammar::LR1Analysis() {

    extensionGrammar();
    ProdSplit a = ProdSplit("S'", { ".", "E" });
    ProjectMap test;
    test[a].insert("$");
    ProjectMap result;
    closure(test, result);

    buildProjCluster();
    buildLR1AnaTable();

}

/// @brief 构造项目集 I 的闭包，放在 resultI 中
/// @param I 输入项目集
/// @param resultI 结果项目集
void Grammar::closure(ProjectMap& I, ProjectMap& J) {
    J = ProjectMap(I.begin(), I.end());
    ProjectMap JNew;
    do {
        JNew = ProjectMap(J.begin(), J.end());
        // 遍历 JNew 中的每一个项目
        for (ProjectMap::iterator iterP = JNew.begin(); iterP != JNew.end(); iterP++) {
            ProdSplit curProd = iterP->first;
            RHS curProdRHS = curProd.second;
            // 归约项目闭包是自身，不用再扩展
            if (curProdRHS.back() == ".")
                continue;
            RHS::iterator pointLoc = find(curProdRHS.begin(), curProdRHS.end(), ".");
            // 找到了 . 
            if (pointLoc != curProdRHS.end()) {
                RHS::iterator nextLoc = pointLoc + 1;
                // .的下一个符号是非终结符
                if (N.find(*nextLoc) != N.end()) {
                    VecSymbol beta = VecSymbol(nextLoc + 1, curProdRHS.end());
                    SymbolSet tmpFirstSet;
                    firstOfVecSymbol(beta, tmpFirstSet);
                    // first(beta) 中有空，或者beta为空，将当前项目的向前看符号加入新的项目的向前看符号
                    if (beta.size() == 0 || tmpFirstSet.find("~") != tmpFirstSet.end())
                        tmpFirstSet.insert(iterP->second.begin(), iterP->second.end());
                    // 遍历.后面的非终结符的所有产生式
                    for (VectorRHS::iterator iterRHS = P[*nextLoc].begin(); iterRHS != P[*nextLoc].end(); iterRHS++) {
                        RHS tmpRHS = *iterRHS;
                        tmpRHS.insert(tmpRHS.begin(), ".");
                        ProdSplit tmpProd = ProdSplit(*nextLoc, tmpRHS);
                        J[tmpProd].insert(tmpFirstSet.begin(), tmpFirstSet.end());
                    }
                }
            }
        }
    } while (JNew.size() != J.size());
}

/// @brief 转移函数 go，resultI = go[I, X]
void Grammar::go(ProjectMap I, Symbol X, ProjectMap& resultI) {

}

/// @brief 拓广文法并对拓广后的文法产生式进行编号
/// @param resultP 拓广后的文法
void Grammar::extensionGrammar() {
    // 将 P 内数据转换为单独的产生式存储到 extensionP 并编号
    extensionP[0] = ProdSplit("S'", { {S} });
    int number = 1; // 编号 0 为拓广后的新起始符 S'
    for (SymbolSet::iterator iterN = N.begin(); iterN != N.end(); iterN++) {
        for (VectorRHS::iterator iterRHS = P[*iterN].begin(); iterRHS != P[*iterN].end(); iterRHS++)
            extensionP[number++] = ProdSplit(*iterN, *iterRHS);
    }
    // 加入新的起始符 S'，修改相关的一些变量
    N.insert("S'");
    P["S'"] = { {S} };
    S = "S'";
}

/// @brief 构造 LR(1) 项目集规范簇
void Grammar::buildProjCluster() {

}

/// @brief 构造 LR(1) 分析表
void Grammar::buildLR1AnaTable() {

}
