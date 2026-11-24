// Set the habit limit for each account.
const MAX_HABITS = 6;

// Set the score needed to keep a daily streak.
const STREAK_TARGET = 80;

// Use separate browser storage keys for accounts and the admin password.
const USERS_STORAGE_KEY = "habitTrackerUsers";
const ADMIN_STORAGE_KEY = "habitTrackerAdminPassword";

// Keep the required first-run admin password for this class demo.
const DEFAULT_ADMIN_PASSWORD = "146524";

// Start with an empty list of user accounts.
let users = [];

// Load saved user records from this browser when available.
const savedUsersText = localStorage.getItem(USERS_STORAGE_KEY);

// Restore each user's separate data after a page reload.
if (savedUsersText !== null) {
    // Turn the saved text into the user array.
    users = JSON.parse(savedUsersText);
}

// Read the saved administrator password.
const savedAdminPassword = localStorage.getItem(ADMIN_STORAGE_KEY);

// Use the default admin password on the first run.
let adminPassword = DEFAULT_ADMIN_PASSWORD;

// Restore the admin password if it was changed before.
if (savedAdminPassword !== null) {
    // Use the password saved by the administrator.
    adminPassword = savedAdminPassword;
}
// Store the default password when it has not been changed.
else {
    localStorage.setItem(ADMIN_STORAGE_KEY, adminPassword);
}

// Keep the user's email that is currently signed in.
let activeUserEmail = "";

// Track whether the administrator has unlocked the admin panel.
let adminLoggedIn = false;

// Find the sign-up and login tab buttons.
const showSignupButton = document.getElementById("show-signup");
const showLoginButton = document.getElementById("show-login");

// Find the sign-up and login forms.
const signupForm = document.getElementById("signup-form");
const loginForm = document.getElementById("login-form");

// Find the sign-up input fields and feedback message.
const signupNameInput = document.getElementById("signup-name");
const signupEmailInput = document.getElementById("signup-email");
const signupPasswordInput = document.getElementById("signup-password");
const signupMessage = document.getElementById("signup-message");

// Find the login input fields and feedback message.
const loginEmailInput = document.getElementById("login-email");
const loginPasswordInput = document.getElementById("login-password");
const loginMessage = document.getElementById("login-message");

// Find the main application views.
const authView = document.getElementById("auth-view");
const userView = document.getElementById("user-view");
const adminView = document.getElementById("admin-view");

// Find the user dashboard title and logout button.
const currentUserNameText = document.getElementById("current-user-name");
const userLogoutButton = document.getElementById("user-logout");

// Find the add-habit form, fields, and feedback message.
const habitForm = document.getElementById("habit-form");
const habitNameInput = document.getElementById("habit-input");
const targetMinutesInput = document.getElementById("target-minutes-input");
const habitMessage = document.getElementById("habit-message");

// Find the active user's status message and habit count.
const userStatusMessage = document.getElementById("user-status-message");
const userHabitCountText = document.getElementById("user-habit-count");

// Find the user's score values.
const scoreCompletedText = document.getElementById("score-completed");
const scoreMinutesText = document.getElementById("score-minutes");
const scorePercentageText = document.getElementById("score-percentage");

// Find the user's streak badges.
const currentStreakText = document.getElementById("current-streak");
const bestStreakText = document.getElementById("best-streak");
const daysLoggedText = document.getElementById("days-logged");

// Find the user's Smart Summary values.
const pendingCountText = document.getElementById("smart-pending-count");
const priorityHabitText = document.getElementById("smart-priority");
const pendingBreakdownText = document.getElementById("smart-breakdown");

// Find the timer, finish-day button, and habit list.
const resetCountdownText = document.getElementById("reset-countdown");
const finishDayButton = document.getElementById("finish-day-button");
const habitList = document.getElementById("habit-list");

// Find the admin login form and its password field.
const adminLoginForm = document.getElementById("admin-login-form");
const adminPasswordInput = document.getElementById("admin-password-input");
const adminLoginMessage = document.getElementById("admin-login-message");

// Find the admin account list and logout button.
const adminPanel = document.getElementById("admin-panel");
const adminUserList = document.getElementById("admin-user-list");
const adminLogoutButton = document.getElementById("admin-logout");

