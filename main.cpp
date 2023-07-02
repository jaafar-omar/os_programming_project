//
// Created by Jaafar on 29/06/2023.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <bits/stdc++.h>
#define INPUT_FILE "C:\\Users\\Asus\\CLionProjects\\bankers_algo\\input_file.txt"
using namespace std;
int ncustomers = 5, nresources = 4;
vector<int> seq;

typedef struct {
    int id = 0;
    vector<int> Max;			// 2D array Max[i,1] = k, means pi need max k instances of resource 1
    vector<int> Allocation;	    // 2D array Allocation[i,1] = k, means pi has k instances of resource 1
    vector<int> Need;			// 2D array Need[i,1] = k, means pi need k instances of resource 1 to start exec
    bool status = false;
} customer;

typedef struct {
    string doit;
    int id;
    int res[4];
} command;


bool IsSafe(vector<customer>  customers, vector<int> available) {
    int finished = ncustomers;
    int j;
    while(finished != 0){
        int counter = finished;
        for(auto & customer : customers) {
            if(!customer.status){
                for( j = 0; j<nresources; j++)
                    if (customer.Need[j] > available[j]) break;
                if(j == nresources) {
                    finished --;
                    customer.status = true;
                    for(int k = 0; k < nresources; k++)
                        available[k] += customer.Allocation[k];
                    seq.push_back(customer.id);

                }
            }
        }
        if(counter == finished){
            return false;
        }

    }
    return true;
}

int request_resources(vector<customer>  customers, vector<int> available, int p , const int req[]){
    int j;
    for( j = 0; j<nresources; j++)
        if (req[j] > available[j] || req[j] > customers[p].Need[j]) break;
    if(j!=nresources) return -1;

    for( j = 0; j<nresources; j++){
        customers[p].Allocation[j] += req[j];
        customers[p].Need[j] -= req[j];
        available[j] -= req[j];
    }
//    // Print Need Matrix
//    cout<<"\n\tNeed Matrix";
//    for(int j = -1; j < ncustomers; j++){
//        cout<<endl;
//        if(j==-1){
//            cout<<"  \t";
//            for(int i = 0; i < nresources; i++){
//                cout<<"R"<<i<<"\t";
//            }
//            continue;
//        }
//        cout<<"C"<<j<<"\t";
//        for(int i = 0; i < nresources; i++)
//            cout<<customers[j].Need[i]<<"\t";
//    }
//    cout<<endl<<endl;

     if(IsSafe(customers, available)) {
         return 0;
     } else { return -1; }
}

void release_resources(vector<customer>& customers,vector<int>& available, int customer_num, const int release[]) {
    for (int i = 0; i < nresources; ++i) {
        customers[customer_num].Allocation[i] -= release[i];
        available[i] += release[i];
    }
}

bool check_release(vector<customer> customers, int customer_id, const int rel[]) {
    for (int i = 0; i < nresources; ++i) {
        if(rel[i] > customers[customer_id].Allocation[i]) {
            return false;
        }
    }
    return true;
}

