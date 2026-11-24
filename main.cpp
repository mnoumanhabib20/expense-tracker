#include <cctype>
#include <iostream>
#include <string>

// Use standard-library names without adding std:: before each one.
using namespace std;

// Set the habit limit for each account.
const int MAX_HABITS = 6;

// Set the account limit for this class project.
const int MAX_USERS = 10;

// Set the starting number of habits for a new account.
const int STARTING_HABITS = 5;

// Keep the seven most recent finished-day scores for each account.
const int WEEK_DAYS = 7;

// Set the productivity percentage needed to keep a streak.
const double STREAK_TARGET = 80.0;

// Store every user's name in one account array.
string userNames[MAX_USERS];

// Store every user's email in one account array.
string userEmails[MAX_USERS];

// Store every user's password in one account array.
string userPasswords[MAX_USERS];

// Store each user's habit name at matching user and habit indexes.
string userHabitNames[MAX_USERS][MAX_HABITS];

// Store each user's target minutes at matching indexes.
int userHabitMinutes[MAX_USERS][MAX_HABITS];

// Store each user's completion state at matching indexes.
bool userHabitDone[MAX_USERS][MAX_HABITS];

// Store each user's number of habits.
int userHabitCount[MAX_USERS];

// Store each user's current streak.
int userCurrentStreak[MAX_USERS];

// Store each user's best streak.
int userBestStreak[MAX_USERS];

// Store each user's number of logged days.
int userDaysLogged[MAX_USERS];

// Store each user's seven most recent productivity scores.
double userWeeklyScores[MAX_USERS][WEEK_DAYS];

// Store each user's completed focus minutes for the same seven days.
int userWeeklyMinutes[MAX_USERS][WEEK_DAYS];

// Track how many accounts currently exist.
int userCount = 0;

// Use -1 to show that no user is currently active.
int activeUserIndex = -1;

// Track whether the administrator is signed in.
bool adminLoggedIn = false;

// Set the default administrator password.
string adminPassword = "146524";

// int; called by the operating system; runs the login and tracker menus.
int main();

// bool; called by signUp and changeAdminPassword; checks for visible text.
bool hasNonSpace(const string value);

// bool; called by addHabit; checks the habit name rules.
bool isValidHabitName(const string habitName);

// bool; called by signUp; checks that the email contains an at sign.
bool isValidEmail(const string email);

// int; called by signUp and loginUser; finds an account by email.
int findUserByEmail(const string email);

// void; called by logged-out menu; creates a new user account.
void signUp();

// void; called by logged-out menu; checks credentials and starts a user session.
void loginUser();

// void; called by user and logged-out menus; starts an administrator session.
void loginAdmin();

// void; called by admin menu; displays account names and emails.
void viewUsers();

// void; called by admin menu; deletes an account and shifts its data.
void deleteUserAccount(int userChoice);

// void; called by admin menu; updates the administrator password.
void changeAdminPassword();

// int; called by habit actions; reads one whole number from the console.
int readInteger(const string prompt);

// void; called by user menu; adds a habit and a positive target time.
void addHabit();

// void; called by user menu; prints only the active user's habits.
void viewHabits();

// bool; called by user menu; toggles a valid habit number.
bool toggleHabitStatus(bool done[], int count, int userChoice);

// void; called by user menu; removes one habit and shifts the remaining entries.
void deleteHabit(int userChoice);

// int; called by summary functions; counts completed habits.
int countCompletedHabits(const bool done[], int count);

// int; called by summary functions; counts pending habits.
int countPendingHabits(const bool done[], int count);

// int; called by daily summary; adds every target minute.
int calculateTotalMinutes(const int minutes[], int count);

// int; called by daily summary; adds minutes for completed habits.
int calculateCompletedMinutes(const int minutes[], const bool done[], int count);

// double; called by summary and finishDay; calculates the productivity score.
double calculateProductivityScore(const bool done[], int count);

// int; called by Smart Analytics; finds the highest-minute pending habit.
int findHighestPriorityPendingHabit(const int minutes[], const bool done[], int count);

// void; called by viewHabits and viewAnalytics; prints this user's daily metrics.
void printDailySummary(const int minutes[], const bool done[], int count, int userIndex);

// void; called by user menu; prints priority and recursive pending details.
void viewAnalytics();

// recursive void; called by viewAnalytics; prints pending habits one index at a time.
void printPendingBreakdownRecursive(const string names[], const int minutes[], const bool done[],
                                    int count, int index);

// void; called by user menu; records the day and resets habits for this user.
void finishDay();

// void; called by finishDay; clears one user's completion flags.
void resetDailyHabits(bool done[], int count);

