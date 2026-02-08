#include<bits/stdc++.h>
using namespace std;

struct Contact {
  char name[65];
  char phone[65];
};

string getInput(ifstream& file) {
  string s;
  char c;
  int readFlag = 0;
  while(file.get(c)) {
    if(c == '\"') {
      if(readFlag == 1) {
        break;
      }
      readFlag = 1;
    } else {
      if(readFlag == 1) {
        s.push_back(c);
      }
    }
  }

  return s;
}

__device__ bool find(char *s1, char *s2)
{
  for (int i = 0; s1[i] != '\0'; i++)
  {
    int flag = 1;
    for (int j = 0; s2[j] != '\0'; j++)
    {
      if (s1[i + j] != s2[j])
      {
        flag = 0;
        break;
      }
    }

    if (flag == 1)
    {
      return true;
    }
  }

  return false;
}

__global__ void myKernel(Contact* phonebook,char* key,int offset) {
  int threadNumber = threadIdx.x + offset;

  if(find(phonebook[threadNumber].name,key)) {
    printf("name: %s phone: %s\n",phonebook[threadNumber].name,phonebook[threadNumber].phone);
  }
}

int main(int argc, char* argv[])
{
  if(argc < 3) {
    cout<<"key and thread limit is not provided"<<endl;
    return 0;
  }
  int threadLimit = atoi(argv[2]);
  ifstream myfile("/content/drive/MyDrive/pp lab/labtest_dataset1.txt");
  vector<Contact> phonebook;

  for(int i = 0;i<2000;i++) {
    string name = getInput(myfile);
    string phone = getInput(myfile);
    Contact c;
    strcpy(c.name,name.c_str());
    strcpy(c.phone,phone.c_str());
    phonebook.push_back(c);
  }

  string key_str = argv[1];
  char key[65];
  strcpy(key,key_str.c_str());

  // gpu memory allocation
  char* d_key;
  cudaMalloc(&d_key,65);
  cudaMemcpy(d_key,key,65,cudaMemcpyHostToDevice);

  int n = phonebook.size();
  Contact* d_phonebook;
  cudaMalloc(&d_phonebook,n * sizeof(Contact));
  cudaMemcpy(d_phonebook,phonebook.data(),n * sizeof(Contact),cudaMemcpyHostToDevice);

  // find matching
  int remaining = n;
  int offset = 0;
  while(remaining > 0) {
    int batchSize = min(threadLimit,remaining);
    myKernel<<<1,batchSize>>>(d_phonebook,d_key,offset);
    cudaDeviceSynchronize();
    remaining -= batchSize;
    offset += batchSize;
  }
  return 0;
}
