#include "Grammar.h"
#include <iostream>

const int FIRST_O_WIDTH = 20;
const int TABLE_O_WIDTH = 20;

/// @brief LL(1) ��������
void Grammar::LL1Analysis() {
    // rewriteGrammar();
    buildFirst();
    buildFollow();
    buildLL1AnaTable();
    outputFirstFollow();
    outputAnaTable();

    std::cout << std::endl << setfill('-') << setw(Grammar::SPLIT_LINE_WIDTH) << "" << std::endl;
    std::cout << "������Ҫ�������ַ�����";
    inputS();
    VecSymbol symbolStack;
    symbolStack.push_back("$");
    symbolStack.push_back(S);
    forwardPointer();
    Symbol X;
    do {
        X = symbolStack.back();
        // X ���ս���Ż��� $
        if (T.find(X) != T.end() || X == "$") {
            if (X == ch) {
                symbolStack.pop_back();
                forwardPointer();
            }
            else
                error(0);
        }
        else {
            if (LL1AnaTable[X][ch] == ERR)
                error(0);
            else {
                symbolStack.pop_back();
                // ���Ҳ�����ʽ����ѹ��ջ��������ջ
                for (RHS::reverse_iterator symbol = LL1AnaTable[X][ch].rbegin(); symbol != LL1AnaTable[X][ch].rend() && *symbol != "~"; symbol++)
                    symbolStack.push_back(*symbol);
                // �������ʽ
                std::cout << X << "->";
                for (RHS::iterator symbol = LL1AnaTable[X][ch].begin(); symbol != LL1AnaTable[X][ch].end(); symbol++)
                    std::cout << *symbol;
                std::cout << std::endl;
            }
        }
    } while (X != "$");
    std::cout << "ʶ��ɹ���" << std::endl;
}

/// @brief ���� first ����
void Grammar::buildFirst() {
    // �������еķ��ս��
    for (SymbolSet::iterator iter = N.begin(); iter != N.end(); iter++) {
        VectorRHS rhss = P[(*iter)];
        // ����һ�����ս���������Ҳ�����ʽ
        for (int j = 0; j < rhss.size(); j++) {
            RHS oneRHS = rhss[j];
            SymbolSet firstOfOneRHS;
            // �ҵ���ǰ�Ҳ�����ʽ�� first ���ϣ������� firstOfOneRHS ��
            firstOfVecSymbol(oneRHS, firstOfOneRHS);
            // ����ǰ�Ҳ�����ʽ�� first ���ϼ��뵽��Ӧ�󲿵� first ������
            first[*iter].insert(firstOfOneRHS.begin(), firstOfOneRHS.end());
        }
    }
}

/// @brief ��һ�������ŵ� first ����
/// @param std::vector<Symbol> vecSymbol �����һ��������
/// @param SymbolSet& firstSet �������ļ��� 
void Grammar::firstOfVecSymbol(std::vector<Symbol> vecSymbol, SymbolSet& firstSet) {
    // �������Ŵ������з���
    for (int i = 0; i < vecSymbol.size(); i++) {
        SymbolSet tmpFirstSet;
        // �ӷ��Ŵ��ĵ�һ�����ſ�ʼ�� first ��
        firstOfOneSymbol(vecSymbol[i], tmpFirstSet);
        firstSet.insert(tmpFirstSet.begin(), tmpFirstSet.end());
        // �����Ե�ǰ�����ҵ��� first ����û�� ~�������Ѱ��
        if (tmpFirstSet.find("~") == tmpFirstSet.end())
            break;
    }
}

/// @brief ��ĳһ�����ŵ� first ����
/// @param a ��Ҫ���� first ���ϵķ���
/// @param firstSet �������ļ���
void Grammar::firstOfOneSymbol(Symbol a, SymbolSet& firstSet) {
    // Ҫ�����ķ������ս�������� first ������������
    if (T.find(a) != T.end()) {
        firstSet.insert(a);
        return;
    }
    // Ҫ�����ķ����Ƿ��ս�����ݹ���� first ����
    else {
        VectorRHS rhss = P[a];
        for (int i = 0; i < rhss.size(); i++) {
            RHS oneRHS = rhss[i];
            firstOfVecSymbol(oneRHS, firstSet);
        }
    }
}

