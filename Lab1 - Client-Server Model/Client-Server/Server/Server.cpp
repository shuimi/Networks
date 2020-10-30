#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<stack>
#include<windows.h>
using namespace std;

const int name_length = 15;
const int max_connections = 20;
int currently_connected = 0;

vector<int> user_file_size;
char users[max_connections][name_length];
int current_position;

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

bool compare(char login_from_struct[name_length], char auth_login[name_length]) {
    for (int i = 0; i < name_length; i++) {
        if (login_from_struct[i] == ' ') return true;
        if (login_from_struct[i] != auth_login[i]) return false;
    }
}

bool find_client_connection(char login[name_length]) {
    for (int i = 0; i < currently_connected; i++) {
        if (compare(users[i], login)) {
            return true;
        }
    }
    return false;
}

void remove_client(char login[name_length]) {
    for (int i = 0; i < currently_connected; i++) {
        if (compare(users[i], login)) {
            if (currently_connected - 1 == i) {
                for (int j = 0; j < name_length; j++) {
                    users[currently_connected - 1][j] = '\0';
                }
                user_file_size.pop_back();
            }
            else {
                for (int j = 0; j < name_length; j++) {
                    users[i][j] = users[currently_connected - 1][j];
                }
                for (int j = 0; j < name_length; j++) {
                    users[currently_connected - 1][j] = '\0';
                }
                user_file_size.erase(user_file_size.begin() + i);
            }
            currently_connected--;
        }
    }
}

void add_client(char login[name_length]) {
    for (int i = 0; i < name_length; i++) {
        users[currently_connected][i] = login[i];
    }
    currently_connected++;
    user_file_size.push_back(0);
}

void init(connection* init, char login[name_length], char command) {
    for (int i = 0; i < name_length; i++) init->login[i] = '\0';
    for (int i = 0; i < name_length; i++) init->login[i] = login[i];
    init->command = command;
}

void connections_init() {
    ofstream init_file("connections.bin", ios::binary | ios::trunc);
    init_file.close();
    current_position = file_size("connections.bin");
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

void process_connection(connection* current_request) {
    cout << "|server_log| new request: imported from pool\n";
    if (find_client_connection(current_request->login)) {
        if (current_request->command == 'S') {
            connection* response = new connection;
            init(response, current_request->login, 'B');
            file_write.open("connections.bin", ios::binary | ios::app);
            file_write.write((char*)response, sizeof(connection));
            file_write.close();
            cout << "|server_log| user already exists: response 'BAD'\n";
        }
        else if (current_request->command == 'E') {
            connection* response = new connection;
            init(response, current_request->login, 'D');
            file_write.open("connections.bin", ios::binary | ios::app);
            file_write.write((char*)response, sizeof(connection));
            file_write.close();
            cout << "|server_log| client request disconnect: user removed\n";
            remove_client(current_request->login);
            cout << "|server_log| removing file " << add_bin(current_request->login) << "\n";
            remove(add_bin(current_request->login));
        }
    }
    else {
        if (current_request->command == 'S') {
            connection* response = new connection;
            init(response, current_request->login, 'W');
            file_write.open("connections.bin", ios::binary | ios::app);
            file_write.write((char*)response, sizeof(connection));
            file_write.close();
            cout << "|server_log| new user: authentified\n";
            add_client(current_request->login);
        }
        else if (current_request->command == 'E') {
            connection* response = new connection;
            init(response, current_request->login, 'B');
            file_write.open("connections.bin", ios::binary | ios::app);
            file_write.write((char*)&response, sizeof(connection));
            file_write.close();
            cout << "|server_log| bad auth request: already disconnected of unidentified\n";
        }
    }
}

void get_auth_request_pool() {
    while (current_position < file_size("connections.bin")) {
        connection* request = new connection;
        file_read.open("connections.bin", ios::binary);
        file_read.seekg(current_position);
        file_read.read((char*)request, sizeof(connection));
        file_read.close();
        if (request->command == 'S' || request->command == 'E') {
            cout << "login: " << request->login << "\ncommand: " << request->command << "\n";
            cout << "current_position = " << current_position << "\ncurrent_size = " << file_size("connections.bin") << "\n";
            process_connection(request);
        }
        current_position += sizeof(connection);
    }
}

int Freebie_Power(data_request* Current_Request) {
    int Unpecented = Current_Request->Current_Term * (Current_Request->Good_Marks - Current_Request->Bad_Marks);
    if (Unpecented < 0) return 0;
    else if ((216 / 100) * Unpecented > 100) return 100;
    else return (216 / 100) * Unpecented;
}

void get_request_pool(char file_path[name_length + 4], int user_index) {

    stack<data_response*> response_list;

    while (user_file_size[user_index] < file_size(file_path)) {
        data_request* new_request = new data_request;
        file_read.open(file_path, ios::binary);
        file_read.seekg(user_file_size[user_index]);
        file_read.read((char*)new_request, sizeof(data_request));
        file_read.close();
        user_file_size[user_index] += sizeof(data_request);
        data_response* new_response = new data_response;
        new_response->FPP = Freebie_Power(new_request);
        response_list.push(new_response);

        cout << new_request->Current_Term << "\t" << new_request->Good_Marks << "\t" << new_request->Bad_Marks << "\t" << response_list.top()->FPP << "\n";
    }

    while (!response_list.empty()) {
        file_write.open(file_path, ios::binary | ios::app);
        file_write.write((char*)response_list.top(), sizeof(data_response));
        file_write.close();
        user_file_size[user_index] += sizeof(data_response);
        response_list.pop();
    }
}

void get_users_pool() {
    if (!user_file_size.empty()) {
        for (int i = 0; i < currently_connected; i++) {
            get_request_pool(add_bin(users[i]), i);
        }
    }
}

void main() {
    connections_init();
    while (true) {
        get_auth_request_pool();
        get_users_pool();
        Sleep(100);
    }
}