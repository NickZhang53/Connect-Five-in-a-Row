#include "pattern_search.h"
#include "Board.h"
#include <queue>
using namespace std;

const Pattern patterns[17] = { {"",0},
                               {"11111", 500000},
                               {"011110", 6000}, /*4320*/
                               {"011100", 720}, /*720*/
                               {"001110", 720},
                               {"011010", 720},
                               {"010110", 720},
                               {"11110", 720},
                               {"01111", 720},
                               {"11011", 720},
                               {"10111", 720},
                               {"11101", 720},
                               {"001100", 120},
                               {"001010", 120},
                               {"010100", 120},
                               {"000100", 20},
                               {"001000", 20} };

//AC_Search AC;

AC_Search::AC_Search() {
    memset(trie, 0, sizeof(trie));
    memset(fail, 0, sizeof(fail));
    memset(word, 0, sizeof(word));
    tot = 0;
    for (int i = 1; i <= 16; ++i)
        insert(patterns[i].patternString, i);
    getFail();
}

void AC_Search::insert(const char *s, int idx) {
    int now = 0;
    for (int i = 0; s[i]; ++i){
        int ch = s[i] - '0';
        if (!trie[now][ch]) trie[now][ch] = ++tot;
        now = trie[now][ch];
    }
    word[now] = idx;
}

void AC_Search::getFail() {
    queue<int> q;
    for (int i = 0; i <= 1; ++i) {
        if (trie[0][i])
            q.push(trie[0][i]);
        fail[trie[0][i]] = 0;
    }
    while (!q.empty()) {
        int u = q.front(); 
        q.pop();
        for (int i = 0; i <= 1; ++i) {
            if (trie[u][i]) {
                fail[trie[u][i]] = trie[fail[u]][i];
                q.push(trie[u][i]);
            } else trie[u][i] = trie[fail[u]][i];
        }
    }
}

int AC_Search::query(const char *s){
    int totScore = 0;
    int now = 0;
    for (int i = 0; s[i]; ++i){
        now = trie[now][s[i] - '0'];
        for (int t = now; t; t = fail[t])
            totScore += patterns[word[t]].score;
    }
    return totScore;
}