// int; called by the operating system; runs the login and tracker menus.
int main()
{
    // Store the selected menu character.
    char choice;

    // Continue until the user chooses to exit or input ends.
    while (true)
    {
        // Show administrator options while the administrator is signed in.
        if (adminLoggedIn)
        {
            // Print the administrator menu.
            cout << "\nAdmin Panel\n";
            cout << "1. View user accounts\n";
            cout << "2. Delete a user account\n";
            cout << "3. Change admin password\n";
            cout << "4. Admin logout\n";
            cout << "5. Exit\n";
        }
        // Show habit options while a user account is active.
        else if (activeUserIndex >= 0)
        {
            // Print the active user's menu without showing other accounts.
            cout << "\nWelcome, " << userNames[activeUserIndex] << "\n";
            cout << "1. View habits and summary\n";
            cout << "2. Add habit with target minutes\n";
            cout << "3. Toggle habit Done / Pending\n";
            cout << "4. Delete a habit\n";
            cout << "5. Finish Day / Start Next Day\n";
            cout << "6. View Smart Analytics & Recursive Summary\n";
            cout << "7. Admin login\n";
            cout << "8. Logout\n";
            cout << "9. Exit\n";
        }
        // Show account options when no user is active.
        else
        {
            // Print the login and sign-up menu.
            cout << "\nHabit & Daily Focus Streak Tracker\n";
            cout << "1. Login\n";
            cout << "2. Sign up\n";
            cout << "3. Admin login\n";
            cout << "4. Exit\n";
        }

        // Ask for one menu selection.
        cout << "Choose an option: ";

        // Stop when the input stream has ended.
        if (!(cin >> choice))
        {
            // Leave the menu loop.
            break;
        }

        // Clear the menu line before any later getline operation.
        cin.ignore(1000, '\n');

        // Run only the menu that matches the current session.
        if (adminLoggedIn)
        {
            // Handle administrator actions.
            switch (choice)
            {
                // Show registered account names and emails.
                case '1':
                    viewUsers();
                    break;

                // Ask which account the administrator wants to delete.
                case '2':
                {
                    // Store the 1-based account number.
                    int userChoice = readInteger("Enter the user number to delete: ");

                    // Delete only a number inside the current account range.
                    if (userChoice >= 1 && userChoice <= userCount)
                    {
                        // Delete the selected account and its private data.
                        deleteUserAccount(userChoice);
                    }
                    // Reject account numbers outside the visible range.
                    else
                    {
                        // Explain the valid user number range.
                        cout << "Please enter a user number from 1 to " << userCount << ".\n";
                    }
                    break;
                }

                // Let the administrator choose a replacement password.
                case '3':
                    changeAdminPassword();
                    break;

                // Leave administrator mode.
                case '4':
                    adminLoggedIn = false;
                    cout << "Admin logged out.\n";
                    break;

                // Exit the whole program.
                case '5':
                    return 0;

                // Explain an unrecognized admin selection.
                default:
                    cout << "Please choose 1, 2, 3, 4, or 5.\n";
                    break;
            }
        }
        // Handle actions for the active user's own data.
        else if (activeUserIndex >= 0)
        {
            // Run the selected habit action.
            switch (choice)
            {
                // View this user's habits and summary.
                case '1':
                    viewHabits();
                    break;

                // Add a new habit and its target minutes.
                case '2':
                    addHabit();
                    break;

                // Ask for a habit number and toggle its state.
                case '3':
                {
                    // Store the 1-based habit number.
                    int userChoice = readInteger("Enter the habit number to toggle: ");

                    // Toggle only when the number is valid.
                    if (toggleHabitStatus(userHabitDone[activeUserIndex],
                                          userHabitCount[activeUserIndex], userChoice))
                    {
                        // Confirm the status change.
                        cout << "Habit status updated.\n";
                    }
                    // Reject an invalid habit number.
                    else
                    {
                        // Explain the valid habit number range.
                        cout << "Please enter a habit number from 1 to "
                             << userHabitCount[activeUserIndex] << ".\n";
                    }
                    break;
                }

                // Ask which habit to remove from this account.
                case '4':
                {
                    // Store the 1-based habit number.
                    int userChoice = readInteger("Enter the habit number to delete: ");

                    // Delete a habit only when its number is in range.
                    if (userChoice >= 1 && userChoice <= userHabitCount[activeUserIndex])
                    {
                        // Remove the selected habit from this user's arrays.
                        deleteHabit(userChoice);
                    }
                    // Reject a habit number outside this user's list.
                    else
                    {
                        // Explain the valid habit number range.
                        cout << "Please enter a habit number from 1 to "
                             << userHabitCount[activeUserIndex] << ".\n";
                    }
                    break;
                }

                // Finish this user's day and clear only this user's habits.
                case '5':
                    finishDay();
                    break;

                // Show this user's smart analytics.
                case '6':
                    viewAnalytics();
                    break;

                // Allow the admin to open the admin panel.
                case '7':
                    loginAdmin();
                    break;

                // Log out the current user.
                case '8':
                    activeUserIndex = -1;
                    cout << "User logged out.\n";
                    break;

                // Exit the whole program.
                case '9':
                    return 0;

                // Explain an unrecognized user selection.
                default:
                    cout << "Please choose a number from 1 to 9.\n";
                    break;
            }
        }
        // Handle login and sign-up actions when no user is active.
        else
        {
            // Run the selected account action.
            switch (choice)
            {
                // Ask an existing user to log in.
                case '1':
                    loginUser();
                    break;

                // Create a new account.
                case '2':
                    signUp();
                    break;

                // Ask for the administrator password.
                case '3':
                    loginAdmin();
                    break;

                // Exit the whole program.
                case '4':
                    return 0;

                // Explain an unrecognized account selection.
                default:
                    cout << "Please choose 1, 2, 3, or 4.\n";
                    break;
            }
        }
    }

    // End successfully when input is closed.
    return 0;
}

