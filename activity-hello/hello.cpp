#include <iostream>
#include <unistd.h>

int main () {
  int returnCode;
  char name[50];
  returnCode = gethostname(name,sizeof(name));
  if(returnCode == 0){
    std::cout<<"hostName: "<<name<<std::endl;
  }
  return 0;
}
