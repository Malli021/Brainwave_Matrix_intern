#include <iostream>
#include <string>
#include <unordered_map>
#include <limits>
#include <ctime>
#include <sstream>
#include <fstream>
#include <iomanip>

using namespace std;

class Bank {
    string name, address;
    char acc_type;
    float balance;
    string password;
    string account_number;
    string transaction_history;

    string generate_account_number() {
        stringstream ss;
        ss << "AC" << time(0);
        return ss.str();
    }

    string get_current_timestamp() {
        time_t now = time(0);
        tm *ltm = localtime(&now);
        stringstream ss;
        ss << setfill('0') << setw(2) << ltm->tm_mday << "/"
           << setw(2) << ltm->tm_mon + 1 << "/"
           << setw(4) << 1900 + ltm->tm_year << " "
           << setw(2) << ltm->tm_hour << ":"
           << setw(2) << ltm->tm_min << ":"
           << setw(2) << ltm->tm_sec;
        return ss.str();
    }

public:
    Bank() : balance(0), acc_type('S') {}

    void open_account();
    void check_balance();
    void deposit_money();
    void withdraw_money();
    void display_account();
    void calculate_interest();
    void transfer_money();
    void change_password(const string &new_password);
    void change_address(const string &new_address);
    bool authenticate(const string &entered_password);
    string get_account_number() { return account_number; }
    string get_name() { return name; }
    float get_balance() { return balance; }
    void add_transaction_history(const string &transaction);
    void save_account(ofstream &out);
    void load_account(ifstream &in);
};

unordered_map<string, Bank> accounts;

void Bank::add_transaction_history(const string &transaction) {
    transaction_history += get_current_timestamp() + " - " + transaction + "\n";
}

// Save account data to a file
void Bank::save_account(ofstream &out) {
    out << account_number << "\n";
    out << name << "\n";
    out << address << "\n";
    out << acc_type << "\n";
    out << balance << "\n";
    out << password << "\n";
    out << transaction_history << "\n";
}

// Load account data from a file
void Bank::load_account(ifstream &in) {
    getline(in, account_number);
    getline(in, name);
    getline(in, address);
    in >> acc_type;
    in >> balance;
    in.ignore();
    getline(in, password);
    getline(in, transaction_history);
}

void Bank::open_account() {
    cout << "Enter your full name: ";
    getline(cin, name);

    cout << "Enter your address: ";
    getline(cin, address);

    cout << "What type of account you want to open? (S for Saving, C for Current): ";
    cin >> acc_type;
    cin.ignore();

    cout << "Enter the initial deposit amount (Minimum $500): ";
    cin >> balance;
    while (balance < 500) {
        cout << "Minimum deposit is $500. Please enter a valid amount: ";
        cin >> balance;
    }
    cin.ignore();

    cout << "Set a password for your account: ";
    getline(cin, password);

    account_number = generate_account_number();
    add_transaction_history("Account created with initial deposit of $" + to_string(balance));

    accounts[account_number] = *this;
    cout << "Account Created Successfully with Account Number: " << account_number << endl;
}

void Bank::check_balance() {
    cout << "Your balance is: $" << balance << endl;
}

void Bank::deposit_money() {
    float amount;
    cout << "Enter how much money you want to deposit: $";
    cin >> amount;

    while (amount <= 0) {
        cout << "Deposit amount must be positive. Try again: ";
        cin >> amount;
    }

    balance += amount;
    add_transaction_history("Deposited $" + to_string(amount));
    cout << "Available Balance after deposit: $" << balance << endl;
}

void Bank::withdraw_money() {
    float amount;
    cout << "Enter how much money you want to withdraw: $";
    cin >> amount;

    while (amount <= 0 || amount > balance) {
        if (amount <= 0) {
            cout << "Withdrawal amount must be positive. Try again: ";
        } else {
            cout << "Insufficient balance! Try again: ";
        }
        cin >> amount;
    }

    balance -= amount;
    add_transaction_history("Withdrew $" + to_string(amount));
    cout << "Available balance after withdrawal: $" << balance << endl;
}