// bool; called by signUp and changeAdminPassword; checks for visible text.
bool hasNonSpace(const string value)
{
    // Check each character for something other than whitespace.
    for (unsigned int i = 0; i < value.length(); i++)
    {
        // Accept the text when one visible character is found.
        if (!isspace(value[i]))
        {
            // Report that the text is not blank.
            return true;
        }
    }

    // Report that the text is empty or spaces only.
    return false;
}

// bool; called by addHabit; checks the habit name rules.
bool isValidHabitName(const string habitName)
{
    // Reject empty and overlong habit names.
    if (habitName.length() == 0 || habitName.length() >= 40)
    {
        // Report that the length is invalid.
        return false;
    }

    // Track whether the name contains at least one letter.
    bool hasLetter = false;

    // Check every character in the habit name.
    for (unsigned int i = 0; i < habitName.length(); i++)
    {
        // Mark the name when an alphabetic character is found.
        if (isalpha(habitName[i]))
        {
            // Remember that a letter exists.
            hasLetter = true;
        }
    }

    // Accept only names with a visible character and a letter.
    if (hasNonSpace(habitName) && hasLetter)
    {
        // Report that the name is valid.
        return true;
    }
    // Reject spaces-only names and names without letters.
    else
    {
        // Report that the name is invalid.
        return false;
    }
}

// bool; called by signUp; checks that the email contains an at sign.
bool isValidEmail(const string email)
{
    // Reject an empty email.
    if (email.length() == 0)
    {
        // Report that the email is missing.
        return false;
    }

    // Check each character for the required at sign.
    for (unsigned int i = 0; i < email.length(); i++)
    {
        // Accept the email when an at sign is found.
        if (email[i] == '@')
        {
            // Report that the basic email rule passed.
            return true;
        }
    }

    // Reject an email without an at sign.
    return false;
}

// int; called by signUp and loginUser; finds an account by email.
int findUserByEmail(const string email)
{
    // Check each registered email.
    for (int i = 0; i < userCount; i++)
    {
        // Return the matching user index.
        if (userEmails[i] == email)
        {
            // Give the caller this user's array position.
            return i;
        }
    }

    // Return -1 when no account matches.
    return -1;
}