// Find the admin password update form and feedback message.
const adminPasswordForm = document.getElementById("admin-password-form");
const newAdminPasswordInput = document.getElementById("new-admin-password");
const adminPasswordMessage = document.getElementById("admin-password-message");

// Return the account position for an email or -1 when it is missing.
function findUserIndex(email) {
    // Check each registered email.
    for (let i = 0; i < users.length; i++) {
        // Return the position of the matching email.
        if (users[i].email === email) {
            // Give the caller this user's array index.
            return i;
        }
    }

    // Report that no account has this email.
    return -1;
}

// Return the signed-in user's record or null when no user is active.
function getActiveUser() {
    // Find the account matching the signed-in email.
    const userIndex = findUserIndex(activeUserEmail);

    // Return the matching private account record when found.
    if (userIndex >= 0) {
        // Give the caller the active user's own data.
        return users[userIndex];
    }

    // Report that no user is signed in.
    return null;
}

// Save the full set of separate user records in this browser.
function saveUsers() {
    // Store account records as browser text.
    localStorage.setItem(USERS_STORAGE_KEY, JSON.stringify(users));
}

// Return an error message when a new habit name is invalid.
function getHabitNameError(name) {
    // Reject empty and overlong habit names.
    if (name.length === 0 || name.length >= 40) {
        // Explain the name length limit.
        return "Use a habit name with fewer than 40 characters.";
    }

    // Track whether the name contains at least one letter.
    let hasLetter = false;

    // Track whether the name contains something besides whitespace.
    let hasNonSpace = false;

    // Check every character in the name.
    for (let i = 0; i < name.length; i++) {
        // Store the character being checked.
        const character = name[i];

        // Mark the name when an English letter is found.
        if ((character >= "a" && character <= "z") || (character >= "A" && character <= "Z")) {
            // Remember that the name contains a letter.
            hasLetter = true;
        }

        // Mark the name when this character is not whitespace.
        if (!(/\s/.test(character))) {
            // Remember that the name is not spaces only.
            hasNonSpace = true;
        }
    }

    // Reject names made only of whitespace.
    if (!hasNonSpace) {
        // Explain that the name cannot be blank.
        return "Habit name cannot be empty or spaces only.";
    }

    // Reject names that contain no letters.
    if (!hasLetter) {
        // Explain that at least one letter is required.
        return "Habit name must include at least one letter.";
    }

    // Return an empty message when all checks pass.
    return "";
}

// Count completed habits for one account.
function countCompletedHabits(user) {
    // Start the completed count at zero.
    let completed = 0;

    // Check only this user's completion array.
    for (let i = 0; i < user.habitDone.length; i++) {
        // Count a habit when its flag is true.
        if (user.habitDone[i]) {
            // Increase the completed count.
            completed++;
        }
    }

    // Return this user's completed count.
    return completed;
}

// Count pending habits for one account.
function countPendingHabits(user) {
    // Start the pending count at zero.
    let pending = 0;

    // Check only this user's completion array.
    for (let i = 0; i < user.habitDone.length; i++) {
        // Count a habit when its flag is false.
        if (!user.habitDone[i]) {
            // Increase the pending count.
            pending++;
        }
    }

    // Return this user's pending count.
    return pending;
}

// Add target minutes only for one user's completed habits.
function calculateCompletedMinutes(user) {
    // Start the completed minute total at zero.
    let completedMinutes = 0;

    // Check each target and its matching completion flag.
    for (let i = 0; i < user.habitDone.length; i++) {
        // Include minutes only when the habit is done.
        if (user.habitDone[i]) {
            // Add this completed habit's target minutes.
            completedMinutes = completedMinutes + user.habitMinutes[i];
        }
    }

    // Return this user's completed minutes.
    return completedMinutes;
}

// Add all target minutes for one account.
function calculateTotalMinutes(user) {
    // Start the full target at zero.
    let totalMinutes = 0;

    // Visit every target in this user's list.
    for (let i = 0; i < user.habitMinutes.length; i++) {
        // Add this habit's target minutes.
        totalMinutes = totalMinutes + user.habitMinutes[i];
    }

    // Return this user's total target.
    return totalMinutes;
}

