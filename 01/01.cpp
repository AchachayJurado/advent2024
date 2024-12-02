#include <iostream>
#include <vector>
#include <map>

using namespace std;
int N=1000;

int main(){
    vector<int> a, b;
    for (int i=0; i<N; i++){
        int x, y;
        cin >> x >> y;
        a.push_back(x);
        b.push_back(y);
    }
    sort(a.begin(), a.end()); 
    sort(b.begin(), b.end()); 

    map<int, int> freq;
    for (int x:b){
        freq[x]++;
    }
    long long sum = 0;
    for (int x:a){
    // for (int i=0; i < N; i++){
        // sum += abs (a[i]-b[i]) Part 1
        sum += (long long) x*freq[x];
    }
    cout << sum << "\n";
}