// void; called by logged-out menu; creates a new user account.
void signUp()
{
    // Stop safely when all user slots are taken.
    if (userCount >= MAX_USERS)
    {
        // Tell the user the account limit.
        cout << "The user limit has been reached.\n";
        return;
    }

    // Store the new user's name.
    string newName;

    // Ask for the user's name.
    cout << "Enter your name: ";
    getline(cin, newName);

    // Reject an empty or spaces-only name.
    if (!hasNonSpace(newName))
    {
        // Explain that the name is required.
        cout << "Name cannot be empty.\n";
        return;
    }

    // Store the new user's email.
    string newEmail;

    // Ask for the user's email.
    cout << "Enter your email: ";
    getline(cin, newEmail);

    // Require a non-empty email containing an at sign.
    if (!isValidEmail(newEmail))
    {
        // Explain the basic email rule.
        cout << "Email must be non-empty and contain @.\n";
        return;
    }

    // Reject an email already assigned to an account.
    if (findUserByEmail(newEmail) >= 0)
    {
        // Explain that every account needs a unique email.
        cout << "That email is already registered.\n";
        return;
    }

    // Store the new user's password.
    string newPassword;

    // Ask for a non-empty password.
    cout << "Enter your password: ";
    getline(cin, newPassword);

    // Reject an empty or spaces-only password.
    if (!hasNonSpace(newPassword))
    {
        // Explain that a password is required.
        cout << "Password cannot be empty.\n";
        return;
    }

    // Store the account details at the next free user index.
    userNames[userCount] = newName;
    userEmails[userCount] = newEmail;
    userPasswords[userCount] = newPassword;

    // Add five simple starter habits to the new user's arrays.
    userHabitNames[userCount][0] = "Read for class";
    userHabitNames[userCount][1] = "Review lecture notes";
    userHabitNames[userCount][2] = "Practice a subject";
    userHabitNames[userCount][3] = "Take a study break";
    userHabitNames[userCount][4] = "Plan tomorrow";

    // Set the starter target minutes.
    userHabitMinutes[userCount][0] = 30;
    userHabitMinutes[userCount][1] = 20;
    userHabitMinutes[userCount][2] = 45;
    userHabitMinutes[userCount][3] = 10;
    userHabitMinutes[userCount][4] = 5;

    // Mark all starter habits pending and clear unused slots.
    for (int i = 0; i < MAX_HABITS; i++)
    {
        // Start each habit as not completed.
        userHabitDone[userCount][i] = false;
    }

    // Start the weekly chart with seven empty scores.
    for (int i = 0; i < WEEK_DAYS; i++)
    {
        // Set this day's score and minutes to zero.
        userWeeklyScores[userCount][i] = 0.0;
        userWeeklyMinutes[userCount][i] = 0;
    }

    // Set this account's initial habit and streak totals.
    userHabitCount[userCount] = STARTING_HABITS;
    userCurrentStreak[userCount] = 0;
    userBestStreak[userCount] = 0;
    userDaysLogged[userCount] = 0;

    // Log the new user in after account creation.
    activeUserIndex = userCount;

    // Add the new account to the registered user count.
    userCount++;

    // Welcome the user to their separate habit list.
    cout << "Account created. Welcome, " << userNames[activeUserIndex] << "!\n";
}

// void; called by logged-out menu; checks credentials and starts a user session.
void loginUser()
{
    // Store the email entered at login.
    string loginEmail;

    // Ask for the account email.
    cout << "Enter your email: ";
    getline(cin, loginEmail);

    // Store the password entered at login.
    string loginPassword;

    // Ask for the account password.
    cout << "Enter your password: ";
    getline(cin, loginPassword);

    // Find the account that owns this email.
    int userIndex = findUserByEmail(loginEmail);

    // Accept the login only when both values match.
    if (userIndex >= 0 && userPasswords[userIndex] == loginPassword)
    {
        // Make the matching account active.
        activeUserIndex = userIndex;

        // Welcome the account without showing another user's data.
        cout << "Welcome back, " << userNames[activeUserIndex] << "!\n";
    }
    // Reject credentials that do not match one account.
    else
    {
        // Show a general login error.
        cout << "Email or password is incorrect.\n";
    }
}

// void; called by user and logged-out menus; starts an administrator session.
void loginAdmin()
{
    // Store the password entered for admin access.
    string enteredPassword;

    // Ask for the administrator password.
    cout << "Enter admin password: ";
    getline(cin, enteredPassword);

    // Open the panel only when the password matches.
    if (enteredPassword == adminPassword)
    {
        // Mark the administrator as signed in.
        adminLoggedIn = true;

        // Confirm that the protected panel is open.
        cout << "Admin login successful.\n";
    }
    // Keep the panel closed when the password is wrong.
    else
    {
        // Explain that access was denied.
        cout << "Admin password is incorrect.\n";
    }
}

// void; called by admin menu; displays account names and emails.
void viewUsers()
{
    // Explain the empty account list.
    if (userCount == 0)
    {
        // Tell the administrator there are no accounts.
        cout << "No user accounts have been created.\n";
        return;
    }

    // Print the account list heading.
    cout << "\nRegistered users:\n";

    // Show each account's public class-project details.
    for (int i = 0; i < userCount; i++)
    {
        // Show the user number, name, and email without the password.
        cout << i + 1 << ". " << userNames[i] << " - " << userEmails[i] << "\n";
    }
}