int main(){
    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);
    vector<int> available = {10, 5, 7, 8};				// num of instances for each resource
    vector<command> commands(1);
    vector<customer> customers(ncustomers);
    vector<int> req(nresources,0);
    int request[nresources];
    int release[nresources];
    char sysexit;
    ifstream readFile;
    string readLine;
    readFile.open(INPUT_FILE);

    for(int i = 0; i < ncustomers; i++)
    {
        customers[i].Max.resize(nresources);
        customers[i].Allocation.resize(nresources);
        customers[i].Need.resize(nresources);
        customers[i].id = i;
        customers[i].status= false;
    }

    if(readFile.is_open()) {
        int row_index = 0;
        while(getline(readFile, readLine)) {
            stringstream ss(readLine);
            int value;
            int column_index = 0;
            while( ss >> value ) {
                customers[row_index].Max[column_index] = value;
                customers[row_index].Need[column_index] = customers[row_index].Max[column_index] - customers[row_index].Allocation[column_index];
                column_index++;
                if(ss.peek() == ',')
                    ss.ignore();
            }
            row_index++;
        }
    }

    do {
        command req_command;
        string input;
        cout << "Enter Commands: ";
        getline(cin, input);

        istringstream iss(input);
        iss >> req_command.doit;
        iss >> req_command.id;
        for (int & re : req_command.res) {
            iss >> re;
        }

        commands.pop_back();
        commands.push_back(req_command);

        if(commands[0].doit == "RQ") {
            for (int i = 0; i < nresources; ++i) {
                request[i] = commands[0].res[i];
            }

            if(request_resources(customers, available, commands[0].id, request) == 0) {
                for (int i = 0; i < nresources; ++i) {
                    customers[commands[0].id].Allocation[i] += request[i];
                    customers[commands[0].id].Need[i] -= request[i];
                    available[i] -= request[i];
                }
                cout << "Request granted" << endl;
            } else {
                cout << "This request will leave the system in an unsafe state; request denied!" << endl;
            }

        } else if(commands[0].doit == "RL") {
            for (int i = 0; i < nresources; ++i) {
                release[i] = commands[0].res[i];
            }

            if(check_release(customers, commands[0].id, release)) {
                release_resources(customers, available, commands[0].id, release);
                cout << "Resources released successfully!" << endl;
                for (int i = 0; i < nresources; ++i) {
                    cout << "R" << i << "\t";
                }
                cout << endl;
                for (int i = 0; i < nresources; ++i) {
                    cout << release[i] << "\t";
                }
                cout << endl;
            } else {
                cout << "Requested release resources is greater than Allocated resources at C" << commands[0].id << "! It should be less than or equal." << endl;
                cout << "Requested Release:" << "\t\t" << ">" << "\t" << "Allocated Resources: " << endl;
                for (int i = 0; i < nresources; ++i) {
                    cout << "R" << i << "\t";
                }
                cout << "\t";
                for (int i = 0; i < nresources; ++i) {
                    cout << "R" << i << "\t";
                }
                cout << endl;

                for (int i = 0; i < nresources; ++i) {
                    cout << release[i] << "\t";
                }
                cout << "\t";
                for (int i = 0; i < nresources; ++i) {
                    cout << customers[commands[0].id].Allocation[i] << "\t";
                }
                cout << endl;
            }

        } else if(commands[0].doit == "*") {
            // Print Available resources
            cout << "Available Resources: " << endl;
            for (int i = 0; i < nresources; ++i) {
                cout << "R" << i << "\t";
            }
            cout << endl;
            for (int i = 0; i < nresources; ++i) {
                cout << available[i] << "\t";
            }
            cout << endl;

            // Print  Max Matrix
            cout<<"\n\tMax Matrix";
            for(int j = -1; j < ncustomers; j++){
                cout<<endl;
                if(j==-1){
                    cout<<"  \t";
                    for(int i = 0; i < nresources; i++){
                        cout<<"R"<<i<<"\t";
                    }
                    continue;
                }
                cout<<"C"<<j<<"\t";
                for(int i = 0; i < nresources; i++)
                    cout<<customers[j].Max[i]<<"\t";
            }
            cout<<endl;

            // Print Allocation Matrix
            cout<<"\n\tAllocation Matrix";
            for(int j = -1; j < ncustomers; j++){
                cout<<endl;
                if(j==-1){
                    cout<<"  \t";
                    for(int i = 0; i < nresources; i++){
                        cout<<"R"<<i<<"\t";
                    }
                    continue;
                }
                cout<<"C"<<j<<"\t";
                for(int i = 0; i < nresources; i++)
                    cout<<customers[j].Allocation[i]<<"\t";
            }
            cout<<endl;


            // Print Need Matrix
            cout<<"\n\tNeed Matrix";
            for(int j = -1; j < ncustomers; j++){
                cout<<endl;
                if(j==-1){
                    cout<<"  \t";
                    for(int i = 0; i < nresources; i++){
                        cout<<"R"<<i<<"\t";
                    }
                    continue;
                }
                cout<<"C"<<j<<"\t";
                for(int i = 0; i < nresources; i++)
                    cout<<customers[j].Need[i]<<"\t";
            }
            cout<<endl;
        }

        cout<<"\n\nDo you want make command again?(y/n)\n";
        cin>>sysexit;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    } while(sysexit=='y'||sysexit=='Y');

    return 0;
}

