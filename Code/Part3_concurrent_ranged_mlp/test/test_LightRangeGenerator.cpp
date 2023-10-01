//
// Created by eaad on 9/26/23.
//
#include "LightRangeGenerator/LightRangeGenerator.h"

int main() {
    LightRandomGenerator LRG(LARGE_RANGES, LARGE_ONLY);
    LRG.SetOverlapPolicy(RETRY_N_TIMES_THEN_NULLPTR);
    std::vector<pair*>* v = LRG.DumpToVector(1000);
    pair *P;
    for (int i=0; i<v->size(); i++) {
        //pair *P = LRG.Next();
        //printf("0x%lx, 0x%lx\n", P->first, P->second);
        P = (*v)[i];
        printf("0x%lx, 0x%lx\n", P->first, P->second);
    }
    printf("size: %lu\n",v->size());
    return 0;
}