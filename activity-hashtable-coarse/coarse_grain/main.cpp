#include <chrono>
#include <fstream>
#include <sstream>
#include <vector>
#include <mutex>
#include <thread>
#include <map>

#include "Dictionary.cpp"
#include "MyHashtable.cpp"

//Tokenize a string into individual word, removing punctuation at the
//end of words
std::vector<std::vector<std::string>> tokenizeLyrics(const std::vector<std::string> files) {
  std::vector<std::vector<std::string>> ret;

  for(auto filename : files) {
    //std::cout<<"reading "<<filename<<"\n";
    std::vector<std::string> my_vect;
    std::ifstream in (filename);

    std::string line;

    //For each line
    while (getline(in, line, '\n')) {
      //Skip all like starting with [
      if (line[0] == '[')
        continue;

      std::stringstream ssline (line);
      //For all words
      while (ssline) {
        std::string word;
        ssline >> word;
        if (ssline) {
          //remove punctuation at the end of word
          while (word.length() > 0
                 && std::ispunct(word[word.length() - 1])) {
            word.pop_back();
          }
          my_vect.push_back(word);
        }
      }
    }
    //std::cout<<"read "<<my_vect.size()<<" words\n";
    ret.push_back(my_vect);
  }
  return ret;
}

void readFile(std::vector<std::string>& content, Dictionary<std::string, int>& dict, std::map<std::string, std::mutex>& mutMap){
  std::cout<<"I'm inside the readFile function"<<std::endl;
  for(auto& w:content){
    std::lock_guard<std::mutex> lg(mutMap[w]);
    int count = dict.get(w);
    count++;
    dict.set(w,count);
  }
}

int main(int argc, char **argv)
{
  if (argc < 4) {
    std::cerr<<"usage: ./main <sources> <testword> <threshold>"<<std::endl;
    return -1;
  }

  // Parse Command Line
  std::string source = argv[1];
  std::string testWord = argv[2];
  int32_t thresholdCount = std::stoi(argv[3]);

  // Obtain List of Files
  std::vector<std::string> files;
  std::ifstream in (source);
  std::string line;
  while (getline(in, line, '\n')) {
    files.push_back(line);
  }

  // Tokenize Lyrics
  auto wordmap = tokenizeLyrics(files);

  MyHashtable<std::string, int> ht;
  Dictionary<std::string, int>& dict = ht;

  std::map<std::string, std::mutex> mutMap;
  std::vector<std::thread> threads;
  for(auto & fileContent:wordmap){
    std::thread thrd(readFile, std::ref(fileContent),std::ref(dict),std::ref(mutMap));
    threads.push_back(std::move(thrd));
  }

  for(auto & t:threads){
    if(t.joinable()){
      t.join();
    }else{
      std::cout<<"Something went wrong"<<std::endl;
    }
  }

  /*
  // Check Hash Table Values 
  //(you can uncomment, but this must be commented out for tests)
  for (auto it : dict) {
    if (it.second > thresholdCount)
      std::cout << it.first << " " << it.second << std::endl;
  }
  */

  // Do not touch this, need for test cases
  std::cout << ht.get(testWord) << std::endl;

  return 0;
}