void Bank::display_account() {
    cout << "\nAccount Details:\n";
    cout << "Account Number: " << account_number << endl;
    cout << "Name: " << name << endl;
    cout << "Address: " << address << endl;
    cout << "Account Type: " << (acc_type == 'S' ? "Saving" : "Current") << endl;
    cout << "Balance: $" << balance << endl;
    cout << "Transaction History:\n" << transaction_history << endl;
}

void Bank::calculate_interest() {
    if (acc_type == 'S') {
        float interest_rate = 0.03;
        float interest = balance * interest_rate;
        balance += interest;
        add_transaction_history("Interest of $" + to_string(interest) + " added to account.");
        cout << "Interest of $" << interest << " added. New Balance: $" << balance << endl;
    } else {
        cout << "Interest calculation is only applicable for savings accounts.\n";
    }
}

void Bank::transfer_money() {
    string receiver_account_number;
    float amount;

    cout << "Enter the receiver's account number: ";
    getline(cin, receiver_account_number);

    // Check if receiver account exists
    if (accounts.find(receiver_account_number) == accounts.end()) {
        cout << "Receiver account not found!\n";
        return;
    }

    cout << "Enter the amount to transfer: $";
    cin >> amount;
    cin.ignore();

    // Validate transfer amount
    if (amount <= 0) {
        cout << "Transfer amount must be positive.\n";
        return;
    }
    if (amount > balance) {
        cout << "Insufficient balance for this transaction.\n";
        return;
    }

    // Deduct amount from sender
    balance -= amount;
    add_transaction_history("Transferred $" + to_string(amount) + " to account " + receiver_account_number);

    // Add amount to receiver
    Bank &receiver = accounts[receiver_account_number];
    receiver.balance += amount;
    receiver.add_transaction_history("Received $" + to_string(amount) + " from account " + account_number);

    cout << "Successfully transferred $" << amount << " to account " << receiver_account_number << ".\n";
}

bool Bank::authenticate(const string &entered_password) {
    return password == entered_password;
}

void login() {
    string entered_account_number, entered_password;
    cout << "Enter your account number: ";
    getline(cin, entered_account_number);

    if (accounts.find(entered_account_number) != accounts.end()) {
        Bank &customer = accounts[entered_account_number];

        cout << "Enter your password: ";
        getline(cin, entered_password);

        if (customer.authenticate(entered_password)) {
            cout << "Login successful!\n";
            int choice;
            while (true) {
                cout << "\n1) Check Balance\n2) Deposit Money\n3) Withdraw Money\n4) Display Account\n";
                cout << "5) Calculate Interest\n6) Transfer Money\n7) Logout\n";
                cout << "Enter your choice: ";
                cin >> choice;
                cin.ignore();

                if (choice == 7) break;

                switch (choice) {
                    case 1:
                        customer.check_balance();
                        break;
                    case 2:
                        customer.deposit_money();
                        break;
                    case 3:
                        customer.withdraw_money();
                        break;
                    case 4:
                        customer.display_account();
                        break;
                    case 5:
                        customer.calculate_interest();
                        break;
                    case 6:
                        customer.transfer_money();
                        break;
                    default:
                        cout << "Invalid choice! Try again.\n";
                }
            }
        } else {
            cout << "Incorrect password!\n";
        }
    } else {
        cout << "Account not found!\n";
    }
}

void save_all_accounts() {
    ofstream out("accounts.txt");
    for (auto &pair : accounts) {
        pair.second.save_account(out);
    }
    out.close();
}

void load_all_accounts() {
    ifstream in("accounts.txt");
    while (in) {
        Bank customer;
        customer.load_account(in);
        if (!customer.get_account_number().empty()) {
            accounts[customer.get_account_number()] = customer;
        }
    }
    in.close();
}

int main() {
    load_all_accounts();
    int choice;

    while (true) {
        cout << "\n1) Open Account\n2) Login\n3) Exit\nEnter your choice: ";
        cin >> choice;
        cin.ignore();

        if (choice == 3) {
            save_all_accounts();
            cout << "Exiting the system.\n";
            break;
        }

        switch (choice) {
            case 1: {
                Bank new_account;
                new_account.open_account();
                save_all_accounts();
                break;
            }
            case 2:
                login();
                break;
            default:
                cout << "Invalid choice! Try again.\n";
        }
    }

    return 0;
}
