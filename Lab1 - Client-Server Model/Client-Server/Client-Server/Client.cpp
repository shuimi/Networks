#include<iostream>
#include<fstream>
#include<string>
#include<windows.h>
using namespace std;

const int name_length = 15;
char current_client[name_length];

struct connection {
    char login[name_length];
    char command;
};

struct data_request {
    char Name[name_length];
    int Current_Term;
    int Good_Marks;
    int Bad_Marks;
};

struct data_response {
    int FPP;
};

ifstream file_read;
ofstream file_write;

int file_size(string file_path) {
    ifstream check_file(file_path, ios::binary | ios::ate);
    int size = check_file.tellg();
    check_file.close();
    return size;
}

void init(connection* init, char login[name_length], char command) {
    for (int i = 0; i < name_length; i++) init->login[i] = ' ';
    for (int i = 0; i < name_length; i++) init->login[i] = login[i];
    init->command = command;
}

bool compare(char login_from_struct[name_length], char auth_login[name_length]) {
    for (int i = 0; i < name_length; i++) {
        if (login_from_struct[i] == ' ') return true;
        if (login_from_struct[i] != auth_login[i]) return false;
    }
}

bool auth_request(char login[name_length]) {
    connection* request = new connection;
    init(request, login, 'S');

    int last_size = file_size("connections.bin");
    file_write.open("connections.bin", ios::binary | ios::app);
    file_write.write((char*)request, sizeof(connection));
    file_write.close();

    while (true) {
        if (last_size < file_size("connections.bin")) {
            connection* response = new connection;
            file_read.open("connections.bin", ios::binary | ios::beg);
            file_read.seekg(last_size, ios::beg);
            file_read.read((char*)response, sizeof(connection));
            file_read.close();

            if (compare(response->login, login)) {
                if (response->command == 'W') return true;
                if (response->command == 'B') return false;
            }
            else {
                cout << "|client_log| bad server response: incorrect login\n";
                return false;
            }
            break;
        }
    }
}

bool exit_request(char login[name_length]) {
    connection* request = new connection;
    init(request, login, 'E');

    int last_size = file_size("connections.bin");
    file_write.open("connections.bin", ios::binary | ios::app);
    file_write.write((char*)request, sizeof(connection));
    file_write.close();

    while (true) {
        if (last_size < file_size("connections.bin")) {
            connection* response = new connection;
            file_read.open("connections.bin", ios::binary);
            file_read.seekg(last_size, ios::beg);
            file_read.read((char*)response, sizeof(connection));
            file_read.close();

            if (compare(response->login, login)) {
                if (response->command == 'D') return true;
                if (response->command == 'B') return false;
            }
            else {
                cout << "|client_log| bad server response: incorrect login\n";
                return false;
            }
            break;
        }
    }
}

void init_user_file(char login[name_length]) {
    char username[name_length + 4];
    int current_name_length = 0;
    while (login[current_name_length] != '\0') current_name_length++;
    for (int i = 0; i < name_length + 4; i++) username[i] = '\0';
    for (int i = 0; i < current_name_length; i++) username[i] = login[i];
    username[current_name_length] = '.';
    username[current_name_length + 1] = 'b';
    username[current_name_length + 2] = 'i';
    username[current_name_length + 3] = 'n';
    ofstream user_file(username, ios::binary | ios::app); user_file.close();
}

void get_client_name() {
    cout << "Enter username:\n";
    for (int i = 0; i < name_length; i++) current_client[i] = '\0';
    cin >> current_client;
}

void Send_Request(int CT, int GM, int BM, char Name[name_length], char file_path[name_length + 4]) {
    data_request* New_Request = (struct data_request*)malloc(sizeof(data_request));
    for (int i = 0; i < name_length; i++) {
        New_Request->Name[i] = Name[i];
    }
    New_Request->Current_Term = CT;
    New_Request->Good_Marks = GM;
    New_Request->Bad_Marks = BM;
    ofstream File(file_path, ios::binary | ios::app);
    File.write((char*)New_Request, sizeof(data_request));
    File.close();
}

void Process_Response(char file_path[name_length + 4]) {
    data_response* New_Response = new data_response;
    ifstream File(file_path, ios::binary);
    int current_position = file_size(file_path) - sizeof(data_response);
    File.seekg(current_position, ios::beg);
    File.read((char*)New_Response, sizeof(data_response));
    File.close();
    cout << "Freebie Power is " << New_Response->FPP << "%\n\n";
}

char* add_bin(char login[name_length]) {
    char* username = new char[name_length + 4];
    int current_name_length = 0;
    while (login[current_name_length] != '\0') current_name_length++;
    for (int i = 0; i < name_length + 4; i++) username[i] = '\0';
    for (int i = 0; i < current_name_length; i++) username[i] = login[i];
    username[current_name_length] = '.';
    username[current_name_length + 1] = 'b';
    username[current_name_length + 2] = 'i';
    username[current_name_length + 3] = 'n';
    return username;
}

void main() {

    while (true) {

        get_client_name();
        if (auth_request(current_client)) {
            init_user_file(current_client);
            cout << "|client_log| server response: welcome\n";
            while (true) {
                string command;
                cout << "\nCommands list: \nRequest - to create and send request to server\nEXIT - to disconnect from server\n";
                cout << "\nEnter command: \n";
                cin >> command; cout << "\n";

                if (command == "Request" || command == "R" || command == "r" || command == "request") {

                    int Current_Term, Good_Marks, Bad_Marks, Saved_Size, Amount;
                    char Name[name_length];

                    cout << "Amount of requests: "; cin >> Amount;
                    for (int i = 0; i < Amount; i++) {
                        cout << "---\nRequest #" << i + 1 << ": \n";
                        cout << "Name: "; cin >> Name;
                        cout << "Term: "; cin >> Current_Term;
                        cout << "Good Marks: "; cin >> Good_Marks;
                        cout << "Bad Marks: "; cin >> Bad_Marks;
                        Saved_Size = file_size(add_bin(current_client));
                        Send_Request(Current_Term, Good_Marks, Bad_Marks, Name, add_bin(current_client));

                        while (true) {
                            cout << "---\nSaved_Size: " << Saved_Size;
                            cout << "\nfile_size: " << file_size(add_bin(current_client)) << "\n---\n";
                            if (Saved_Size < file_size(add_bin(current_client))) {
                                Process_Response(add_bin(current_client));
                                break;
                            }
                        }
                    }
                }
                else if (command == "EXIT" || command == "E" || command == "e" || command == "exit") {
                    if (exit_request(current_client)) {
                        cout << "|client_log| server response: disconnected\n";
                        break;
                    }
                }
            }
            break;
        }
        else {
            cout << "Incorrect username, try again...\n";
        }
    }
    Sleep(700);
}