// void; called by admin menu; deletes an account and shifts its data.
void deleteUserAccount(int userChoice)
{
    // Convert the user-facing number to an array index.
    int deletedIndex = userChoice - 1;

    // Log out the active account when it is the one being deleted.
    if (activeUserIndex == deletedIndex)
    {
        // Reset the active user marker.
        activeUserIndex = -1;
    }
    // Move the active user index when an earlier account is removed.
    else if (activeUserIndex > deletedIndex)
    {
        // Keep the active user attached to the same account after shifting.
        activeUserIndex--;
    }

    // Shift every account after the removed user one position left.
    for (int i = deletedIndex; i < userCount - 1; i++)
    {
        // Copy the account's basic details.
        userNames[i] = userNames[i + 1];
        userEmails[i] = userEmails[i + 1];
        userPasswords[i] = userPasswords[i + 1];

        // Copy that account's habit count and streak totals.
        userHabitCount[i] = userHabitCount[i + 1];
        userCurrentStreak[i] = userCurrentStreak[i + 1];
        userBestStreak[i] = userBestStreak[i + 1];
        userDaysLogged[i] = userDaysLogged[i + 1];

        // Copy that account's recent score history.
        for (int j = 0; j < WEEK_DAYS; j++)
        {
            // Shift the matching score and minute totals.
            userWeeklyScores[i][j] = userWeeklyScores[i + 1][j];
            userWeeklyMinutes[i][j] = userWeeklyMinutes[i + 1][j];
        }

        // Copy every habit field for that account.
        for (int j = 0; j < MAX_HABITS; j++)
        {
            // Shift the habit name.
            userHabitNames[i][j] = userHabitNames[i + 1][j];

            // Shift the target minutes.
            userHabitMinutes[i][j] = userHabitMinutes[i + 1][j];

            // Shift the completion state.
            userHabitDone[i][j] = userHabitDone[i + 1][j];
        }
    }

    // Remove the now-unused last account slot.
    userCount--;

    // Clear the old account details from the unused slot.
    userNames[userCount] = "";
    userEmails[userCount] = "";
    userPasswords[userCount] = "";

    // Clear the unused account's habit and streak values.
    userHabitCount[userCount] = 0;
    userCurrentStreak[userCount] = 0;
    userBestStreak[userCount] = 0;
    userDaysLogged[userCount] = 0;

    // Clear the removed account's score history.
    for (int j = 0; j < WEEK_DAYS; j++)
    {
        // Remove the old score and minute totals.
        userWeeklyScores[userCount][j] = 0.0;
        userWeeklyMinutes[userCount][j] = 0;
    }

    // Clear every habit value from the unused account slot.
    for (int j = 0; j < MAX_HABITS; j++)
    {
        // Remove the old habit name.
        userHabitNames[userCount][j] = "";

        // Remove the old habit target.
        userHabitMinutes[userCount][j] = 0;

        // Mark the old habit as not done.
        userHabitDone[userCount][j] = false;
    }

    // Confirm the account was removed.
    cout << "User account deleted.\n";
}

// void; called by admin menu; updates the administrator password.
void changeAdminPassword()
{
    // Store the replacement administrator password.
    string newPassword;

    // Ask the signed-in administrator for a new password.
    cout << "Enter a new admin password: ";
    getline(cin, newPassword);

    // Reject an empty or spaces-only replacement.
    if (!hasNonSpace(newPassword))
    {
        // Explain that the replacement cannot be blank.
        cout << "Admin password cannot be empty.\n";
        return;
    }

    // Save the new password for this program session.
    adminPassword = newPassword;

    // Confirm the password change.
    cout << "Admin password updated.\n";
}

// int; called by habit actions; reads one whole number from the console.
int readInteger(const string prompt)
{
    // Keep asking until a whole number is entered.
    while (true)
    {
        // Show the caller's question.
        cout << prompt;

        // Store the number typed by the user.
        int value;

        // Return the value when integer input succeeds.
        if (cin >> value)
        {
            // Clear the rest of the input line.
            cin.ignore(1000, '\n');

            // Give the requested number back to the caller.
            return value;
        }
        // Recover when the input is not a whole number.
        else
        {
            // Clear the failed input state.
            cin.clear();

            // Remove invalid characters from the line.
            cin.ignore(1000, '\n');

            // Explain the required input type.
            cout << "Please enter a whole number.\n";
        }
    }
}

