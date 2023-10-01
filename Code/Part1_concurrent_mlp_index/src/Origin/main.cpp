//
// Created by eaad on 8/1/23.
//

#include <MlpSetUInt64.h>
#include <common.h>

int main() {
    const int N = 1000;
    vector< vector<int> > choices;
    choices.resize(8);
    rep(i,0,7)
    {
        int sz = (i <= 1) ? 2 : 5;
        rep(j,0,sz-1)
        {
            int x = rand() % 256;
            choices[i].push_back(x);
        }
    }
    MlpSetUInt64::MlpSet ms;
    ms.Init(N + 1000);
    rep(steps,0,N-1)
    {
        uint64_t value = 0;
        rep(i,0,7)
        {
            value = value * 256 + choices[i][rand() % choices[i].size()];
        }
        bool ms_inserted = ms.Insert(value);
        if (steps % (N / 10) == 0)
        {
            printf("%d%% completed\n", steps / (N/10)*10);
        }
    }
}