// Calculate the percentage of one user's habits marked done.
function calculateProductivityScore(user) {
    // Read the number of habits for this user.
    const count = user.habitDone.length;

    // Return zero when the account has no habits.
    if (count === 0) {
        // Avoid dividing by zero.
        return 0;
    }

    // Count the completed habits for the percentage.
    const completed = countCompletedHabits(user);

    // Return the percentage using JavaScript decimal division.
    return completed / count * 100;
}

// Find the pending habit with the highest target minutes.
function findHighestPriorityPendingHabit(user) {
    // Start with no pending habit selected.
    let highestIndex = -1;

    // Check every habit in this user's list.
    for (let i = 0; i < user.habitNames.length; i++) {
        // Consider only habits that are pending.
        if (!user.habitDone[i]) {
            // Keep the first pending habit or a higher-minute habit.
            if (highestIndex === -1 || user.habitMinutes[i] > user.habitMinutes[highestIndex]) {
                // Save this habit's index.
                highestIndex = i;
            }
        }
    }

    // Return -1 when all of this user's habits are done.
    return highestIndex;
}

// Build one user's pending list by recursively visiting habit indexes.
function buildPendingBreakdown(user, index) {
    // Stop safely at the end of the array, including when it is empty.
    if (index >= user.habitNames.length) {
        // Return an empty ending from the recursive base case.
        return "";
    }

    // Build the text for the remaining habit indexes.
    const remainingText = buildPendingBreakdown(user, index + 1);

    // Skip the current habit when it is already done.
    if (user.habitDone[index]) {
        // Keep only pending names from the remaining indexes.
        return remainingText;
    }

    // Make a text line for the current pending habit.
    const currentLine = user.habitNames[index] + " - " + user.habitMinutes[index] + " minutes";

    // Return this line when no later pending habit exists.
    if (remainingText === "") {
        // End the recursive result with this pending habit.
        return currentLine;
    }

    // Join this habit to the remaining pending list.
    return currentLine + "\n" + remainingText;
}

// Return a local calendar date as a simple comparison key.
function getDateKey(date) {
    // Build the key from the browser's local date.
    return date.getFullYear() + "-" + (date.getMonth() + 1) + "-" + date.getDate();
}

// Return local midnight after a chosen number of calendar days.
function getMidnightAfterDays(startDate, daysToAdd) {
    // Copy the date so the caller's value stays unchanged.
    const resetDate = new Date(startDate.getTime());

    // Move the copy to the beginning of its local day.
    resetDate.setHours(0, 0, 0, 0);

    // Add calendar days without assuming every day has 24 hours.
    resetDate.setDate(resetDate.getDate() + daysToAdd);

    // Return the reset time in milliseconds.
    return resetDate.getTime();
}

// Update one user's day count and streak values.
function recordFinishedDay(user, score, dateKey) {
    // Count the finished day for this user.
    user.totalDaysLogged++;

    // Keep the streak when the score meets the target.
    if (score >= STREAK_TARGET) {
        // Increase this user's current streak.
        user.currentStreak++;

        // Save a best streak only when the current one is greater.
        if (user.currentStreak > user.bestStreak) {
            // Store this user's new best streak.
            user.bestStreak = user.currentStreak;
        }
    }
    // Reset this user's streak when the score is below target.
    else {
        // Clear only the current streak.
        user.currentStreak = 0;
    }

    // Remember the local date that was finished.
    user.lastFinishedDate = dateKey;
}

// Clear all habits for one user's next day.
function resetDailyHabits(user) {
    // Visit every habit in this user's list.
    for (let i = 0; i < user.habitDone.length; i++) {
        // Mark the habit pending for the next day.
        user.habitDone[i] = false;
    }
}

