#include <iostream>
#include <thread>
#include <vector>

void minion(int i){
  std::cout<<"Hello! I am minion "<<i<<std::endl;
}

void overloard(){
  std::cout<<"Hello! I am Overloard!"<<std::endl;
}

int main (int argc, char** argv) {
  if (argc < 2) {
    std::cerr<<"usage: "<<argv[0]<<" <nbminions>\n";
    return -1;
  }
  int numMinions = atoi(argv[1]);
  std::vector<std::thread> threads;
  for(unsigned int i=0;i<numMinions;i++){
    std::thread thrd(minion,i);
    threads.push_back(std::move(thrd));
  }

  for(auto & t : threads){
    if(t.joinable()){
      t.join();
    }else{
       std::cout<<"t is not joinable"<<std::endl;
    }
  }
  std::thread thrd(overloard);
  if(thrd.joinable()){
    thrd.join();
  }else{
    std::cout<<"thrd is not joinable"<<std::endl;
  }
  
  return 0;
}
