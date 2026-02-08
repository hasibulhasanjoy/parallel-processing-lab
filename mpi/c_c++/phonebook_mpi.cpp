
#include<bits/stdc++.h>
#include<mpi.h>
using namespace std;

struct Contact {
  string name;
  string phone;
};

void send_string(const string& text, int reciver) {
  int len = text.size() + 1;
  MPI_Send(&len, 1, MPI_INT, reciver, 1, MPI_COMM_WORLD);
  MPI_Send(text.c_str(), len, MPI_CHAR, reciver, 1, MPI_COMM_WORLD);
}

string recive_string(int sender) {
  int len;
  MPI_Recv(&len, 1, MPI_INT, sender, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  char* buff = new char[len];
  MPI_Recv(buff, len, MPI_CHAR, sender, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  string s(buff);
  delete[] buff;
  return s;
}

string vector_to_string(vector<Contact>& c,int start, int end) {
  string result;
  int n = c.size();
  for(int i = start; i < min(n, end); i++) {
    result += c[i].name + "," + c[i].phone + "\n";
  }
  return result;
}

vector<Contact> string_to_contact(string& text) {
  vector<Contact> contacts;
  istringstream iss(text);
  string line;
  while(getline(iss,line)) {
    int comma = line.find(",");
    string name = line.substr(0, comma);
    string phone = line.substr(comma+1);
    contacts.push_back({name,phone});
  }
  return contacts;
}

string check(Contact &c, string &key) {
  if(c.name.find(key) != string::npos) {
    return c.name + " " + c.phone + "\n";
  }

  return "";
}

void read(vector<Contact>& c, vector<string>& files) {
  for(auto &file: files) {
    ifstream f(file);
    string line;
    while(getline(f,line)) {
      int comma = line.find(",");
      string name = line.substr(1,comma-2);
      string phone = line.substr(comma+2,line.size() - comma - 3);
      c.push_back({name, phone});
    }
  }
}

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  string key = argv[argc - 1];
  double start, end;

  if(rank == 0) {
    vector<string> files(argv + 1, argv + argc - 1);
    vector<Contact> contacts;
    read(contacts, files);

    int n = contacts.size();
    int chunk = (n + size - 1) / size;

    for(int i = 1;i < size; i++) {
      string text = vector_to_string(contacts, i * chunk, (i + 1) * chunk);
      send_string(text, i);
    }

    start = MPI_Wtime();
    string result;
    for(int i = 0; i< min(chunk, n); i++) {
      string match = check(contacts[i], key);
      if(!match.empty()) {
        result += match;
      }
    }
    end = MPI_Wtime();

    for(int i = 1; i < size; i++) {
      string rcv = recive_string(i);
      if(!rcv.empty()) {
        result += rcv;
      }
    }

    ofstream out("output.txt");
    out<< result;
    out.close();
    printf("process %d took %f second\n", rank, end - start);
  } else {
    string rcv = recive_string(0);
    vector<Contact> contacts = string_to_contact(rcv);

    start = MPI_Wtime();

    string result;
    for(auto &c: contacts) {
      string match = check(c, key);
      if(!match.empty()) {
        result += match;
      }
    }
    end = MPI_Wtime();
    send_string(result, 0);
    printf("process %d took %f second\n", rank, end - start);
  }

  MPI_Finalize();
  return 0;
}
