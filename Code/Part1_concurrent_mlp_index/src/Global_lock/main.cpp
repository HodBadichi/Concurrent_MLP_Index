//
// Created by eaad on 8/1/23.
//

#include <MlpSetUInt64.h>
#include <common.h>
#include <thread>

void worker(int idx, MlpSetUInt64::MlpSet* ms) {
    int arr[10];
    int c = 0;
    for (int i=0; i<10; i++)
    {
        bool inserted = ms->Insert(i);
        if (inserted)
            arr[c++] = inserted;

        printf("t%d, val:%d, status:%d\n", idx, i, inserted);
    }

}

int main() {
    const int N = 1000;
    cout << "Welcome to concurrent mlp_ds\n";
    MlpSetUInt64::MlpSet ms;
    ms.Init(N + 1000);

    std::thread t1(worker, 1, &ms);
    std::thread t2(worker, 2, &ms);


    t1.join();
    t2.join();
    return 0;
}