// Finish a user day and schedule its first delayed automatic reset.
function finishActiveUserDay() {
    // Find the currently signed-in account.
    const user = getActiveUser();

    // Stop when there is no active account.
    if (user === null) {
        // Do not change any account data.
        return;
    }

    // Read today's local date and its comparison key.
    const now = new Date();
    const todayKey = getDateKey(now);

    // Prevent finishing the same calendar date twice.
    if (user.lastFinishedDate === todayKey) {
        // Explain why this second finish was ignored.
        user.statusMessage = "This calendar day has already been finished.";
        renderUserView();
        return;
    }

    // Save today's score before clearing the habit flags.
    const score = calculateProductivityScore(user);

    // Update this user's streak, day count, and last finished date.
    recordFinishedDay(user, score, todayKey);

    // Clear this user's habits so the next day can begin.
    resetDailyHabits(user);

    // Skip tonight and schedule the first automatic reset for midnight after tomorrow.
    user.nextResetTime = getMidnightAfterDays(now, 2);

    // Tell the user the score and reset schedule.
    user.statusMessage = "Day " + user.totalDaysLogged + " finished at " + score.toFixed(1)
        + "%. Habits are ready for the next day; automatic reset is after tomorrow, not tonight.";

    // Save this user's updated data.
    saveUsers();

    // Refresh the user's summary and timer.
    renderUserView();
}

// Apply an automatic midnight reset to the user's previous calendar day.
function applyAutomaticReset(user) {
    // Keep the scheduled local midnight being processed.
    const resetDate = new Date(user.nextResetTime);

    // Set the finished date to the day just before this reset.
    const finishedDate = new Date(resetDate.getTime());
    finishedDate.setDate(finishedDate.getDate() - 1);

    // Calculate the score for this user's day before clearing it.
    const score = calculateProductivityScore(user);

    // Record the user's previous day and streak result.
    recordFinishedDay(user, score, getDateKey(finishedDate));

    // Clear only this user's habit completion states.
    resetDailyHabits(user);

    // Schedule later daily resets at the next local midnight.
    user.nextResetTime = getMidnightAfterDays(resetDate, 1);

    // Explain the automatic reset and the score that was checked.
    user.statusMessage = "Automatic midnight reset completed. The previous day's score was "
        + score.toFixed(1) + "%.";

    // Save the reset account data.
    saveUsers();
}

// Process missed resets and update this user's countdown.
function updateResetTimer() {
    // Find the account that owns the visible timer.
    const user = getActiveUser();

    // Stop when no user is signed in.
    if (user === null) {
        // Leave the login and admin views unchanged.
        return;
    }

    // Track whether a scheduled reset was applied.
    let resetOccurred = false;

    // Apply each saved reset that passed while the page was closed.
    while (user.nextResetTime > 0 && user.nextResetTime <= Date.now()) {
        // Process this user's scheduled midnight reset.
        applyAutomaticReset(user);
        resetOccurred = true;
    }

    // Redraw the user dashboard after a reset changed its data.
    if (resetOccurred && window.location.hash !== "#/admin") {
        // Show the updated scores, habits, and status.
        renderUserView();
    }

    // Show instructions when this user has no reset scheduled.
    if (user.nextResetTime === 0) {
        // Explain how the first automatic reset is scheduled.
        document.getElementById("reset-countdown").textContent =
            "Finish Day to schedule the automatic reset after tomorrow, not tonight.";
        return;
    }

    // Count the seconds remaining until the reset.
    const secondsRemaining = Math.ceil((user.nextResetTime - Date.now()) / 1000);

    // Calculate the remaining whole days.
    const days = Math.floor(secondsRemaining / 86400);

    // Calculate the remaining hours after whole days.
    const hours = Math.floor((secondsRemaining % 86400) / 3600);

    // Calculate the remaining minutes after whole hours.
    const minutes = Math.floor((secondsRemaining % 3600) / 60);

    // Calculate the remaining seconds after whole minutes.
    const seconds = secondsRemaining % 60;

    // Display the countdown to this user's reset time.
    document.getElementById("reset-countdown").textContent = "Automatic reset in "
        + days + "d " + hours + "h " + minutes + "m " + seconds + "s (12:00 AM local time).";
}

// Remove a user's habit and shift its matching parallel arrays.
function deleteHabit(user, habitIndex) {
    // Shift each later habit into the removed position.
    for (let i = habitIndex; i < user.habitNames.length - 1; i++) {
        // Shift the habit name.
        user.habitNames[i] = user.habitNames[i + 1];

        // Shift the target minutes.
        user.habitMinutes[i] = user.habitMinutes[i + 1];

        // Shift the completion state.
        user.habitDone[i] = user.habitDone[i + 1];
    }

    // Remove the duplicate last entries after shifting.
    user.habitNames.pop();
    user.habitMinutes.pop();
    user.habitDone.pop();

    // Confirm and save the deletion.
    user.statusMessage = "Habit deleted.";
    saveUsers();
    renderUserView();
}

