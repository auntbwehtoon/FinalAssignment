#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ctype.h"

//global variables
int email_valid=-1;
int nrc_valid=-1;
int chk_amt=-1;
int pass_strength=-1;
double maxTransferAmount;

// Structure for storing user information
typedef struct {
    int id;
    char email[50];
    char name[50];
    char password[20];
    char NRC[20];
    double amount;
    double loan;
    int transition_records;
    char phone_number[20];
    int cash_in_records;
    int cash_out_records;
    char address[100];
    int account_status;
} User;

// Structure for AVL tree node
typedef struct AVLNode {
    User user;
    struct AVLNode* left;
    struct AVLNode* right;
    int height;
} AVLNode;

// Function to get the maximum of two integers
int max(int a, int b) {
    return (a > b) ? a : b;
}

// Function to get the height of a node in the AVL tree
int getHeight(AVLNode* node) {
    if (node == NULL)
        return 0;
    return node->height;
}

// Function to create a new AVL tree node
AVLNode* createNode(User user) {
    AVLNode* newNode = (AVLNode*)malloc(sizeof(AVLNode));
    newNode->user = user;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->height = 1;
    return newNode;
}

// Function to right rotate a subtree rooted with y
AVLNode* rightRotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

    return x;
}

// Function to left rotate a subtree rooted with x
AVLNode* leftRotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

    return y;
}

// Function to get the balance factor of a node
int getBalance(AVLNode* node) {
    if (node == NULL)
        return 0;
    return getHeight(node->left) - getHeight(node->right);
}