// void; called by user menu; adds a habit and a positive target time.
void addHabit()
{
    // Stop safely when this user's habit list is full.
    if (userHabitCount[activeUserIndex] >= MAX_HABITS)
    {
        // Tell the active user the habit limit.
        cout << "Your habit list is full.\n";
        return;
    }

    // Store the new habit name.
    string newHabitName;

    // Keep asking until this user enters a valid name.
    while (true)
    {
        // Ask for a habit name.
        cout << "Enter a habit name (1 to 39 characters): ";
        getline(cin, newHabitName);

        // Stop asking after the name passes the validation rules.
        if (isValidHabitName(newHabitName))
        {
            // Leave the validation loop.
            break;
        }
        // Explain why the name was rejected.
        else
        {
            // Show the name rules.
            cout << "Use a name with a letter and fewer than 40 characters.\n";
        }
    }

    // Store the new target minute value.
    int targetMinutes;

    // Keep asking until a positive integer is entered.
    while (true)
    {
        // Read a target using the shared whole-number helper.
        targetMinutes = readInteger("Enter target minutes (positive integer): ");

        // Leave the loop after a positive target is entered.
        if (targetMinutes > 0)
        {
            // Keep the accepted target value.
            break;
        }
        // Reject zero and negative target values.
        else
        {
            // Explain the target rule.
            cout << "Target minutes must be greater than zero.\n";
        }
    }

    // Store the new values at the active user's first unused habit index.
    userHabitNames[activeUserIndex][userHabitCount[activeUserIndex]] = newHabitName;
    userHabitMinutes[activeUserIndex][userHabitCount[activeUserIndex]] = targetMinutes;
    userHabitDone[activeUserIndex][userHabitCount[activeUserIndex]] = false;

    // Increase only the active user's habit count.
    userHabitCount[activeUserIndex]++;

    // Confirm the new habit and its target.
    cout << "Habit added with a " << targetMinutes << "-minute target.\n";
}

// void; called by user menu; prints only the active user's habits.
void viewHabits()
{
    // Use this user's arrays and streak values to build the daily summary.
    printDailySummary(userHabitMinutes[activeUserIndex], userHabitDone[activeUserIndex],
                      userHabitCount[activeUserIndex], activeUserIndex);

    // Explain the empty habit-list case.
    if (userHabitCount[activeUserIndex] == 0)
    {
        // Tell the user there are no habits.
        cout << "No habits have been added yet.\n";
        return;
    }

    // Add a heading before this user's habits.
    cout << "\nYour habits:\n";

    // Visit every habit owned by the active user.
    for (int i = 0; i < userHabitCount[activeUserIndex]; i++)
    {
        // Show the user-facing number, name, and target.
        cout << i + 1 << ". " << userHabitNames[activeUserIndex][i]
             << " - target: " << userHabitMinutes[activeUserIndex][i] << " minutes";

        // Show the done state for this user's habit.
        if (userHabitDone[activeUserIndex][i])
        {
            // Print the completed label.
            cout << " - done\n";
        }
        // Show the pending state for this user's habit.
        else
        {
            // Print the pending label.
            cout << " - not done\n";
        }
    }
}

// bool; called by user menu; toggles a valid habit number.
bool toggleHabitStatus(bool done[], int count, int userChoice)
{
    // Reject numbers outside the user range from one through count.
    if (userChoice < 1 || userChoice > count)
    {
        // Report that no habit changed.
        return false;
    }

    // Convert the user-facing number to a zero-based index.
    int habitIndex = userChoice - 1;

    // Return a completed habit to pending.
    if (done[habitIndex])
    {
        // Store the pending state.
        done[habitIndex] = false;
    }
    // Mark a pending habit completed.
    else
    {
        // Store the completed state.
        done[habitIndex] = true;
    }

    // Report a successful toggle.
    return true;
}

// void; called by user menu; removes a habit and shifts the remaining entries.
void deleteHabit(int userChoice)
{
    // Convert the user-facing habit number to a zero-based index.
    int deletedIndex = userChoice - 1;

    // Shift later habits over the deleted position.
    for (int i = deletedIndex; i < userHabitCount[activeUserIndex] - 1; i++)
    {
        // Copy the following habit's name.
        userHabitNames[activeUserIndex][i] = userHabitNames[activeUserIndex][i + 1];

        // Copy the following habit's target minutes.
        userHabitMinutes[activeUserIndex][i] = userHabitMinutes[activeUserIndex][i + 1];

        // Copy the following habit's completion state.
        userHabitDone[activeUserIndex][i] = userHabitDone[activeUserIndex][i + 1];
    }

    // Decrease only this user's habit count.
    userHabitCount[activeUserIndex]--;

    // Clear the unused position at the end of this user's list.
    userHabitNames[activeUserIndex][userHabitCount[activeUserIndex]] = "";
    userHabitMinutes[activeUserIndex][userHabitCount[activeUserIndex]] = 0;
    userHabitDone[activeUserIndex][userHabitCount[activeUserIndex]] = false;

    // Confirm that the selected habit was removed.
    cout << "Habit deleted.\n";
}

