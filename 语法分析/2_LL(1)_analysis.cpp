#include "Grammar.h"
#include <iostream>


/// @brief LL(1) ��������
void Grammar::LL1Analysis() {
    // rewriteGrammar();
    buildFirst();
    buildFollow();
    buildLL1AnaTable();




    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "������Ҫ�������ַ�����";
    inputS();

}

/// @brief ���� first ����
void Grammar::buildFirst() {
    // �������еķ��ս��
    for (SymbolSet::iterator iter = N.begin(); iter != N.end(); iter++) {
        vectorRHS rhss = P[(*iter)];
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
        vectorRHS rhss = P[a];
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
        vectorRHS rhss = P[(*iter)];
        // ����һ�����ս���������Ҳ�����ʽ
        for (vectorRHS::iterator iterJ = rhss.begin(); iterJ != rhss.end(); iterJ++) {
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
        vectorRHS rhss = P[(*iterLeft)];
        // ����һ�����ս���������Ҳ�����ʽ
        for (vectorRHS::iterator iterRhs = rhss.begin(); iterRhs != rhss.end(); iterRhs++) {
            RHS oneRHS = *iterRhs;
            SymbolSet tmpFirstSet;
            firstOfVecSymbol(oneRHS, tmpFirstSet);
            // ��ÿ���ս���� a��FIRST(�Ҳ�)��A->�� ���� M[A,a]
            for (SymbolSet::iterator a = tmpFirstSet.begin(); a != tmpFirstSet.end(); a++) {
                if ((*a) != "~")
                    anaTable[*iterLeft][*a] = oneRHS;
                else {
                    for (SymbolSet::iterator b = follow[*iterLeft].begin(); b != follow[*iterLeft].end(); b++)
                        anaTable[*iterLeft][*b] = oneRHS;
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
            if (anaTable[*line].find(*row) == anaTable[*line].end())
                anaTable[*line].insert(std::pair<Symbol, RHS>(*row, { "ERR" }));
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
