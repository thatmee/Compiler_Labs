/**
 * @file 3_LR(1)_analysis.cpp
 * @author NanYafeng
 * @brief LR(1) 分析程序相关的函数实现
 * @version 1.0
 * @date 2021-11-21
 */
#include "Grammar.h"

 /// @brief LR(1) 分析程序
void Grammar::LR1Analysis()
{
    extensionGrammar();
    outputExtentionG();
    buildProjCluster();
    outputProjCluster();
    outputLR1AnaTable();

    std::cout << std::endl
        << setfill('-') << setw(Grammar::SPLIT_LINE_WIDTH) << "" << std::endl;
    std::cout << "请输入要分析的字符串，符号之间使用空格分隔：";
    inputS();
    std::cout << std::endl
        << setfill('-') << setw(Grammar::SPLIT_LINE_WIDTH) << "" << std::endl;
    std::cout << "分析过程如下：" << std::endl;

    std::vector<int> stateStack;
    VecSymbol symbolStack;
    stateStack.push_back(0);
    symbolStack.push_back("-");
    forwardPointer();
    int stateS = 0;
    do
    {
        stateS = stateStack.back();
        // 错误状态
        if (LR1AnaTable[stateS].find(ch) == LR1AnaTable[stateS].end())
            error(0);
        // 分析表中有相应的动作或 goto 目标
        else
        {
            std::string action = LR1AnaTable[stateS][ch];
            // 移进动作
            if (action[0] == 'S')
            {
                // 将当前符号压入符号栈
                symbolStack.push_back(ch);
                // 将转移后的状态压入状态栈
                stateStack.push_back(std::stoi(action.substr(1)));
                // 前移指针
                forwardPointer();
            }
            // 归约动作
            else if (action[0] == 'R')
            {
                // 获取归约所使用的产生式 A -> β
                int prodN = std::stoi(action.substr(1));
                ProdSplit p = extensionP[prodN];
                // 从符号栈顶和状态栈顶弹出 |β| 个符号
                int lenBeta = p.second.size();
                for (int i = 0; i < lenBeta; i++)
                {
                    stateStack.pop_back();
                    symbolStack.pop_back();
                }
                // 将 A 压入符号栈
                symbolStack.push_back(p.first);
                // 把经过 A 到达的状态压入状态栈
                int curS = stateStack.back();
                stateStack.push_back(std::stoi(LR1AnaTable[curS][p.first]));
                // 输出产生式 A -> β
                std::cout << p.first << " -> ";
                for (RHS::iterator iterRHS = p.second.begin(); iterRHS != p.second.end(); iterRHS++)
                    std::cout << *iterRHS << " ";
                std::cout << std::endl;
            }
            // 接受动作
            else if (action == "ACC")
                break;
        }
    } while (1);
    std::cout << "识别成功！";
}

/// @brief 获取产生式的序号
int Grammar::getProductCnt(ProdSplit& p)
{
    for (ProdCnt::iterator iterPC = extensionP.begin(); iterPC != extensionP.end(); iterPC++)
    {
        if (p == iterPC->second)
            return iterPC->first;
    }
}

/// @brief 拓广文法并对拓广后的文法产生式进行编号
/// @param resultP 拓广后的文法
void Grammar::extensionGrammar()
{
    // 将 P 内数据转换为单独的产生式存储到 extensionP 并编号
    extensionP[0] = ProdSplit("S'", { {S} });
    int number = 1; // 编号 0 为拓广后的新起始符 S'
    for (SymbolSet::iterator iterN = N.begin(); iterN != N.end(); iterN++)
    {
        for (VectorRHS::iterator iterRHS = P[*iterN].begin(); iterRHS != P[*iterN].end(); iterRHS++)
            extensionP[number++] = ProdSplit(*iterN, *iterRHS);
    }
    // 加入新的起始符 S'，修改相关的一些变量
    N.insert("S'");
    P["S'"] = { {S} };
    S = "S'";
}