// int; called by summary functions; counts completed habits.
int countCompletedHabits(const bool done[], int count)
{
    // Start the completed count at zero.
    int completed = 0;

    // Check every used completion flag.
    for (int i = 0; i < count; i++)
    {
        // Count a habit when its flag is true.
        if (done[i])
        {
            // Increase the completed count.
            completed++;
        }
    }

    // Return the completed habit count.
    return completed;
}

// int; called by summary functions; counts pending habits.
int countPendingHabits(const bool done[], int count)
{
    // Start the pending count at zero.
    int pending = 0;

    // Check every used completion flag.
    for (int i = 0; i < count; i++)
    {
        // Count a habit when its flag is false.
        if (!done[i])
        {
            // Increase the pending count.
            pending++;
        }
    }

    // Return the pending habit count.
    return pending;
}

// int; called by daily summary; adds every target minute.
int calculateTotalMinutes(const int minutes[], int count)
{
    // Start the full target at zero.
    int totalMinutes = 0;

    // Add target minutes from each used array position.
    for (int i = 0; i < count; i++)
    {
        // Include this habit's target.
        totalMinutes = totalMinutes + minutes[i];
    }

    // Return the total target minutes.
    return totalMinutes;
}

// int; called by daily summary; adds minutes for completed habits.
int calculateCompletedMinutes(const int minutes[], const bool done[], int count)
{
    // Start the completed minute total at zero.
    int completedMinutes = 0;

    // Check every habit and its completion state.
    for (int i = 0; i < count; i++)
    {
        // Include minutes only when the habit is done.
        if (done[i])
        {
            // Add this completed habit's target.
            completedMinutes = completedMinutes + minutes[i];
        }
    }

    // Return completed focus minutes.
    return completedMinutes;
}

// double; called by summary and finishDay; calculates the productivity score.
double calculateProductivityScore(const bool done[], int count)
{
    // Avoid division by zero for an empty habit list.
    if (count == 0)
    {
        // Return a zero score.
        return 0.0;
    }

    // Count completed habits before calculating the percentage.
    int completed = countCompletedHabits(done, count);

    // Convert to double so division keeps its fractional part.
    double score = static_cast<double>(completed) / count * 100.0;

    // Return the daily percentage.
    return score;
}

// int; called by Smart Analytics; finds the highest-minute pending habit.
int findHighestPriorityPendingHabit(const int minutes[], const bool done[], int count)
{
    // Start with no pending habit selected.
    int highestIndex = -1;

    // Check each used habit index.
    for (int i = 0; i < count; i++)
    {
        // Consider only habits that are pending.
        if (!done[i])
        {
            // Select the first pending habit or a higher-minute habit.
            if (highestIndex == -1 || minutes[i] > minutes[highestIndex])
            {
                // Save this habit's index.
                highestIndex = i;
            }
        }
    }

    // Return -1 when no pending habit exists.
    return highestIndex;
}

// void; called by viewHabits and viewAnalytics; prints this user's daily metrics.
void printDailySummary(const int minutes[], const bool done[], int count, int userIndex)
{
    // Get this user's completed habit count.
    int completed = countCompletedHabits(done, count);

    // Get this user's pending habit count.
    int pending = countPendingHabits(done, count);

    // Get this user's full target minutes.
    int totalMinutes = calculateTotalMinutes(minutes, count);

    // Get this user's completed focus minutes.
    int completedMinutes = calculateCompletedMinutes(minutes, done, count);

    // Get this user's productivity percentage.
    double score = calculateProductivityScore(done, count);

    // Print the user's own counts and minutes.
    cout << "\nDaily Summary for " << userNames[userIndex] << "\n";
    cout << "Completed habits: " << completed << "\n";
    cout << "Pending habits: " << pending << "\n";
    cout << "Target minutes: " << totalMinutes << "\n";
    cout << "Completed minutes: " << completedMinutes << " / " << totalMinutes << "\n";
    cout << "Productivity score: " << score << "%\n";

    // Print this user's streak and day totals.
    cout << "Current streak: " << userCurrentStreak[userIndex] << " day(s)\n";
    cout << "Best streak: " << userBestStreak[userIndex] << " day(s)\n";
    cout << "Days logged: " << userDaysLogged[userIndex] << "\n";
}

