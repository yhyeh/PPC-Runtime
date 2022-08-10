#include<iostream>
#include<fstream>
#include<cmath>
#include<vector>
#include<string>
#include<sstream>
#include<stdio.h>
#include<chrono>
#include<algorithm>
#include<unordered_set>
#include<random>

using namespace std;

/* class */
class tNode {
public:
    tNode* parent;
    vector<tNode*> childs;
    vector<int> funcSet;
    // unordered_set<int> ancestorSet;
    tNode(){
        parent = NULL;
    }
    string funcStr(){
        ostringstream ss;
        for (int fidx: funcSet){
            ss << ",f" << fidx;
        }
        ss << "}"; // << this;
        string result = ss.str();
        result[0] = '{';
        return result;
    };
};

class PC {
public:
    vector<vector<int>> C;
    vector<int> CSize;
    int K, k;
    vector<vector<vector<tNode*>>> trees;
    static unsigned seed;
    static default_random_engine randEng;

    PC(int K){
        this->K = K;
        C.resize(K, vector<int>(K, -1));
        generateRandPC();
        // generatePC();
    };
    
    

    string toStr(){
        string result = "{";
        for (vector<int> c: C){
            ostringstream ss;
            string cluster;
            if (c[0] == -1) break;

            for (int fidx: c){
                if (fidx == -1) break;
                ss << ",f" << fidx;
            }
            ss << "}";
            cluster = ss.str();
            cluster[0] = '{';
            result.append(cluster);
        }
        result.append("}");
        return result;
    }
private:
    void generateRandPC(){
        int funcToAlloc = K;
        int clusterSize;
        while(funcToAlloc > 0){
            vector<float> distrVal = getDistrVal(funcToAlloc);
            discrete_distribution<int> distribution(distrVal.begin(), distrVal.end());
            clusterSize = distribution(randEng)+1;
            CSize.push_back(clusterSize);
            funcToAlloc -= clusterSize;
        }
        k = CSize.size();
        int iFunc = 1;
        for (int c = 0; c < k; c++){
            for (int f = 0; f < CSize[c]; f++){
                C[c][f] = iFunc;
                iFunc++;
            }
        }
        // cout << "PC" << toStr() << endl;
    }
    void generatePC(){
        CSize.push_back(2);
        CSize.push_back(2);
        C[0][0] = 1;
        C[0][1] = 2;
        C[1][0] = 3;
        C[1][1] = 4;
        k = CSize.size();
        // cout << "PC" << toStr() << endl;
    }
    vector<float> getDistrVal(int len){
        vector<float> distr;
        for (int i = 1; i <= len; i++){
            distr.push_back(1.0/sqrt(i));
        }
        return distr;
    }
};

/* for combination */
class Combination{
public:
    vector<vector<vector<vector<int>>>> book;
    /* dim detail */
    // 0: which cluster
    // 1: from clusterSize choose 1~clusterSize
    // 2: which combination
    // 3: element in combination

    Combination(PC pc){
        book.resize(pc.CSize.size());
        for (int c = 0; c < pc.CSize.size(); c++){
            
            book[c].resize(pc.CSize[c]);
            
            for(int i = 0; i < pc.CSize[c]; i++){
                int n = pc.CSize[c];
                int r = i+1;
                int idx = 0;
                book[c][i].resize(factorial(n)/factorial(n-r)/factorial(r), vector<int>(r, -1));

                vector<int> myints(r);
                vector<int>::iterator first = myints.begin(), last = myints.end();

                generate(first, last, UniqueNumber);

                for (int j = 0; j < myints.size(); j++){
                    book[c][i][idx][j] = myints[j]-1;
                }
                idx++;
                /*
                std::for_each(first, last, myfunction);
                std::cout << std::endl;
                */

                while((*first) != n-r+1){
                    vector<int>::iterator mt = last;

                    while (*(--mt) == n-(last-mt)+1);
                    (*mt)++;
                    while (++mt != last) *mt = *(mt-1)+1;

                    for (int j = 0; j < myints.size(); j++){
                    book[c][i][idx][j] = myints[j]-1;
                    }
                    idx++;
                    /*
                    std::for_each(first, last, myfunction);
                    std::cout << std::endl;
                    */
                }
            }
        }
    }
private:
    struct c_unique {
        int current;
        c_unique() {current=0;}
        int operator()() {return ++current;}
    } UniqueNumber;
    /*
    void myfunction (int i) {
        std::cout << i-1 << ' ';
    }
    */
    int factorial(int n){
        return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
    }
};


/* function */
vector<vector<vector<int>>> algo1(int);
void algo2(PC&, int, vector<vector<vector<int>>>& , Combination&);
void generatePC(int, PC&);
void printPath(int, vector<vector<vector<tNode*>>>&);
void printPathRecur(tNode*, vector<string>&, int, int, unordered_set<int>, int);

//unsigned PC::seed = 0;
unsigned PC::seed = chrono::system_clock::now().time_since_epoch().count();
default_random_engine PC::randEng = default_random_engine(seed);

