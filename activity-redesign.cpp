/*
1) Reduce
1.1)
*/
template<typename T, typename op>
T reduce (T* array, size_t n){
    float frac = n/P;
    int num_in_each_process = std::ceil(frac);
    T results[P];
    for(int i=0;i<P;i++){
        int start_index = i * snum_in_each_process;
        results[i] = array[start_index];
        for(int j=1;j<num_in_each_process;j++){
            if(start_index + j < n){
                results[i] = op(results[i], array[start_index + j]);
            }
        }
    }
    T result = results[0];
    for(int i=1;i<P;i++){
        result = op(result,results[i]);
    }
}
/*
width: P
CP:    3 + n/P + P
work:  3 + n + P
1.2)
int, max:       yes, because each block would have a local maximum, then the global would be found from those values
string, concat: yes, because each block is still working in sequential order within it's contained process. When 'conquering', they are also still in order
float, sum:     yes, because of the communitive proproty of addition: (A+B)+C == A+(B+C)
float, max:     yes, because each block would have a local maximum, then the global would be found from those values

2)Prefix Sum
*/
void prefixsum (int* arr , int n, int* pr) {
    float frac = n/P;
    int num_in_each_process = std::ceil(frac);
    for(int i=0;i<P;i++){
        int start_index = i * num_in_each_process;
        pr[start_index] = 0;
        for(int j=0;j<num_in_each_process;j++){
            if(start_index + j + 1 < n){
                pr[start_index + j + 1] = pr[start_index + j] + arr[start_index + j];
            }
        }
    }
    for(int i=1;i<P;i++){
        int start_index = i * num_in_each_process;
        int diff = pr[start_index - 1];
        for(int j=0;j<num_in_each_process;j++){
            if(start_index + j + 1 < n){
                pr[start_index + j] += diff;
            }
        }
    }
}
/*
width: P
CP:    2 + n/P + Pn
work:  3 + 2n

3)Merge Sort
*/
T[] merge(T* a, T* b){
    float frac = n/P;
    int num_in_each_process = std::ceil(frac);
    T merged[P];
    for(int i=0;i<P;i++){
        int start_index = i * num_in_each_process;
        T[] a1 = a[start_index..start_index + num_in_each_process];
        T[] b1 = b[start_index..start_index + num_in_each_process];
        merged[i] = doMerge(a1,b1);
    }
    for(int i=0;i<P/2;i++){
        T retHold[std::ceil(merged/2)];
        for(int j=0;j<retHold.size();j++){
            T m1 = merged.at(j);
            T m2 = merged.at(merged.size()-j-1);
            if(m1 != m2){
                retHold[j] = doMerge(m1,m2);
            }
        }
        merged = retHold;
    }
    return merged;
}

T[] doMerge(T* a, T* b){
    int i = 0;
    int j = 0;
    int k = 0;
    T ret[2n];
    while(i < a.size() && j <b.size()){
        if(b[j] < a[i]){
            ret[k] = b[j];
            j++;
        }else{
            ret[k] = a[i];
            i++;
        }
        k++;
    }
    for(i;i<a.size();i++){
        ret[k] = a[i];
        k++;
    }
    for(j;j<b.size();j++){
        ret[k] = b[j];
        k++;
    }
}
/*
work: nPlog(nP)
CP:   nlog(nP)
*/