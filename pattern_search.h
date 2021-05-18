#pragma once

using namespace std;

struct Pattern{
    char patternString[10];
    int score;
};

extern const Pattern patterns[17];

class AC_Search{
private:
    int trie[50][3], fail[50], word[50], tot;
    void insert(const char *s, int idx);
    void getFail();
public:
    AC_Search();
    int query(const char *s);
};