// Function to insert a user into the AVL tree
AVLNode* insertUser(AVLNode* node, User user) {
    if (node == NULL)
        return createNode(user);

    if (user.id < node->user.id)
        node->left = insertUser(node->left, user);
    else if (user.id > node->user.id)
        node->right = insertUser(node->right, user);
    else
        return node;

    node->height = 1 + max(getHeight(node->left), getHeight(node->right));

    int balance = getBalance(node);

    if (balance > 1 && user.id < node->left->user.id)
        return rightRotate(node);

    if (balance < -1 && user.id > node->right->user.id)
        return leftRotate(node);

    if (balance > 1 && user.id > node->left->user.id) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    if (balance < -1 && user.id < node->right->user.id) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

// Function to check password strength
int checkPasswordStrength(const char* password) {
    int length = strlen(password);
    int hasUpperCase = 0;
    int hasLowerCase = 0;
    int hasDigit = 0;
    int hasSpecialChar = 0;

    // Check if the password meets the minimum length requirement
    if (length < 8) {
        return 0;  // Weak password
    }

    // Check each character of the password for different criteria
    for (int i = 0; i < length; i++) {
        if (isupper(password[i])) {
            hasUpperCase = 1;
        } else if (islower(password[i])) {
            hasLowerCase = 1;
        } else if (isdigit(password[i])) {
            hasDigit = 1;
        } else {
            hasSpecialChar = 1;
        }
    }

    // Check if the password meets the minimum requirements for strength
    if (hasUpperCase && hasLowerCase && hasDigit && hasSpecialChar) {
        return pass_strength=1;  // Strong password
    } else if ((hasUpperCase || hasLowerCase) && hasDigit) {
        return pass_strength=0;  // Medium password
    } else {
        return pass_strength=-1;  // Weak password
    }
}


// Function to save the user information to a file
void saveUserToFile(User user) {
    FILE* file = fopen("user_data.txt", "a");
    fprintf(file, "%d,%s,%s,%s,%s,%.2lf,%s,%s,%d,%.2lf,%d,%d,%d\n",
            user.id, user.email, user.name, user.password, user.NRC, user.amount,
            user.phone_number, user.address, user.account_status, user.loan, user.transition_records,
            user.cash_in_records, user.cash_out_records);
    fclose(file);
}

//Function to output user information from file when login
void displayUserInfo(int userId) {
    FILE* file = fopen("user_data.txt", "r");
    if (file) {
        char line[1000];
        while (fgets(line, sizeof(line), file)) {
            User user;
            sscanf(line, "%d,%[^,],%[^,],%[^,],%[^,],%lf,%[^,],%[^,],%d",
                   &user.id, user.email, user.name, user.password, user.NRC, &user.amount,
                   user.phone_number,
                   user.address, &user.account_status);
            if (user.id == userId) {
                printf("\n===== User Information =====\n");
                printf("User ID: %d\n", user.id);
                printf("Email: %s\n", user.email);
                printf("Name: %s\n", user.name);
                printf("NRC: %s\n", user.NRC);
                printf("Amount: %lf\n", user.amount);
                printf("Phone Number: %s\n", user.phone_number);
                printf("Address: %s\n", user.address);


                if (user.account_status==0){
                    printf("Account Status: %s\n", "Personal");
                }
                else{
                    printf("Account Status: %s\n", "Business");
                }

                printf("=============================\n");
                break;
            }
        }
        fclose(file);
    }
}

//Update some userinfo after login
void updateUserInfo(int userId, int acc_lvl) {
    FILE* file = fopen("user_data.txt", "r+");
    if (file) {
        char line[1000];
        long int currentPos = ftell(file);
        while (fgets(line, sizeof(line), file)) {
            User user;
            sscanf(line, "%d,%[^,],%[^,],%[^,],%[^,],%lf,%[^,],%[^,],%d,%lf,%d,%d,%d",
                   &user.id, user.email, user.name, user.password, user.NRC, &user.amount,
                   user.phone_number, user.address, &user.account_status, &user.loan, &user.transition_records,
                   &user.cash_in_records, &user.cash_out_records);
            if (user.id == userId) {
                // Ask the user if they want to take a loan
                char choice;
                printf("\nDo you want to take a loan? (y/n): ");
                scanf(" %c", &choice);
                if (choice == 'y' || choice == 'Y') {
                    printf("Do you have a job? (y/n): ");
                    scanf(" %c", &choice);
                    if (choice == 'y' || choice == 'Y') {
                        double salary;
                        printf("How much salary per month? :");
                        scanf("%lf", &salary);
                        if (salary >= 200) {
                            printf("Enter Loan: ");
                            scanf("%lf", &user.loan);
                        } else {
                            printf("\nSalary must be at least $200 per month. Loan application rejected!\n");
                        }
                    } else {
                        printf("Loan application rejected!\n");
                    }
                }

                // Ask the user if they want to transfer money
                printf("\nDo you want to transfer money? (y/n): ");
                scanf(" %c", &choice);
                if (choice == 'y' || choice == 'Y') {
                    double transferAmount;
                    printf("Enter the amount to transfer: ");
                    scanf("%lf", &transferAmount);

                    // Check account status for transfer limit

                    if (acc_lvl == 0) { // Personal account
                        maxTransferAmount = 300;
                    } else{ // Business account
                        maxTransferAmount = 1000;
                    }

                    if (transferAmount <= maxTransferAmount) {
                        if (transferAmount <= user.amount) {
                            user.amount -= transferAmount;
                            user.transition_records +=1;
                        } else {
                            printf("Insufficient balance. Transfer canceled!\n");
                        }
                    } else {
                        printf("Transfer amount exceeds the maximum limit. Transfer canceled!\n");
                    }
                }

                // Ask the user if they want to cash in
                printf("\nDo you want to cash in? (y/n): ");
                scanf(" %c", &choice);
                if (choice == 'y' || choice == 'Y') {
                    double cashInAmount;
                    printf("Enter the amount to cash in: ");
                    scanf("%lf", &cashInAmount);
                    user.amount += cashInAmount;
                    user.cash_in_records += 1;
                }

                // Ask the user if they want to cash out
                printf("\nDo you want to cash out? (y/n): ");
                scanf(" %c", &choice);
                if (choice == 'y' || choice == 'Y') {
                    double cashOutAmount;
                    printf("Enter the amount to cash out: ");
                    scanf("%lf", &cashOutAmount);
                    if (cashOutAmount <= user.amount) {
                        user.amount -= cashOutAmount;
                        user.cash_out_records += 1;
                    } else {
                        printf("Insufficient balance. Cash out canceled!\n");
                    }
                }

                // Move the file pointer to the beginning of the line
                fseek(file, currentPos, SEEK_SET);

                // Write the updated user information back to the file
                fprintf(file, "%d,%s,%s,%s,%s,%.2lf,%s,%s,%d,%.2lf,%d,%d,%d\n",
                        user.id, user.email, user.name, user.password, user.NRC, user.amount,
                        user.phone_number, user.address, user.account_status, user.loan, user.transition_records,
                        user.cash_in_records, user.cash_out_records);
                printf("\nUser information updated successfully!\n");
                break;
            }
            currentPos = ftell(file);
        }
        fclose(file);
    }
}


// Function to validate email format
int emailValidation(const char* email) {
    int atSymbolCount = 0;
    int dotSymbolCount = 0;
    int atIndex = -1;
    int dotIndex = -1;

    // Check if the email contains at least one '@' symbol and one '.'
    for (int i = 0; email[i] != '\0'; i++) {
        if (email[i] == '@') {
            atSymbolCount++;
            atIndex = i;
        } else if (email[i] == '.') {
            dotSymbolCount++;
            dotIndex = i;
        }
    }

    if (atSymbolCount != 1 || dotSymbolCount < 1 || atIndex > dotIndex) {
        return 0;  // Invalid email format
    }

    // Check if the email starts or ends with '@' or '.'
    if (email[0] == '@' || email[0] == '.' || email[atIndex - 1] == '@' || email[dotIndex + 1] == '.') {
        return 0;  // Invalid email format
    }

    return email_valid=1;  // Valid email format
}

// Function to validate NRC format
int isValidNRC(const char* NRC) {
    // Check if the NRC length is correct
    if (strlen(NRC) != 15) {
        return 0;  // Invalid NRC format
    }

    // Check if the first two characters are alphabets
    if (!isalpha(NRC[0]) || !isalpha(NRC[1])) {
        return 0;  // Invalid NRC format
    }

    // Check if the third character is '/'
    if (NRC[2] != '/') {
        return 0;  // Invalid NRC format
    }

    // Check if the remaining characters are digits
    for (int i = 3; i < 15; i++) {
        if (!isdigit(NRC[i])) {
            return 0;  // Invalid NRC format
        }
    }

    return nrc_valid=1;  // Valid NRC format
}

//Function to valid deposited amount
int checkAmount(const double amt){
    if(amt==0|| amt<0 || amt<200){
        return 0;
    }

    return chk_amt=1;
}



int main() {
    AVLNode* root = NULL;
    int nextId = 1;

    // Load existing user data from file
    FILE* file = fopen("user_data.txt", "r");
    if (file) {
        char line[1000];
        while (fgets(line, sizeof(line), file)) {
            User user;
            sscanf(line, "%d,%[^,],%[^,],%[^,],%[^,],%lf,%lf,%d,%[^,],%d,%d,%[^,],%d",
                   &user.id, user.email, user.name, user.password, user.NRC, &user.amount,
                   &user.loan, &user.transition_records, user.phone_number,
                   &user.cash_in_records, &user.cash_out_records, user.address, &user.account_status);
            root = insertUser(root, user);
            nextId = user.id + 1;
        }
        fclose(file);
    }

    int choice;
    do {
        printf("\n===== Welcome from NCC Bank Management System =====\n\n");
        printf("If you are new, you need to Register ==> 1.\n If you've already registered, login ==> 2. \n\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                User newUser;
                newUser.id = nextId++;
                email_valid=-1;

                while (email_valid==-1) {
                    printf("\nEnter your email:");
                    scanf("%s", newUser.email);


                    emailValidation(newUser.email);
                    if (email_valid == -1) {
                        printf("Your email format is not valid\n");
                    }
                }

                printf("Enter Name: ");
                scanf(" %s", newUser.name);

                pass_strength=-1;
                while (pass_strength==-1) {
                    printf("Enter Password: ");
                    scanf("%s", newUser.password);


                    checkPasswordStrength(newUser.password);
                    if (pass_strength == -1) {
                        printf("Your password is weak.\n\n");
                    }
                    else{
                        printf("Your password strength is good.\n\n");
                    }
                }



                nrc_valid=-1;
                while (nrc_valid==-1) {
                    printf("Enter NRC: ");
                    scanf("%s", newUser.NRC);


                    isValidNRC(newUser.NRC);
                    if (nrc_valid == -1) {
                        printf("Your nrc format is not valid\n\n");
                    }
                }


                chk_amt=-1;
                while (chk_amt==-1) {
                    printf("Enter Amount: ");
                    scanf("%lf", &newUser.amount);


                    checkAmount(newUser.amount);
                    if (chk_amt == -1) {
                        printf("Minimum $200 deposit required.\n\n");
                    }
                }

                printf("Enter Phone Number: ");
                scanf("%s", newUser.phone_number);
                printf("Enter Address: ");
                scanf("%s", newUser.address);
                printf("Enter Account Status (0-Personal, 1-Business): ");
                scanf("%d", &newUser.account_status);


                root = insertUser(root, newUser);
                saveUserToFile(newUser);

                printf("Registration successful! Your user ID is: %d\n", newUser.id);
                break;
            }
            case 2: {
                int userId;
                char password[20];
                printf("\nEnter Your User ID: ");
                scanf("%d", &userId);
                printf("Enter Your Password: ");
                scanf("%s", password);

                // Search for the user in the AVL tree
                AVLNode* currentNode = root;
                while (currentNode != NULL) {
                    if (userId < currentNode->user.id)
                        currentNode = currentNode->left;
                    else if (userId > currentNode->user.id)
                        currentNode = currentNode->right;
                    else {
                        if (strcmp(password, currentNode->user.password) == 0) {
                            printf("\nLogin Successful! Welcome, %s!\n", currentNode->user.name);
                            displayUserInfo(userId);
                            updateUserInfo(userId,currentNode->user.account_status);
                            break;
                        } else {
                            printf("\nIncorrect password. Login failed!\n");
                            break;
                        }
                    }
                }

                if (currentNode == NULL)
                    printf("\nUser ID not found. Login failed!\n");

                break;
            }
            case 3: {
                // Exit the program
                break;
            }
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 3);

    return 0;
}