// Draw habit rows using only the signed-in user's arrays.
function renderHabitList(user) {
    // Remove the previous rows before drawing this user's habits.
    habitList.textContent = "";

    // Visit each habit belonging to this user.
    for (let i = 0; i < user.habitNames.length; i++) {
        // Create this habit's list row.
        const habitItem = document.createElement("li");

        // Apply the shared row style.
        habitItem.className = "habit-item";

        // Highlight this row when the habit is done.
        if (user.habitDone[i]) {
            // Apply the completed habit style.
            habitItem.className = "habit-item completed";
        }

        // Create the habit name element.
        const nameText = document.createElement("span");

        // Apply the habit name style.
        nameText.className = "habit-name";

        // Show the user's number and habit name.
        nameText.textContent = (i + 1) + ". " + user.habitNames[i];

        // Create the target minute element.
        const detailText = document.createElement("span");

        // Apply the target detail style.
        detailText.className = "habit-detail";

        // Show this habit's target.
        detailText.textContent = user.habitMinutes[i] + " min target";

        // Create the completion status badge.
        const statusBadge = document.createElement("span");

        // Apply the standard pending badge style.
        statusBadge.className = "habit-status";

        // Choose the badge state from this user's habit flag.
        if (user.habitDone[i]) {
            // Show a green done badge.
            statusBadge.className = "habit-status completed-status";
            statusBadge.textContent = "Done";
        }
        // Show pending when this habit is not done.
        else {
            statusBadge.textContent = "Pending";
        }

        // Create the button that toggles this habit.
        const toggleButton = document.createElement("button");

        // Apply the compact toggle button style.
        toggleButton.className = "toggle-button";

        // Keep this button from submitting the add form.
        toggleButton.type = "button";

        // Set the action label for this habit.
        if (user.habitDone[i]) {
            // Let the user move a done habit back to pending.
            toggleButton.textContent = "Mark pending";
        }
        // Let the user mark a pending habit done.
        else {
            toggleButton.textContent = "Mark done";
        }

        // Create the button that deletes this habit.
        const deleteButton = document.createElement("button");

        // Apply the warning style to the delete button.
        deleteButton.className = "delete-button";

        // Keep the delete button from submitting the add form.
        deleteButton.type = "button";

        // Label the habit removal action.
        deleteButton.textContent = "Delete";

        // Connect the toggle button to this user's habit index.
        toggleButton.addEventListener("click", function () {
            // Flip this user's selected completion flag.
            user.habitDone[i] = !user.habitDone[i];

            // Confirm the status change.
            user.statusMessage = "Habit status updated.";

            // Save and redraw only this user's data.
            saveUsers();
            renderUserView();
        });

        // Connect the delete button to this user's habit index.
        deleteButton.addEventListener("click", function () {
            // Remove this user's selected habit.
            deleteHabit(user, i);
        });

        // Add the name, target, status, and actions to the row.
        habitItem.appendChild(nameText);
        habitItem.appendChild(detailText);
        habitItem.appendChild(statusBadge);
        habitItem.appendChild(toggleButton);
        habitItem.appendChild(deleteButton);

        // Add this user's completed row to the page.
        habitList.appendChild(habitItem);
    }
}