/// @brief ���� follow ����
void Grammar::buildFollow() {
    // ���� follow ��֮��ĸ���ʽ
    UpdateMap update;
    // �������в���ʽ����ɣ�
    // 1. ����� first ������ص����� follow ���ϸ���
    // 2. ������ follow ��֮����໥����ʽ��������
    for (SymbolSet::iterator iter = N.begin(); iter != N.end(); iter++) {
        VectorRHS rhss = P[(*iter)];
        // ����һ�����ս���������Ҳ�����ʽ
        for (VectorRHS::iterator iterJ = rhss.begin(); iterJ != rhss.end(); iterJ++) {
            RHS oneRHS = *iterJ;
            // ����һ���Ҳ�����ʽ�����з���
            for (RHS::iterator iterK = oneRHS.begin(); iterK != oneRHS.end(); iterK++) {
                // ��ÿһ�����ս������������ķ��Ŵ��� first ���ϸ������� follow ��
                if (N.find(*iterK) != N.end()) {
                    SymbolSet tmpFirstSet;
                    std::vector<Symbol> behind = std::vector<Symbol>(iterK + 1, oneRHS.end());
                    firstOfVecSymbol(behind, tmpFirstSet);
                    follow[*iterK].insert(tmpFirstSet.begin(), tmpFirstSet.end());
                }
            }
            // ���ű���һ���Ҳ�����ʽ�����з���
            for (RHS::reverse_iterator iterP = oneRHS.rbegin(); iterP != oneRHS.rend(); iterP++) {
                // ����������ս��������������ʽ
                if (T.find(*iterP) != T.end())
                    break;
                // �����˷��ս��
                else
                {
                    // ��ǰ���ս�� ��= ��ʱ�������µĸ���ʽ����ǰ�󲿵� follow ��������ǰ���ս��
                    if (*iterP != *iter)
                        update[*iter].insert(*iterP);
                    // ��ǰ���ս�������Ƴ��գ����ټ�����������ʽ��������Ҫ������ǰɨ��
                    SymbolSet tmpFirstSet;
                    firstOfOneSymbol(*iterP, tmpFirstSet);
                    if (tmpFirstSet.find("~") == tmpFirstSet.end())
                        break;
                }
            }
        }
    }
    // �� $ ���Ÿ�����ʼ���ŵ� follow ��
    follow[S].insert("$");
    // ���ݸ���ʽ���ϸ��� follow ��
    for (UpdateMap::iterator iter = update.begin(); iter != update.end(); iter++)
        recurUpdFollow(iter->first, update);
    // follow ��ȥ��
    for (SymbolSet::iterator iter = N.begin(); iter != N.end(); iter++) {
        follow[*iter].erase("~");
    }
}

/// @brief ��a���ſ�ʼ�ݹ���� follow ��
void Grammar::recurUpdFollow(Symbol a, UpdateMap& update) {
    UpdateMap::iterator aLoc = update.find(a);
    if (aLoc == update.end())
        return;
    for (SymbolSet::iterator iter = aLoc->second.begin(); iter != aLoc->second.end(); iter++) {
        follow[*iter].insert(follow[a].begin(), follow[a].end());
        recurUpdFollow(*iter, update);
    }
}

/// @brief ����Ԥ�������
void Grammar::buildLL1AnaTable() {
    // �������в���ʽ
    for (SymbolSet::iterator iterLeft = N.begin(); iterLeft != N.end(); iterLeft++) {
        VectorRHS rhss = P[(*iterLeft)];
        // ����һ�����ս���������Ҳ�����ʽ
        for (VectorRHS::iterator iterRhs = rhss.begin(); iterRhs != rhss.end(); iterRhs++) {
            RHS oneRHS = *iterRhs;
            SymbolSet tmpFirstSet;
            firstOfVecSymbol(oneRHS, tmpFirstSet);
            // ��ÿ���ս���� a��FIRST(�Ҳ�)��A->�� ���� M[A,a]
            for (SymbolSet::iterator a = tmpFirstSet.begin(); a != tmpFirstSet.end(); a++) {
                if ((*a) != "~")
                    LL1AnaTable[*iterLeft][*a] = oneRHS;
                else {
                    for (SymbolSet::iterator b = follow[*iterLeft].begin(); b != follow[*iterLeft].end(); b++)
                        LL1AnaTable[*iterLeft][*b] = oneRHS;
                }
            }
        }
    }
    // �������޶���� M[A,a] ���ϴ����ʶ
    SymbolSet rowSet = T;
    rowSet.erase("~");
    rowSet.insert("$");
    for (SymbolSet::iterator line = N.begin(); line != N.end(); line++) {
        for (SymbolSet::iterator row = rowSet.begin(); row != rowSet.end(); row++) {
            if (LL1AnaTable[*line].find(*row) == LL1AnaTable[*line].end())
                LL1AnaTable[*line].insert(std::pair<Symbol, RHS>(*row, { "ERR" }));
        }
    }
}