/// @brief 构造项目集 I 的闭包，放在 resultI 中
/// @param I 输入项目集
/// @param resultI 结果项目集
void Grammar::closure(ProjectMap& I, ProjectMap& J)
{
    J = I;
    ProjectMap JNew;
    do
    {
        JNew = J;
        // 遍历 JNew 中的每一个项目
        for (ProjectMap::iterator iterP = JNew.begin(); iterP != JNew.end(); iterP++)
        {
            ProdSplit curProd = iterP->first;
            RHS curProdRHS = curProd.second;
            // 归约项目闭包是自身，不用再扩展
            if (curProdRHS.back() == ".")
                continue;
            RHS::iterator pointLoc = find(curProdRHS.begin(), curProdRHS.end(), ".");
            // 找到了 .
            if (pointLoc != curProdRHS.end())
            {
                RHS::iterator nextLoc = pointLoc + 1;
                // .的下一个符号是非终结符
                if (N.find(*nextLoc) != N.end())
                {
                    VecSymbol beta = VecSymbol(nextLoc + 1, curProdRHS.end());
                    SymbolSet tmpFirstSet;
                    firstOfVecSymbol(beta, tmpFirstSet);
                    // first(beta) 中有空，或者beta为空，将当前项目的向前看符号加入新的项目的向前看符号
                    if (beta.size() == 0 || tmpFirstSet.find("~") != tmpFirstSet.end())
                        tmpFirstSet.insert(iterP->second.begin(), iterP->second.end());
                    // 遍历.后面的非终结符的所有产生式
                    for (VectorRHS::iterator iterRHS = P[*nextLoc].begin(); iterRHS != P[*nextLoc].end(); iterRHS++)
                    {
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
/// @param Inumber 代表项目集 I 的序号，用于构造分析表
void Grammar::go(ProjectMap I, Symbol X, ProjectMap& resultI, int Inumber)
{
    ProjectMap J;
    // 遍历每一个项目
    for (ProjectMap::iterator iterP = I.begin(); iterP != I.end(); iterP++)
    {
        ProdSplit curProd = iterP->first;
        RHS curProdRHS = curProd.second;
        // 归约项目
        if (curProdRHS.back() == ".")
        {
            // 更新分析表
            // 左部是 S'，更新 action 表为接受动作
            if (curProd.first == "S'")
                LR1AnaTable[Inumber]["$"] = "ACC";
            // 左部不是 S'，更新 action 表为归约动作
            else
            {
                curProd.second.pop_back(); // 去掉末尾的点
                for (SymbolSet::iterator iterS = iterP->second.begin(); iterS != iterP->second.end(); iterS++)
                    LR1AnaTable[Inumber][*iterS] = "R" + std::to_string(getProductCnt(curProd));
            }
            // 归约项目没有后继，跳过后续处理
            continue;
        }
        RHS::iterator pointLoc = find(curProdRHS.begin(), curProdRHS.end(), ".");
        // 找到了 .
        if (pointLoc != curProdRHS.end())
        {
            RHS::iterator nextLoc = pointLoc + 1;
            if (*nextLoc == X)
            {
                // 将 . 向右移动一位
                curProdRHS.erase(pointLoc);
                RHS::iterator XLoc = find(curProdRHS.begin(), curProdRHS.end(), X);
                curProdRHS.insert(XLoc + 1, ".");
                ProdSplit tmpProd = ProdSplit(curProd.first, curProdRHS);
                J.insert(Project(tmpProd, iterP->second));
            }
        }
    }
    closure(J, resultI);
}

/// @brief 构造 LR(1) 项目集规范族
void Grammar::buildProjCluster()
{
    ProjectMap I0;
    ProjectMap st;
    ProdSplit start = extensionP[0];
    start.second.insert(start.second.begin(), ".");
    st[start].insert("$");
    closure(st, I0);
    C[0] = I0;

    int cnt = 1;
    ProjectCluster CNew;
    do
    {
        CNew = C;
        // 遍历每一个项目集
        for (ProjectCluster::iterator iterPM = C.begin(); iterPM != C.end(); iterPM++)
        {
            // 遍历终结符
            for (SymbolSet::iterator iterT = T.begin(); iterT != T.end(); iterT++)
            {
                ProjectMap tmpPM;
                go(iterPM->second, *iterT, tmpPM, iterPM->first);
                if (tmpPM.size() != 0)
                {
                    // 查看 C中有没有这个项目集
                    int isExist = 0;
                    ProjectCluster::iterator iterPos;
                    for (iterPos = C.begin(); iterPos != C.end(); iterPos++)
                    {
                        if (tmpPM == iterPos->second)
                        {
                            isExist = 1;
                            break;
                        }
                    }
                    // 如果 go[I,X] 不在 C 中
                    if (!isExist)
                    {
                        // 将新的项目集插入到 C
                        C.insert(std::pair<int, ProjectMap>(cnt, tmpPM));
                        // 更新 action 表
                        LR1AnaTable[iterPM->first][*iterT] = "S" + std::to_string(cnt);
                        cnt++;
                    }
                    // 如果 go[I,X] 在 C 中，更新 action 表
                    if (isExist)
                    {
                        LR1AnaTable[iterPM->first][*iterT] = "S" + std::to_string(iterPos->first);
                    }
                }
            }
            //遍历非终结符
            for (SymbolSet::iterator iterN = N.begin(); iterN != N.end(); iterN++)
            {
                ProjectMap tmpPM;
                go(iterPM->second, *iterN, tmpPM, iterPM->first);
                if (tmpPM.size() != 0)
                {
                    // 查看 C中有没有这个项目集
                    int isExist = 0;
                    ProjectCluster::iterator iterPos;
                    for (iterPos = C.begin(); iterPos != C.end(); iterPos++)
                    {
                        if (tmpPM == iterPos->second)
                        {
                            isExist = 1;
                            break;
                        }
                    }
                    // 如果 go[I,X] 不为空，且不在 C 中
                    if (!isExist)
                    {
                        // 将新的项目集插入到 C
                        C.insert(std::pair<int, ProjectMap>(cnt, tmpPM));
                        // 更新 goto 表
                        LR1AnaTable[iterPM->first][*iterN] = std::to_string(cnt);
                        cnt++;
                    }

                    // 如果 go[I,X] 在 C 中，更新 goto 表
                    if (isExist)
                    {
                        LR1AnaTable[iterPM->first][*iterN] = std::to_string(iterPos->first);
                    }
                }
            }
        }
    } while (CNew.size() != C.size());
}

/// @brief 输出 LR(1) 分析表
void Grammar::outputLR1AnaTable()
{
    std::cout << std::endl
        << setfill('-') << setw(Grammar::SPLIT_LINE_WIDTH) << "" << std::endl;
    std::cout << "LR(1) 分析表为：" << std::endl;
    // 输出表头
    std::cout << "state\t";
    for (SymbolSet::iterator iterS = T.begin(); iterS != T.end(); iterS++)
        std::cout << *iterS << "\t";
    std::cout << "$\t";
    for (SymbolSet::iterator iterS = N.begin(); iterS != N.end(); iterS++)
    {
        if (*iterS != "S'")
            std::cout << *iterS << "\t";
    }

    std::cout << std::endl;
    // 输出分析表内容
    for (int i = 0; i < LR1AnaTable.size(); i++)
    {
        std::cout << i << "\t";
        // 对所有终结符输出 action
        for (SymbolSet::iterator iterS = T.begin(); iterS != T.end(); iterS++)
        {
            std::unordered_map<Symbol, std::string>::iterator loc = LR1AnaTable[i].find(*iterS);
            if (loc != LR1AnaTable[i].end())
                std::cout << loc->second;
            std::cout << "\t";
        }
        // 对 $ 输出 action
        std::unordered_map<Symbol, std::string>::iterator loc = LR1AnaTable[i].find("$");
        if (loc != LR1AnaTable[i].end())
            std::cout << loc->second;
        std::cout << "\t";
        // 对非终结符输出 goto
        for (SymbolSet::iterator iterS = N.begin(); iterS != N.end(); iterS++)
        {
            if (*iterS != "S'")
            {
                std::unordered_map<Symbol, std::string>::iterator loc = LR1AnaTable[i].find(*iterS);
                if (loc != LR1AnaTable[i].end())
                    std::cout << loc->second;
                std::cout << "\t";
            }
        }
        std::cout << std::endl;
    }
}

/// @brief 输出拓广后的文法
void Grammar::outputExtentionG()
{
    std::cout << std::endl
        << setfill('-') << setw(Grammar::SPLIT_LINE_WIDTH) << "" << std::endl;
    std::cout << "拓广后的文法为：" << std::endl;
    for (int i = 0; i < extensionP.size(); i++)
    {
        std::cout << "(" << i << ")  " << extensionP[i].first << " -> ";
        for (RHS::iterator iterRHS = extensionP[i].second.begin(); iterRHS != extensionP[i].second.end(); iterRHS++)
            std::cout << *iterRHS << " ";
        std::cout << std::endl;
    }
}

/// @brief 输出项目集规范族
void Grammar::outputProjCluster()
{
    std::cout << std::endl
        << setfill('-') << setw(Grammar::SPLIT_LINE_WIDTH) << "" << std::endl;
    std::cout << "项目集规范族为：" << std::endl;
    // 遍历所有项目集
    for (ProjectCluster::iterator iterPM = C.begin(); iterPM != C.end(); iterPM++)
    {
        ProjectMap curProjectSet = iterPM->second;
        std::cout << "I" << std::to_string(iterPM->first) << ":" << std::endl;
        // 遍历一个项目集的所有项目
        for (ProjectMap::iterator iterP = curProjectSet.begin(); iterP != curProjectSet.end(); iterP++)
        {
            ProdSplit curProd = iterP->first;
            SymbolSet curLookAhead = iterP->second;
            std::cout << curProd.first << " -> ";
            for (RHS::iterator iterS = curProd.second.begin(); iterS != curProd.second.end(); iterS++)
                std::cout << *iterS << " ";
            std::cout << "\t";
            for (SymbolSet::iterator iterS = curLookAhead.begin(); iterS != curLookAhead.end(); iterS++)
                std::cout << *iterS << " ";
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}