// Render the signed-in user's own dashboard and data.
function renderUserView() {
    // Find the account signed in to the tracker.
    const user = getActiveUser();

    // Leave the view unchanged if the session no longer exists.
    if (user === null) {
        // Show the normal route, which returns to sign-up or login.
        renderPage();
        return;
    }

    // Apply a midnight reset that passed while this user was away.
    while (user.nextResetTime > 0 && user.nextResetTime <= Date.now()) {
        // Reset and record the expired day.
        applyAutomaticReset(user);
    }

    // Show only the user dashboard.
    authView.hidden = true;
    userView.hidden = false;
    adminView.hidden = true;

    // Show this account's name and habit count.
    currentUserNameText.textContent = user.name;
    userHabitCountText.textContent = user.habitNames.length + " of " + MAX_HABITS + " habits";

    // Show this user's current status message.
    userStatusMessage.textContent = user.statusMessage || "";

    // Calculate this user's score and minute totals.
    const completedCount = countCompletedHabits(user);
    const completedMinutes = calculateCompletedMinutes(user);
    const totalMinutes = calculateTotalMinutes(user);
    const productivityScore = calculateProductivityScore(user);

    // Display the completed habit count.
    scoreCompletedText.textContent = completedCount + " / " + user.habitNames.length;

    // Display completed focus minutes against the user's target.
    scoreMinutesText.textContent = completedMinutes + " / " + totalMinutes;

    // Display the productivity percentage.
    scorePercentageText.textContent = productivityScore.toFixed(1) + "%";

    // Display the user's current and best streaks.
    currentStreakText.textContent = user.currentStreak + " days";
    bestStreakText.textContent = user.bestStreak + " days";

    // Display the user's logged day count.
    daysLoggedText.textContent = user.totalDaysLogged;

    // Calculate this user's pending habit count.
    pendingCountText.textContent = countPendingHabits(user);

    // Find this user's highest-priority pending habit.
    const highestIndex = findHighestPriorityPendingHabit(user);

    // Explain when this user has no pending habits.
    if (highestIndex === -1) {
        // Show that there is no pending priority habit.
        priorityHabitText.textContent = "None";
    }
    // Show the highest-minute pending habit when one exists.
    else {
        priorityHabitText.textContent = user.habitNames[highestIndex]
            + " (" + user.habitMinutes[highestIndex] + " min)";
    }

    // Build this user's recursive pending list.
    const pendingBreakdown = buildPendingBreakdown(user, 0);

    // Show a clear message when the recursive list is empty.
    if (pendingBreakdown === "") {
        // Explain that no habits are pending.
        pendingBreakdownText.textContent = "No pending habits.";
    }
    // Show the recursive list when pending habits exist.
    else {
        pendingBreakdownText.textContent = pendingBreakdown;
    }

    // Draw only the active user's habit rows.
    renderHabitList(user);

    // Refresh the countdown for this user's saved reset time.
    updateResetTimer();
}

// Render the protected admin login or account management panel.
function renderAdminView() {
    // Show the admin route and hide user and authentication views.
    authView.hidden = true;
    userView.hidden = true;
    adminView.hidden = false;

    // Show the password form until the admin is authenticated.
    adminLoginForm.hidden = adminLoggedIn;
    adminPanel.hidden = !adminLoggedIn;

    // Draw user account controls only after admin login.
    if (adminLoggedIn) {
        // Remove the previous account rows.
        adminUserList.textContent = "";

        // Explain the empty account list.
        if (users.length === 0) {
            // Add a simple empty-list row.
            const emptyItem = document.createElement("li");
            emptyItem.textContent = "No user accounts.";
            adminUserList.appendChild(emptyItem);
        }

        // Visit each user account without showing its habit details.
        for (let i = 0; i < users.length; i++) {
            // Create one account row.
            const userItem = document.createElement("li");

            // Apply the account row style.
            userItem.className = "admin-user-item";

            // Create a text label with public account details only.
            const accountText = document.createElement("span");
            accountText.textContent = users[i].name + " - " + users[i].email;

            // Create a button for deleting this account.
            const deleteButton = document.createElement("button");

            // Apply the warning style to the account delete button.
            deleteButton.className = "delete-button";

            // Keep the delete button from submitting any form.
            deleteButton.type = "button";

            // Label the account removal action.
            deleteButton.textContent = "Delete Account";

            // Connect the button to the selected account.
            deleteButton.addEventListener("click", function () {
                // Remember the selected email before removing its account.
                const deletedEmail = users[i].email;

                // Remove only this account from the account list.
                users.splice(i, 1);

                // Log out the account if it was the active user.
                if (activeUserEmail === deletedEmail) {
                    // Clear the active user's email.
                    activeUserEmail = "";
                }

                // Save the account deletion.
                saveUsers();

                // Refresh the admin account list.
                renderAdminView();
            });

            // Add the account label and button to the row.
            userItem.appendChild(accountText);
            userItem.appendChild(deleteButton);

            // Add the account row to the admin list.
            adminUserList.appendChild(userItem);
        }
    }
}

