// complie
// g++ -std=c++11 algo1.cpp -o algo1.out
//
#include<iostream>
#include<fstream>
#include<cmath>
#include<vector>
#include<chrono>

using namespace std;

vector<vector<vector<int>>> algo1(int);

int main(){
    int nSample = 100;
    ofstream outf("algo1.csv");
    for (int K = 3; K <= 10; K++){
        for (int i = 0; i < nSample; i++){
            auto t1 = chrono::high_resolution_clock::now();
            algo1(K);
            auto t2 = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>( t2 - t1 ).count();
            if (i != 0){
                outf << ",";
            }
            outf << duration;
        }
        outf << endl;
    }
    outf.close();
}

vector<vector<vector<int>>> algo1(int K){
    int cur_p;
    vector<vector<vector<int>>> S(K+1, vector<vector<int>>(pow(2, K-1), vector<int>(K, -1)));
    for (int k = 1; k <= K; k++){
        cur_p = 0;
        for (int i = 1; i < k; i++){
            for (int p = 0; p < pow(2, k-i-1); p++){ 
                S[k][cur_p][0] = i;
                for (int q = 0; q < K-1; q++){
                    if (S[k-i][p][q] == -1) break; 
                    S[k][cur_p][q+1] = S[k-i][p][q];
                }
                cur_p++;
            }
        }
        // for i == k
        S[k][cur_p][0] = k;
    }
    
    // print enum result
    
    for (int k = 1; k < K+1; k++){
        cout << "k = " << k << endl;
        for (int p = 0; p < pow(2, k-1); p++){
            cout << "[";
            for (int q = 0; q < K; q++){
                if (S[k][p][q] == -1) break;
                if (q != 0) cout << ", ";
                cout << S[k][p][q];
            }
            cout << "]";
        }
        cout << endl;
    }
    
    return S;
}