/// @brief ����ֱ����ݹ�
/// @return 1-������ݹ��������ɹ�
///         0-��������ݹ�
int Grammar::removDirectRecur() {
    return 0;
}

/// @brief ��ȡ������
/// @return 1-�������Ӳ������޸ĳɹ�
///         0-������������
int Grammar::extractFactor() {
    return 0;
}

/// @brief ��д�ķ�
void Grammar::rewriteGrammar() {
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "����ֱ����ݹ�..." << std::endl;
    if (removDirectRecur())
        std::cout << "�����ɹ�" << std::endl;
    else
        std::cout << "��ǰ�ķ�����ݹ�" << std::endl;
    std::cout << "��ȡ������..." << std::endl;
    if (extractFactor())
        std::cout << "��ȡ�ɹ�" << std::endl;
    else
        std::cout << "��ǰ�ķ���������" << std::endl;
    std::cout << "��ǰ�ķ���" << std::endl;
    output();
}

/// @brief ��� first ���Ϻ� follow ����
void Grammar::outputFirstFollow() {
    std::cout << std::endl << setfill('-') << setw(Grammar::SPLIT_LINE_WIDTH) << "" << std::endl;
    std::cout << "��� FIRST FOLLOW��" << std::endl;
    // �����ͷ
    std::cout << "symbol\t" << setfill(' ') << setw(FIRST_O_WIDTH) << setiosflags(ios::left) << "first\t" << "follow" << std::endl;
    for (SymbolSet::iterator iterN = N.begin(); iterN != N.end(); iterN++) {
        std::string oLine = "";
        std::cout << *iterN << "\t";
        for (SymbolSet::iterator iterF = first[*iterN].begin(); iterF != first[*iterN].end(); iterF++)
            oLine = oLine + *iterF + " ";
        std::cout << setw(FIRST_O_WIDTH) << setiosflags(ios::left) << oLine;
        oLine = "";
        for (SymbolSet::iterator iterF = follow[*iterN].begin(); iterF != follow[*iterN].end(); iterF++)
            oLine = oLine + *iterF + " ";
        std::cout << setw(FIRST_O_WIDTH) << setiosflags(ios::left) << oLine << std::endl;
    }
}

/// @brief ��� LL(1) ������
void Grammar::outputAnaTable() {
    std::cout << std::endl << setfill('-') << setw(Grammar::SPLIT_LINE_WIDTH) << "" << std::endl;
    std::cout << "��� LL(1) ������" << std::endl;

    // �����ͷ
    SymbolSet rowSet = T;
    rowSet.erase("~");
    rowSet.insert("$");
    std::cout << setfill(' ') << setw(TABLE_O_WIDTH) << setiosflags(ios::left) << "N";
    for (SymbolSet::iterator iterS = rowSet.begin(); iterS != rowSet.end(); iterS++)
        std::cout << setw(TABLE_O_WIDTH) << setiosflags(ios::left) << *iterS;
    std::cout << std::endl;

    // �������������
    for (SymbolSet::iterator iterN = N.begin(); iterN != N.end(); iterN++) {
        std::cout << setw(TABLE_O_WIDTH) << setiosflags(ios::left) << *iterN;
        std::string oLine = "";
        for (SymbolSet::iterator iterT = rowSet.begin(); iterT != rowSet.end(); iterT++) {
            oLine = "";
            RHS curRHS = LL1AnaTable[*iterN][*iterT];
            if (curRHS != ERR) {
                oLine = oLine + *iterN + " -> ";
                for (RHS::iterator iterRHS = curRHS.begin(); iterRHS != curRHS.end(); iterRHS++)
                    oLine = oLine + *iterRHS + " ";
            }
            std::cout << setw(TABLE_O_WIDTH) << setiosflags(ios::left) << oLine;
        }
        std::cout << std::endl;
    }
}