// Render sign-up, login, user, or admin according to the current route.
function renderPage() {
    // Show the admin view only on the protected hash route.
    if (window.location.hash === "#/admin") {
        // Display the admin password form or unlocked panel.
        renderAdminView();
        return;
    }

    // Return to the user dashboard when an account is active.
    if (activeUserEmail !== "") {
        // Show only the active user's account data.
        renderUserView();
        return;
    }

    // Show the account forms when no user is signed in.
    authView.hidden = false;
    userView.hidden = true;
    adminView.hidden = true;
}

// Show the chosen sign-up or login form.
function showAuthTab(tabName) {
    // Show sign-up and hide login for the sign-up tab.
    if (tabName === "signup") {
        // Select the sign-up form and tab.
        signupForm.hidden = false;
        loginForm.hidden = true;
        showSignupButton.className = "tab-button selected-tab";
        showLoginButton.className = "tab-button";
    }
    // Show login and hide sign-up for the login tab.
    else {
        // Select the login form and tab.
        signupForm.hidden = true;
        loginForm.hidden = false;
        showSignupButton.className = "tab-button";
        showLoginButton.className = "tab-button selected-tab";
    }
}

// Create a new account after checking the required sign-up fields.
signupForm.addEventListener("submit", function (event) {
    // Stop the browser from reloading the page.
    event.preventDefault();

    // Read the new name, email, and password.
    const name = signupNameInput.value.trim();
    const email = signupEmailInput.value.trim().toLowerCase();
    const password = signupPasswordInput.value;

    // Reject a blank name.
    if (name.length === 0) {
        // Ask for the missing name.
        signupMessage.textContent = "Enter your name.";
        return;
    }

    // Require a non-empty email containing an at sign.
    if (email.length === 0 || email.indexOf("@") === -1) {
        // Explain the basic email rule.
        signupMessage.textContent = "Enter an email address containing @.";
        return;
    }

    // Reject an empty or spaces-only password.
    if (password.trim().length === 0) {
        // Ask for a non-empty password.
        signupMessage.textContent = "Enter a password.";
        return;
    }

    // Reject duplicate emails without changing letter case.
    if (findUserIndex(email) >= 0) {
        // Explain that each email can be used once.
        signupMessage.textContent = "An account already uses that email.";
        return;
    }

    // Create a separate set of habits and counters for the new user.
    const newUser = {
        name: name,
        email: email,
        password: password,
        habitNames: ["Read for class", "Review lecture notes", "Practice a subject", "Take a study break", "Plan tomorrow"],
        habitMinutes: [30, 20, 45, 10, 5],
        habitDone: [false, false, false, false, false],
        currentStreak: 0,
        bestStreak: 0,
        totalDaysLogged: 0,
        lastFinishedDate: "",
        nextResetTime: 0,
        statusMessage: "Welcome! Your habits are private to this account."
    };

    // Add the new user record to the account list.
    users.push(newUser);

    // Sign in the new account.
    activeUserEmail = email;

    // Save the account and display its own dashboard.
    saveUsers();
    renderPage();
});

// Sign in only when the email and password match one account.
loginForm.addEventListener("submit", function (event) {
    // Stop the browser from reloading the page.
    event.preventDefault();

    // Read and normalize the login email.
    const email = loginEmailInput.value.trim().toLowerCase();

    // Read the login password exactly as entered.
    const password = loginPasswordInput.value;

    // Find the account that owns this email.
    const userIndex = findUserIndex(email);

    // Accept the login only when the password also matches.
    if (userIndex >= 0 && users[userIndex].password === password) {
        // Set the active account email.
        activeUserEmail = users[userIndex].email;

        // Clear an old login error.
        loginMessage.textContent = "";

        // Show only this user's dashboard.
        renderPage();
    }
    // Reject credentials that do not match one account.
    else {
        // Keep the user logged out.
        loginMessage.textContent = "Email or password is incorrect.";
    }
});