int main(){
    int nSample = 20;
    int nDup = 10;
    vector<vector<vector<int>>> S;
    char valfname[30]; 
    char pcfname[30]; 
    bool test = false; 
    
    if (test){
        int K = 5;
        S = algo1(K);
        for (int i = 0; i < nSample; i++){
            cout << "\nSample" << i+1 << "=====================" << endl;
            auto t1 = chrono::high_resolution_clock::now();
            PC randPC(K);
            Combination combination(randPC);
            algo2(randPC, K, S, combination);
            //printPath(K, randPC.trees);
            auto t2 = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>( t2 - t1 ).count();
            cout << "duration: " << duration << endl;
        }
    }
    else{
        for (int K = 3; K <= 10; K++){
            cout << "K=" << K << endl;
            S = algo1(K);
            sprintf(valfname, "algo2_output/valK%d.csv", K);
            sprintf(pcfname, "algo2_output/pcK%d.csv", K);
            ofstream valfile(valfname);
            ofstream pcfile(pcfname);
            for (int i = 0; i < nSample; i++){
                PC randPC(K);
                pcfile << randPC.toStr() << endl;
                for (int d = 0; d < nDup; d++){
                    auto t1 = chrono::high_resolution_clock::now();
                    Combination combination(randPC);
                    algo2(randPC, K, S, combination);
                    // printPath(K, randPC.trees);
                    auto t2 = chrono::high_resolution_clock::now();
                    auto duration = chrono::duration_cast<chrono::microseconds>( t2 - t1 ).count();
                    if (d != 0){
                        valfile << ",";
                    }
                    valfile << duration;
                }
                valfile << endl;
            }
            S.clear();
            valfile.close();
            pcfile.close();
        }
    }
}

void algo2(PC& pc, int K, vector<vector<vector<int>>>& S, Combination& comb){
    int level;
    int sum;
    int delta;
    bool stop_;

    for (int s = 0; s < pow(2, K-1); s++){ // iterate SK, length[tight] 2^(K-1)
        vector<vector<tNode*>> Ts(K);
        stop_ = false;
        level = 0;
        for (int j = 0; j < pc.k; j++){ // iterate cluster, length[max] K
            sum = 0;
            delta = 0;
            for (int i = level; i < S[K][s].size(); i++){ // iterate s, length[max] K
                if (S[K][s][i] == -1) {
                    break;
                }
                sum += S[K][s][i];
                if (sum == pc.CSize[j]){
                    // build this cluster
                    delta = i-level+1;
                    break;
                }else if (sum > pc.CSize[j]){
                    // invalid s
                    stop_ = true;
                    break;
                }
            }

            if (stop_){
                break;
            }else { // build tree
                int x;
                for (x = level; x < level+delta; x++){ // iterate level, length[max] K
                    int r = S[K][s][x]; 
                    // choose r from pc.C[j] as node
                    for (vector<int>combOpt: comb.book[j][r-1]){ // iterate combination, length[max] K!/(K/2)!/(K/2)!
                        tNode* tmpNode = new tNode();
                        for (int funcidx: combOpt){
                            tmpNode->funcSet.push_back(pc.C[j][funcidx]);
                        }
                        // tmpNode.ancestorSet.insert(tmpNode.funcSet.begin(), tmpNode.funcSet.end());
                        Ts[x].push_back(tmpNode);
                    }
                    // connect to upper level for all non-root level
                    if (x > 0){ // not root
                        for (int y = 0; y < Ts[x-1].size(); y++){
                            //size_t nAncFunc = upper.ancestorSet.size();
                            
                            for (int z = 0; z < Ts[x].size(); z++){
                                Ts[x-1][y]->childs.push_back(Ts[x][z]);
                                
                                /*
                                size_t nLowerFunc = lower.funcSet.size();
                                unordered_set<int> uniq(upper.funcSet.begin(), upper.funcSet.end());
                                if (lower.ancestorSet.size() == nAncFunc + lower.funcSet.size()){
                                    // multually exclusive
                                    lower.ancestorSet.insert(upper.ancestorSet.begin(), lower.ancestorSet.end());
                                    upper.childs.push_back(&lower);
                                }
                                */
                            }
                        }                   
                    }
                }
                level = x; 
            }
        }
        if (stop_) continue;
        else{
            pc.trees.push_back(Ts);
        }
    }
}

void printPath(int K, vector<vector<vector<tNode*>>>& trees){
    for (vector<vector<tNode*>>& tree: trees){
        for (tNode* root: tree[0]){
            vector<string> path(K);
            unordered_set<int> funcs;
            printPathRecur(root, path, 0, 0, funcs, K);
        }
    }
}
void printPathRecur(tNode* node, vector<string>& path, int pathLen, int funcLen, unordered_set<int> funcs, int K){
    path[pathLen] = node->funcStr();
    pathLen++;
    funcLen += node->funcSet.size();
    funcs.insert(node->funcSet.begin(), node->funcSet.end());
    if (funcs.size() < funcLen) return;

    if (node->childs.size() == 0){
        // print multual exclusive path        
        cout << path[0];
        for (int i = 1; i < pathLen; i++){
            cout << "->" << path[i];
        }
        cout << endl;
        
    }else {
        for (tNode* child: node->childs){
            printPathRecur(child, path, pathLen, funcLen, funcs, K);
        }
    }
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
    /*
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
    */
    return S;
}