// void; called by user menu; prints priority and recursive pending details.
void viewAnalytics()
{
    // Reuse the nested-call daily summary for the active account.
    printDailySummary(userHabitMinutes[activeUserIndex], userHabitDone[activeUserIndex],
                      userHabitCount[activeUserIndex], activeUserIndex);

    // Find this user's highest-priority pending habit.
    int highestIndex = findHighestPriorityPendingHabit(
        userHabitMinutes[activeUserIndex], userHabitDone[activeUserIndex],
        userHabitCount[activeUserIndex]);

    // Show the highest-priority pending habit when one exists.
    if (highestIndex >= 0)
    {
        // Print only this user's habit details.
        cout << "Highest-priority pending habit: " << userHabitNames[activeUserIndex][highestIndex]
             << " (" << userHabitMinutes[activeUserIndex][highestIndex] << " minutes)\n";
    }
    // Explain when there are no pending habits.
    else if (userHabitCount[activeUserIndex] == 0)
    {
        // Report an empty habit list.
        cout << "No habits are in the list.\n";
    }
    // Explain when all existing habits are completed.
    else
    {
        // Report the empty pending list.
        cout << "All habits are completed.\n";
    }

    // Label the recursive pending list.
    cout << "Recursive pending breakdown:\n";

    // Print pending details recursively from index zero.
    printPendingBreakdownRecursive(userHabitNames[activeUserIndex],
                                   userHabitMinutes[activeUserIndex],
                                   userHabitDone[activeUserIndex],
                                   userHabitCount[activeUserIndex], 0);

    // Explain when there are no pending rows to print.
    if (countPendingHabits(userHabitDone[activeUserIndex], userHabitCount[activeUserIndex]) == 0)
    {
        // Print the empty pending message.
        cout << "No pending habits.\n";
    }

    // Print the recent finished-day scores and minutes for chart data.
    cout << "Recent finished-day data, oldest to newest:\n";
    for (int i = WEEK_DAYS - 1; i >= 0; i--)
    {
        // Show one day's score and completed focus minutes.
        cout << userWeeklyScores[activeUserIndex][i] << "% / "
             << userWeeklyMinutes[activeUserIndex][i] << " min; ";
    }
    cout << "\n";
}

// recursive void; called by viewAnalytics; prints pending habits one index at a time.
void printPendingBreakdownRecursive(const string names[], const int minutes[], const bool done[],
                                    int count, int index)
{
    // Stop safely at the end of the array, including when count is zero.
    if (index >= count)
    {
        // Finish the recursive base case.
        return;
    }

    // Print this habit only when it is pending.
    if (!done[index])
    {
        // Show this pending habit and its target.
        cout << "- " << names[index] << " (" << minutes[index] << " minutes)\n";
    }

    // Continue with the next habit index.
    printPendingBreakdownRecursive(names, minutes, done, count, index + 1);
}

// void; called by user menu; records the day and resets habits for this user.
void finishDay()
{
    // Calculate this user's score before resetting habits.
    double score = calculateProductivityScore(userHabitDone[activeUserIndex],
                                              userHabitCount[activeUserIndex]);
    // Calculate focus minutes earned on this user's day.
    int completedMinutes = calculateCompletedMinutes(
        userHabitMinutes[activeUserIndex], userHabitDone[activeUserIndex],
        userHabitCount[activeUserIndex]);

    // Count this user's finished day.
    userDaysLogged[activeUserIndex]++;

    // Shift older scores one place to make room for today.
    for (int i = WEEK_DAYS - 1; i > 0; i--)
    {
        // Copy the preceding score and minutes into this position.
        userWeeklyScores[activeUserIndex][i] = userWeeklyScores[activeUserIndex][i - 1];
        userWeeklyMinutes[activeUserIndex][i] = userWeeklyMinutes[activeUserIndex][i - 1];
    }

    // Store today's score and focus minutes at the newest position.
    userWeeklyScores[activeUserIndex][0] = score;
    userWeeklyMinutes[activeUserIndex][0] = completedMinutes;

    // Continue this user's streak when the score meets the target.
    if (score >= STREAK_TARGET)
    {
        // Increase the active user's current streak.
        userCurrentStreak[activeUserIndex]++;

        // Save a best streak only when the current one is greater.
        if (userCurrentStreak[activeUserIndex] > userBestStreak[activeUserIndex])
        {
            // Store the new best streak.
            userBestStreak[activeUserIndex] = userCurrentStreak[activeUserIndex];
        }
    }
    // Reset this user's current streak when the score is too low.
    else
    {
        // Keep the best value and clear the current streak.
        userCurrentStreak[activeUserIndex] = 0;
    }

    // Reset only this user's completion flags.
    resetDailyHabits(userHabitDone[activeUserIndex], userHabitCount[activeUserIndex]);

    // Report this user's score and focus minutes for the day.
    cout << "Day logged at " << score << "% with "
         << completedMinutes << " focus minutes.\n";
}

// void; called by finishDay; clears one user's completion flags.
void resetDailyHabits(bool done[], int count)
{
    // Visit each habit in this user's list.
    for (int i = 0; i < count; i++)
    {
        // Mark this habit pending for the next day.
        done[i] = false;
    }
}