// Add a habit with its own positive whole-number target.
habitForm.addEventListener("submit", function (event) {
    // Stop the browser from reloading the page.
    event.preventDefault();

    // Find the active account before changing any data.
    const user = getActiveUser();

    // Stop if the session ended before this form was submitted.
    if (user === null) {
        // Return to the login view.
        renderPage();
        return;
    }

    // Read the entered name and target minutes.
    const name = habitNameInput.value.trim();
    const targetMinutes = Number(targetMinutesInput.value);

    // Check the habit name before adding it.
    const nameError = getHabitNameError(name);

    // Show the name error when validation fails.
    if (nameError !== "") {
        // Display the name validation message.
        habitMessage.textContent = nameError;
        return;
    }

    // Require a positive whole-number target.
    if (targetMinutes <= 0 || targetMinutes % 1 !== 0) {
        // Explain the target minutes rule.
        habitMessage.textContent = "Target minutes must be a positive whole number.";
        return;
    }

    // Stop safely when this user's habit list is full.
    if (user.habitNames.length >= MAX_HABITS) {
        // Tell the user the habit limit.
        habitMessage.textContent = "Your habit list is full.";
        return;
    }

    // Add matching values to this user's parallel arrays.
    user.habitNames.push(name);
    user.habitMinutes.push(targetMinutes);
    user.habitDone.push(false);

    // Confirm the saved habit and its target.
    user.statusMessage = "Habit added with a " + targetMinutes + "-minute target.";
    habitMessage.textContent = user.statusMessage;

    // Clear the form fields for another habit.
    habitNameInput.value = "";
    targetMinutesInput.value = "30";

    // Save and redraw only the active user's data.
    saveUsers();
    renderUserView();
});

// Finish the active user's day from the dashboard button.
finishDayButton.addEventListener("click", function () {
    // Score the user's day and schedule its automatic reset.
    finishActiveUserDay();
});

// Show the sign-up tab when its tab button is selected.
showSignupButton.addEventListener("click", function () {
    // Display the sign-up form.
    showAuthTab("signup");
});

// Show the login tab when its tab button is selected.
showLoginButton.addEventListener("click", function () {
    // Display the login form.
    showAuthTab("login");
});

// Log out the active user and show the account forms.
userLogoutButton.addEventListener("click", function () {
    // Clear the active account email.
    activeUserEmail = "";

    // Return to the normal sign-up and login route.
    window.location.hash = "";
    renderPage();
});

// Check the admin password before opening account controls.
adminLoginForm.addEventListener("submit", function (event) {
    // Stop the browser from reloading the page.
    event.preventDefault();

    // Compare the entered password with the current admin password.
    if (adminPasswordInput.value === adminPassword) {
        // Unlock the administrator panel.
        adminLoggedIn = true;

        // Clear any previous login error.
        adminLoginMessage.textContent = "";

        // Show account controls after successful authentication.
        renderAdminView();
    }
    // Keep the panel locked when the password is wrong.
    else {
        // Explain that the admin password did not match.
        adminLoginMessage.textContent = "Admin password is incorrect.";
    }
});

// Log out from the admin panel and return to the tracker.
adminLogoutButton.addEventListener("click", function () {
    // Lock the admin panel.
    adminLoggedIn = false;

    // Return from the admin hash route to the tracker.
    window.location.hash = "";
    renderPage();
});

// Change the admin password after checking the new value.
adminPasswordForm.addEventListener("submit", function (event) {
    // Stop the browser from reloading the page.
    event.preventDefault();

    // Read the proposed password without surrounding spaces.
    const newPassword = newAdminPasswordInput.value.trim();

    // Reject an empty or spaces-only password.
    if (newPassword.length === 0) {
        // Explain that the new password is required.
        adminPasswordMessage.textContent = "Enter a non-empty admin password.";
        return;
    }

    // Save the new password in memory and browser storage.
    adminPassword = newPassword;
    localStorage.setItem(ADMIN_STORAGE_KEY, adminPassword);

    // Clear the form and confirm the change.
    newAdminPasswordInput.value = "";
    adminPasswordMessage.textContent = "Admin password updated.";
});

// Render the correct page after a direct route or hash change.
window.addEventListener("hashchange", function () {
    // Check the route and session before displaying a view.
    renderPage();
});

// Show the sign-up, login, user, or admin view when the page loads.
renderPage();

// Update the active user's countdown once each second.
setInterval(updateResetTimer, 1000);
