/*
Name: Nguyễn Huỳnh Minh Tuyết
ID:24110144
Project: Bank Account Management System
*/

#include <iostream>
#include <vector>
using namespace std;

class Transaction {
protected:
    double amount;
    string type; // "deposit" or "withdraw"
    string date;

public:
    Transaction(double amt, string t, string d) {
        amount = amt;
        type = t;
        date = d;
    }

    void setAmount(double amt) { amount = amt; }
    void setType(string t) { type = t; }
    void setDate(string d) { date = d; }
    double getAmount() const { return amount; }
    string getType() const { return type; }
    string getDate() const { return date; }

    // In transaction
    friend ostream& operator<<(ostream& os, const Transaction& tr) {
        os << "[" << tr.date << "] " << tr.type << ": " << tr.amount;
        return os;
    }
};

class Account {
protected:
    string accountNumber;
    double balance;
    string accountName;
    vector<Transaction> transactions;

public:
    Account(string accNum, string accName, double initialBalance) {
        accountNumber = accNum;
        accountName = accName;
        balance = initialBalance;
        transactions.push_back(Transaction(initialBalance, "create", "today"));
    }

    void setAccountNumber(string accNum) { accountNumber = accNum; }
    void setAccountName(string accName) { accountName = accName; }
    void setBalance(double bal) { balance = bal; }
    string getAccountNumber() const { return accountNumber; }
    string getAccountName() const { return accountName; }
    vector<Transaction> getTransactions() const { return transactions; }
    double getBalance() const { return balance; }

    // operator += : thêm transaction
    Account& operator+=(const Transaction& t) {
        if (t.getType() == "deposit") {
            deposit(t.getAmount());
        } else if (t.getType() == "withdraw") {
            withdraw(t.getAmount());
        }
        transactions.push_back(t);
        return *this;
    }

    // operator == : so sánh balance
    bool operator==(const Account& other) const {
        return this->balance == other.balance;
        // hoặc: return this->accountNumber == other.accountNumber;
    }

    // nạp tiền
    virtual void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
        } else {
            cout << "Deposit amount must be positive!" << endl;
        }
    }

    // rút tiền
    virtual void withdraw(double amount) {
        if (amount > 0 && amount <= balance) {
            balance -= amount;
        } else {
            cout << "Insufficient funds or invalid amount!" << endl;
        }
    }
};

class Customers {
protected:
    string name;
    string ID;
    vector<Account*> accounts;

public:
    Customers(string n, string id) {
        name = n;
        ID = id;
    }
    void setName(string n) { name = n; }
    void setID(string id) { ID = id; }
    string getName() const { return name; }
    string getID() const { return ID; }

    // mở tài khoản mới
    void addAccount(Account* acc) {
        accounts.push_back(acc);
    }

    // view totals across accounts
    double getTotalBalance() const {
        double total = 0;
        for (const auto& acc : accounts) {
            total += acc->getBalance();
        }
        return total;
    }
};

class SavingsAccount : public Account {
private:
    double interestRate;
    double withdrawalLimit = 5000; // Giới hạn rút tiền mỗi lần
    double withdrawalFee = 10;     // Phí rút tiền

public:
    SavingsAccount(string accNum, string accName, double initialBalance, double rate)
        : Account(accNum, accName, initialBalance), interestRate(rate) {}

    void setInterestRate(double rate) { interestRate = rate; }
    double getInterestRate() const { return interestRate; }
    void setWithdrawalLimit(double limit) { withdrawalLimit = limit; }
    double getWithdrawalLimit() const { return withdrawalLimit; }
    void setWithdrawalFee(double fee) { withdrawalFee = fee; }
    double getWithdrawalFee() const { return withdrawalFee; }

    // Áp dụng lãi suất
    void applyInterest() {
        double interest = balance * interestRate / 100;
        deposit(interest);
        transactions.push_back(Transaction(interest, "interest", "today"));
    }

    // Override phương thức rút tiền
    void withdraw(double amount) override {
        if (amount > withdrawalLimit) {
            cout << "Withdrawal exceeds limit for savings account!" << endl;
            return;
        }
        if (amount + withdrawalFee > balance) {
            cout << "Insufficient funds for withdrawal and fee!" << endl;
            return;
        }
        balance -= (amount + withdrawalFee);
        transactions.push_back(Transaction(amount, "withdraw(with fee)", "today"));
    }
};


class Operations{
    protected:
    vector<Account> accounts;
    vector<Customers> customers;
    vector<Transaction> transactions;

    public:
    void addAccount(const Account& acc){
        accounts.push_back(acc);
    }

    void addCustomer(const Customers& cust){
        customers.push_back(cust);
    }

    void addTransaction(const Transaction& tr){
        transactions.push_back(tr);
    }

    // thuc hien giao dich
    void performTransaction(const string& accNum, const Transaction& tr){
        for(Account& acc : accounts){
            if(acc.getAccountNumber() == accNum){
                acc += tr; // sử dụng operator +=
                break;
            }
        }
    }

    // ap dung lai suat cho tat ca tai khoan tiet kiem
    void applyInterestToSavings(){
        for(Account& acc : accounts){
            SavingsAccount* savAcc = dynamic_cast<SavingsAccount*>(&acc);
            if(savAcc){
                savAcc->applyInterest();
            }
        }
    }

    void displayAccountInfo(const string& accNum){
        for(const Account& acc : accounts){
            if(acc.getAccountNumber() == accNum){
                cout<<"Account Number: "<<acc.getAccountNumber()<<endl;
                cout<<"Account Name: "<<acc.getAccountName()<<endl;
                cout<<"Balance: "<<acc.getBalance()<<endl;
                cout<<"Transactions:"<<endl;
                for(const auto& tr : acc.getTransactions()){
                    cout<<tr<<endl;
                }
                break;
            }
        }
    }
    
};


int main() {
    Operations ops;

    // Tạo khách hàng
    Customers cust1("Nguyen Van A", "C001");
    Customers cust2("Le Thi B", "C002");
    ops.addCustomer(cust1);
    ops.addCustomer(cust2);

    // Tạo tài khoản
    SavingsAccount savAcc1("A001", "Nguyen Van A - Savings", 10000, 5.0);
    Account acc1("A002", "Nguyen Van A - Checking", 5000);
    ops.addAccount(savAcc1);
    ops.addAccount(acc1);
    cust1.addAccount(&savAcc1);
    cust1.addAccount(&acc1);

    // Thực hiện giao dịch
    ops.performTransaction("A001", Transaction(2000, "deposit", "2024-10-01"));
    ops.performTransaction("A002", Transaction(1000, "withdraw", "2024-10-02"));
    ops.performTransaction("A001", Transaction(3000, "withdraw", "2024-10-03")); // Rút tiền trong giới hạn

    // Áp dụng lãi suất cho tài khoản tiết kiệm
    ops.applyInterestToSavings();

    // Hiển thị thông tin tài khoản
    ops.displayAccountInfo("A001");
    ops.displayAccountInfo("A002");

    return 0;
   
}
