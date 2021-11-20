#include "Grammar.h"
#include <iostream>

/// @brief LR(1) ��������
void Grammar::LR1Analysis() {
    extensionGrammar();
    buildProjCluster();
    outputProjCluster();
    outputLR1AnaTable();

    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "������Ҫ�������ַ�����";
    inputS();


}

/// @brief �����Ŀ���淶��
void Grammar::outputProjCluster() {
    // ����������Ŀ��
    for (ProjectCluster::iterator iterPM = C.begin(); iterPM != C.end(); iterPM++) {
        ProjectMap curProjectSet = iterPM->second;
        std::cout << "I" << std::to_string(iterPM->first) << ":" << std::endl;
        // ����һ����Ŀ����������Ŀ
        for (ProjectMap::iterator iterP = curProjectSet.begin(); iterP != curProjectSet.end(); iterP++) {
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

/// @brief ������Ŀ�� I �ıհ������� resultI ��
/// @param I ������Ŀ��
/// @param resultI �����Ŀ��
void Grammar::closure(ProjectMap& I, ProjectMap& J) {
    J = I;
    ProjectMap JNew;
    do {
        JNew = J;
        // ���� JNew �е�ÿһ����Ŀ
        for (ProjectMap::iterator iterP = JNew.begin(); iterP != JNew.end(); iterP++) {
            ProdSplit curProd = iterP->first;
            RHS curProdRHS = curProd.second;
            // ��Լ��Ŀ�հ���������������չ
            if (curProdRHS.back() == ".")
                continue;
            RHS::iterator pointLoc = find(curProdRHS.begin(), curProdRHS.end(), ".");
            // �ҵ��� . 
            if (pointLoc != curProdRHS.end()) {
                RHS::iterator nextLoc = pointLoc + 1;
                // .����һ�������Ƿ��ս��
                if (N.find(*nextLoc) != N.end()) {
                    VecSymbol beta = VecSymbol(nextLoc + 1, curProdRHS.end());
                    SymbolSet tmpFirstSet;
                    firstOfVecSymbol(beta, tmpFirstSet);
                    // first(beta) ���пգ�����betaΪ�գ�����ǰ��Ŀ����ǰ�����ż����µ���Ŀ����ǰ������
                    if (beta.size() == 0 || tmpFirstSet.find("~") != tmpFirstSet.end())
                        tmpFirstSet.insert(iterP->second.begin(), iterP->second.end());
                    // ����.����ķ��ս�������в���ʽ
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

/// @brief ת�ƺ��� go��resultI = go[I, X]
/// @param Inumber ������Ŀ�� I ����ţ����ڹ��������
void Grammar::go(ProjectMap I, Symbol X, ProjectMap& resultI, int Inumber) {
    ProjectMap J;
    // ����ÿһ����Ŀ
    for (ProjectMap::iterator iterP = I.begin(); iterP != I.end(); iterP++) {
        ProdSplit curProd = iterP->first;
        RHS curProdRHS = curProd.second;
        // ��Լ��Ŀ
        if (curProdRHS.back() == ".") {
            // ���·�����
            // ���� S'������ action ��Ϊ���ܶ���
            if (curProd.first == "S'")
                LR1AnaTable[Inumber]["$"] = "ACC";
            // �󲿲��� S'������ action ��Ϊ��Լ����
            else {
                curProd.second.pop_back(); // ȥ��ĩβ�ĵ�
                for (SymbolSet::iterator iterS = iterP->second.begin(); iterS != iterP->second.end(); iterS++)
                    LR1AnaTable[Inumber][*iterS] = "R" + std::to_string(getProductCnt(curProd));
            }
            // ��Լ��Ŀû�к�̣�������������
            continue;
        }
        RHS::iterator pointLoc = find(curProdRHS.begin(), curProdRHS.end(), ".");
        // �ҵ��� .
        if (pointLoc != curProdRHS.end()) {
            RHS::iterator nextLoc = pointLoc + 1;
            if (*nextLoc == X) {
                // �� . �����ƶ�һλ
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

/// @brief ��ȡ����ʽ�����
int Grammar::getProductCnt(ProdSplit& p) {
    for (ProdCnt::iterator iterPC = extensionP.begin(); iterPC != extensionP.end(); iterPC++) {
        if (p == iterPC->second)
            return iterPC->first;
    }
}

/// @brief �ع��ķ������ع����ķ�����ʽ���б��
/// @param resultP �ع����ķ�
void Grammar::extensionGrammar() {
    // �� P ������ת��Ϊ�����Ĳ���ʽ�洢�� extensionP �����
    extensionP[0] = ProdSplit("S'", { {S} });
    int number = 1; // ��� 0 Ϊ�ع�������ʼ�� S'
    for (SymbolSet::iterator iterN = N.begin(); iterN != N.end(); iterN++) {
        for (VectorRHS::iterator iterRHS = P[*iterN].begin(); iterRHS != P[*iterN].end(); iterRHS++)
            extensionP[number++] = ProdSplit(*iterN, *iterRHS);
    }
    // �����µ���ʼ�� S'���޸���ص�һЩ����
    N.insert("S'");
    P["S'"] = { {S} };
    S = "S'";
}

/// @brief ���� LR(1) ��Ŀ���淶��
void Grammar::buildProjCluster() {
    ProjectMap I0;
    ProjectMap st;
    ProdSplit start = extensionP[0];
    start.second.insert(start.second.begin(), ".");
    st[start].insert("$");
    closure(st, I0);
    C[0] = I0;

    int cnt = 1;
    ProjectCluster CNew;
    do {
        CNew = C;
        // ����ÿһ����Ŀ��
        for (ProjectCluster::iterator iterPM = C.begin(); iterPM != C.end(); iterPM++) {
            // �����ս��
            for (SymbolSet::iterator iterT = T.begin(); iterT != T.end(); iterT++) {
                ProjectMap tmpPM;
                go(iterPM->second, *iterT, tmpPM, iterPM->first);
                if (tmpPM.size() != 0) {
                    // �鿴 C����û�������Ŀ��
                    int isExist = 0;
                    ProjectCluster::iterator iterPos;
                    for (iterPos = C.begin(); iterPos != C.end(); iterPos++) {
                        if (tmpPM == iterPos->second) {
                            isExist = 1;
                            break;
                        }
                    }
                    // ��� go[I,X] ���� C ��
                    if (!isExist) {
                        // ���µ���Ŀ�����뵽 C
                        C.insert(std::pair<int, ProjectMap>(cnt, tmpPM));
                        // ���� action ��
                        LR1AnaTable[iterPM->first][*iterT] = "S" + std::to_string(cnt);
                        cnt++;
                    }
                    // ��� go[I,X] �� C �У����� action ��
                    if (isExist) {
                        LR1AnaTable[iterPM->first][*iterT] = "S" + std::to_string(iterPos->first);
                    }
                }
            }
            //�������ս��
            for (SymbolSet::iterator iterN = N.begin(); iterN != N.end(); iterN++) {
                ProjectMap tmpPM;
                go(iterPM->second, *iterN, tmpPM, iterPM->first);
                if (tmpPM.size() != 0) {
                    // �鿴 C����û�������Ŀ��
                    int isExist = 0;
                    ProjectCluster::iterator iterPos;
                    for (iterPos = C.begin(); iterPos != C.end(); iterPos++) {
                        if (tmpPM == iterPos->second) {
                            isExist = 1;
                            break;
                        }
                    }
                    // ��� go[I,X] ��Ϊ�գ��Ҳ��� C ��
                    if (!isExist) {
                        // ���µ���Ŀ�����뵽 C
                        C.insert(std::pair<int, ProjectMap>(cnt, tmpPM));
                        // ���� goto ��
                        LR1AnaTable[iterPM->first][*iterN] = std::to_string(cnt);
                        cnt++;
                    }

                    // ��� go[I,X] �� C �У����� goto ��
                    if (isExist) {
                        LR1AnaTable[iterPM->first][*iterN] = std::to_string(iterPos->first);
                    }
                }
            }
        }
    } while (CNew.size() != C.size());
}

/// @brief ���� LR(1) ������
void Grammar::buildLR1AnaTable() {

}

/// @brief ��� LR(1) ������
void Grammar::outputLR1AnaTable() {
    // �����ͷ
    std::cout << "state\t";
    for (SymbolSet::iterator iterS = T.begin(); iterS != T.end(); iterS++)
        std::cout << *iterS << "\t";
    std::cout << "$\t";
    for (SymbolSet::iterator iterS = N.begin(); iterS != N.end(); iterS++) {
        if (*iterS != "S'")
            std::cout << *iterS << "\t";
    }

    std::cout << std::endl;
    // �������������
    for (int i = 0; i < LR1AnaTable.size(); i++) {
        std::cout << i << "\t";
        // �������ս����� action
        for (SymbolSet::iterator iterS = T.begin(); iterS != T.end(); iterS++) {
            std::unordered_map<Symbol, std::string>::iterator loc = LR1AnaTable[i].find(*iterS);
            if (loc != LR1AnaTable[i].end())
                std::cout << loc->second;
            std::cout << "\t";
        }
        // �� $ ��� action
        std::unordered_map<Symbol, std::string>::iterator loc = LR1AnaTable[i].find("$");
        if (loc != LR1AnaTable[i].end())
            std::cout << loc->second;
        std::cout << "\t";
        // �Է��ս����� goto
        for (SymbolSet::iterator iterS = N.begin(); iterS != N.end(); iterS++) {
            std::unordered_map<Symbol, std::string>::iterator loc = LR1AnaTable[i].find(*iterS);
            if (loc != LR1AnaTable[i].end())
                std::cout << loc->second;
            std::cout << "\t";
        }
        std::cout << std::endl;
    }
}
