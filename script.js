// Set the habit limit for each account.
const MAX_HABITS = 6;

// Set the score needed to keep a daily streak.
const STREAK_TARGET = 80;

// Set the number of weekdays shown in each chart.
const WEEK_DAYS = 7;

// Use separate browser storage keys for accounts and the admin password.
const USERS_STORAGE_KEY = "habitTrackerUsers";
const ADMIN_STORAGE_KEY = "habitTrackerAdminPassword";
const ACTIVE_USER_STORAGE_KEY = "habitTrackerActiveUser";

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

// Store the weekday names in chart order from Monday to Sunday.
const WEEKDAY_NAMES = ["Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"];

// Prepare saved user data for the current Monday-to-Sunday chart.
for (let i = 0; i < users.length; i++) {
    // Reset chart arrays when the saved week is old or incomplete.
    prepareWeeklyData(users[i], new Date());
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

// Restore the signed-in user for this browser tab after a refresh.
let activeUserEmail = sessionStorage.getItem(ACTIVE_USER_STORAGE_KEY) || "";

// Track whether the administrator has unlocked the admin panel.
let adminLoggedIn = false;

// Remember which dashboard section the user selected.
let currentSection = "home";

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
const homeSection = document.getElementById("home-section");
const statsSection = document.getElementById("stats-section");
const settingsSection = document.getElementById("settings-section");
const timeGreetingText = document.getElementById("time-greeting");

// Find the user dashboard title and logout button.
const currentUserNameText = document.getElementById("current-user-name");
const userLogoutButton = document.getElementById("user-logout");
const settingsAccountNameText = document.getElementById("settings-account-name");
const settingsAccountEmailText = document.getElementById("settings-account-email");
const profileCard = document.getElementById("profile-card");

// Find the desktop and mobile navigation buttons.
const navHomeButton = document.getElementById("nav-home");
const navStatsButton = document.getElementById("nav-stats");
const navSettingsButton = document.getElementById("nav-settings");
const mobileNavHomeButton = document.getElementById("mobile-nav-home");
const mobileNavStatsButton = document.getElementById("mobile-nav-stats");
const mobileNavSettingsButton = document.getElementById("mobile-nav-settings");
const mobileSettingsButton = document.getElementById("mobile-settings-button");

// Find the add-habit form, fields, and feedback message.
const habitForm = document.getElementById("habit-form");
const habitNameInput = document.getElementById("habit-input");
const targetMinutesInput = document.getElementById("target-minutes-input");
const habitMessage = document.getElementById("habit-message");
const addHabitButton = document.getElementById("add-habit-button");
const addHabitPanel = document.getElementById("add-habit-panel");

// Find the active user's status message and habit count.
const userStatusMessage = document.getElementById("user-status-message");
const userHabitCountText = document.getElementById("user-habit-count");
const todayHabitsSummaryText = document.getElementById("today-habits-summary");
const todayProgressSummaryText = document.getElementById("today-progress-summary");
const currentStreakSummaryText = document.getElementById("current-streak-summary");
const bestStreakSummaryText = document.getElementById("best-streak-summary");

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
const statsResetCountdownText = document.getElementById("stats-reset-countdown");
const finishDayButton = document.getElementById("finish-day-button");
const habitList = document.getElementById("habit-list");
const weeklyChart = document.getElementById("weekly-chart");
const homeChartMetricSelect = document.getElementById("home-chart-metric");
const homeChartDetailText = document.getElementById("home-chart-detail");
const statsChartViewSelect = document.getElementById("stats-chart-view");
const statsChartMetricSelect = document.getElementById("stats-chart-metric");
const statsChart = document.getElementById("stats-chart");
const statsChartDetailText = document.getElementById("stats-chart-detail");
const weeklyTotalMinutesText = document.getElementById("weekly-total-minutes");

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

// Return the Monday date key for the week containing a date.
function getWeekStartKey(date) {
    // Copy the date so the caller's value stays unchanged.
    const monday = new Date(date.getTime());

    // Remove the time before calculating the weekday.
    monday.setHours(0, 0, 0, 0);

    // Find how many days to move backward to Monday.
    const daysAfterMonday = (monday.getDay() + 6) % WEEK_DAYS;
    monday.setDate(monday.getDate() - daysAfterMonday);

    // Return the local Monday as a simple key.
    return getDateKey(monday);
}

// Return the position of a date from Monday zero to Sunday six.
function getWeekDayIndex(date) {
    // Convert Sunday-first dates to Monday-first chart positions.
    return (date.getDay() + 6) % WEEK_DAYS;
}

// Turn a saved local date key back into a date value.
function getDateFromKey(dateKey) {
    // Split the year, month, and day into simple pieces.
    const dateParts = dateKey.split("-");

    // Create the date in local time rather than UTC.
    return new Date(Number(dateParts[0]), Number(dateParts[1]) - 1, Number(dateParts[2]));
}

// Prepare seven progress and focus-minute values for one local week.
function prepareWeeklyData(user, date) {
    // Find the Monday key for this selected date.
    const weekStart = getWeekStartKey(date);

    // Reset both metric arrays for a new or incomplete week.
    if (user.weekStart !== weekStart
        || !Array.isArray(user.weeklyScores)
        || user.weeklyScores.length !== WEEK_DAYS) {
        // Keep this user's chart on the current week only.
        user.weekStart = weekStart;
        user.weeklyScores = [0, 0, 0, 0, 0, 0, 0];
        user.weeklyMinutes = [0, 0, 0, 0, 0, 0, 0];
    }

    // Create minute values for accounts saved before the chart update.
    if (!Array.isArray(user.weeklyMinutes) || user.weeklyMinutes.length !== WEEK_DAYS) {
        // Start old accounts with seven empty minute totals.
        user.weeklyMinutes = [0, 0, 0, 0, 0, 0, 0];
    }

    // Treat older accounts as having an unfinished current day.
    if (typeof user.dayFinished !== "boolean") {
        // Start the new finish marker as false.
        user.dayFinished = false;
    }
}

// Update today's chart bar from the active habit states.
function updateWeeklyToday(user) {
    // Find today's date and weekday chart position.
    const today = new Date();
    const todayKey = getDateKey(today);
    const dayIndex = getWeekDayIndex(today);

    // Prepare this user's arrays for the current calendar week.
    prepareWeeklyData(user, today);

    // Keep the finished score visible until the user changes a habit.
    if (user.dayFinished && user.lastFinishedDate === todayKey) {
        // Report that no new chart values need saving.
        return false;
    }

    // Calculate today's live progress and completed focus minutes.
    const score = calculateProductivityScore(user);
    const minutes = calculateCompletedMinutes(user);
    let changed = false;

    // Save today's percentage when it has changed.
    if (user.weeklyScores[dayIndex] !== score) {
        // Update only this weekday's score.
        user.weeklyScores[dayIndex] = score;
        changed = true;
    }

    // Save today's minutes when they have changed.
    if (user.weeklyMinutes[dayIndex] !== minutes) {
        // Update only this weekday's focus minutes.
        user.weeklyMinutes[dayIndex] = minutes;
        changed = true;
    }

    // Tell the caller whether the account needs saving.
    return changed;
}

// Return the greeting that matches the user's local time.
function getTimeGreeting(date) {
    // Read the current hour from the browser's local clock.
    const hour = date.getHours();

    // Use the night greeting before five or from nine onward.
    if (hour < 5 || hour >= 21) {
        // Return the overnight greeting.
        return "Good Night";
    }

    // Use the morning greeting before noon.
    if (hour < 12) {
        // Return the morning greeting.
        return "Good Morning";
    }

    // Use the afternoon greeting until five.
    if (hour < 17) {
        // Return the afternoon greeting.
        return "Good Afternoon";
    }

    // Use the evening greeting from five until nine.
    if (hour < 21) {
        // Return the evening greeting.
        return "Good Evening";
    }

    // Keep a final fallback for clear control flow.
    return "Good Night";
}

// Update the greeting using the current local time.
function updateTimeGreeting() {
    // Show the correct greeting for this moment.
    timeGreetingText.textContent = getTimeGreeting(new Date());
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

// Update one user's daily totals, weekly chart, and streak values.
function recordFinishedDay(user, score, minutes, dateKey) {
    // Count the finished day for this user.
    user.totalDaysLogged++;

    // Find the calendar weekday that was finished.
    const finishedDate = getDateFromKey(dateKey);
    const dayIndex = getWeekDayIndex(finishedDate);
    prepareWeeklyData(user, finishedDate);

    // Save the finished score and minutes in that weekday's chart slots.
    user.weeklyScores[dayIndex] = score;
    user.weeklyMinutes[dayIndex] = minutes;
    user.dayFinished = true;

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
    const completedMinutes = calculateCompletedMinutes(user);

    // Update this user's streak, day count, and last finished date.
    recordFinishedDay(user, score, completedMinutes, todayKey);

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
    const completedMinutes = calculateCompletedMinutes(user);

    // Record the user's previous day and streak result.
    recordFinishedDay(user, score, completedMinutes, getDateKey(finishedDate));

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
        const countdownMessage = "Finish Day to schedule the automatic reset after tomorrow, not tonight.";
        resetCountdownText.textContent = countdownMessage;
        statsResetCountdownText.textContent = countdownMessage;
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
    const countdownMessage = "Automatic reset in "
        + days + "d " + hours + "h " + minutes + "m " + seconds + "s (12:00 AM local time).";
    resetCountdownText.textContent = countdownMessage;
    statsResetCountdownText.textContent = countdownMessage;
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
    user.dayFinished = false;

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

        // Create a group for the name and target details.
        const detailGroup = document.createElement("div");
        detailGroup.className = "habit-detail";

        // Create the habit name element.
        const nameText = document.createElement("span");

        // Apply the habit name style.
        nameText.className = "habit-name";

        // Show the user's number and habit name.
        nameText.textContent = (i + 1) + ". " + user.habitNames[i];

        // Create the target minute element.
        const detailText = document.createElement("span");

        // Apply the target detail style.
        detailText.className = "habit-minutes";

        // Show this habit's target.
        detailText.textContent = user.habitMinutes[i] + " min target";

        // Create the completion status badge.
        const statusBadge = document.createElement("span");

        // Apply the standard pending badge style.
        statusBadge.className = "status-badge";

        // Choose the badge state from this user's habit flag.
        if (user.habitDone[i]) {
            // Show a green done badge.
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
            user.dayFinished = false;

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

        // Put the name and target in one aligned details column.
        detailGroup.appendChild(nameText);
        detailGroup.appendChild(detailText);
        habitItem.appendChild(detailGroup);
        habitItem.appendChild(statusBadge);
        habitItem.appendChild(toggleButton);
        habitItem.appendChild(deleteButton);

        // Add this user's completed row to the page.
        habitList.appendChild(habitItem);
    }
}

// Show the selected dashboard section and update both navigation bars.
function showSection(sectionName) {
    // Keep an unsigned admin route inside the settings section.
    if (getActiveUser() === null && sectionName !== "settings") {
        // Do not open private dashboard sections without an account.
        sectionName = "settings";
    }

    // Remember the selected section for the next dashboard refresh.
    currentSection = sectionName;
    homeSection.hidden = sectionName !== "home";
    statsSection.hidden = sectionName !== "stats";
    settingsSection.hidden = sectionName !== "settings";
    adminView.hidden = sectionName !== "settings";

    // Match the desktop and mobile active navigation styles.
    navHomeButton.className = sectionName === "home" ? "nav-button active" : "nav-button";
    navStatsButton.className = sectionName === "stats" ? "nav-button active" : "nav-button";
    navSettingsButton.className = sectionName === "settings" ? "nav-button active" : "nav-button";
    mobileNavHomeButton.className = sectionName === "home" ? "mobile-nav-button active" : "mobile-nav-button";
    mobileNavStatsButton.className = sectionName === "stats" ? "mobile-nav-button active" : "mobile-nav-button";
    mobileNavSettingsButton.className = sectionName === "settings" ? "mobile-nav-button active" : "mobile-nav-button";

    // Show account details only when a user is signed in.
    const user = getActiveUser();
    profileCard.hidden = user === null;

    // Refresh the settings identity and admin access controls.
    if (user !== null) {
        // Display only the active user's own account details.
        settingsAccountNameText.textContent = user.name;
        settingsAccountEmailText.textContent = user.email;
    }

    // Keep admin login and account controls inside Settings.
    adminLoginForm.hidden = adminLoggedIn;
    adminPanel.hidden = !adminLoggedIn;
}

// Add one clickable bar with a hover label to a chart.
function addChartBar(chart, label, value, maximum, metric, tooltip, isToday, showDetails) {
    // Create a column that holds the value, bar, and day label.
    const column = document.createElement("div");
    column.className = "chart-column";

    // Show the value above the bar in the selected measure.
    const valueLabel = document.createElement("span");
    valueLabel.className = "chart-value";
    valueLabel.textContent = metric === "minutes" ? Math.round(value) + "m" : Math.round(value) + "%";

    // Create a button so each bar can be inspected with a click.
    const barButton = document.createElement("button");
    barButton.className = "chart-bar-button";
    barButton.type = "button";
    barButton.title = tooltip;
    barButton.setAttribute("aria-label", tooltip);

    // Create the colored bar and scale it to the chart maximum.
    const barFill = document.createElement("span");
    barFill.className = isToday ? "chart-bar-fill latest-score" : "chart-bar-fill";
    const barHeight = maximum > 0 ? value / maximum * 100 : 0;
    barFill.style.height = barHeight + "%";
    barButton.appendChild(barFill);

    // Show the short day or habit label below the bar.
    const dayLabel = document.createElement("span");
    dayLabel.className = "chart-label";
    dayLabel.textContent = label;

    // Show this bar's complete information when it is selected.
    barButton.addEventListener("click", showDetails);

    // Add each chart part in reading order.
    column.appendChild(valueLabel);
    column.appendChild(barButton);
    column.appendChild(dayLabel);
    chart.appendChild(column);
}

// Draw a Monday-to-Sunday chart for progress or focus minutes.
function renderWeeklyChart(user) {
    // Remove the previous chart bars.
    weeklyChart.textContent = "";

    // Read the home chart's selected measure.
    const metric = homeChartMetricSelect.value;
    let maximum = metric === "minutes" ? calculateTotalMinutes(user) : 100;
    const weekStart = getDateFromKey(user.weekStart);

    // Scale minute bars to the largest value in this week's chart.
    if (metric === "minutes") {
        // Check every weekday for a value above today's target.
        for (let i = 0; i < WEEK_DAYS; i++) {
            // Keep historical values visible after target changes.
            if (user.weeklyMinutes[i] > maximum) {
                // Raise the scale to fit this day's minutes.
                maximum = user.weeklyMinutes[i];
            }
        }
        // Keep an empty minute chart safe from division by zero.
        if (maximum < 1) {
            // Use one minute as the smallest safe chart scale.
            maximum = 1;
        }
    }

    // Add one chart bar for each day from Monday through Sunday.
    for (let i = 0; i < WEEK_DAYS; i++) {
        // Calculate the calendar date for this bar.
        const chartDate = new Date(weekStart.getTime());
        chartDate.setDate(chartDate.getDate() + i);

        // Read this weekday's saved score and focus minutes.
        const score = user.weeklyScores[i];
        const minutes = user.weeklyMinutes[i];
        const value = metric === "minutes" ? minutes : score;
        const dateLabel = (chartDate.getMonth() + 1) + "/" + chartDate.getDate();
        const tooltip = WEEKDAY_NAMES[i] + " " + dateLabel + ": "
            + score.toFixed(1) + "% progress, " + minutes + " focus minutes";

        // Show both measures for the day selected by the user.
        const showDetails = function () {
            homeChartDetailText.textContent = WEEKDAY_NAMES[i] + " " + dateLabel + ": "
                + score.toFixed(1) + "% progress | " + minutes + " focus minutes.";
        };

        // Draw the weekday bar and connect its detail action.
        addChartBar(weeklyChart, WEEKDAY_NAMES[i], value, maximum, metric,
            tooltip, i === getWeekDayIndex(new Date()), showDetails);
    }

    // Show today's details when the chart first appears.
    const todayIndex = getWeekDayIndex(new Date());
    const todayDate = new Date();
    const todayLabel = (todayDate.getMonth() + 1) + "/" + todayDate.getDate();
    homeChartDetailText.textContent = WEEKDAY_NAMES[todayIndex] + " " + todayLabel + ": "
        + user.weeklyScores[todayIndex].toFixed(1) + "% progress | "
        + user.weeklyMinutes[todayIndex] + " focus minutes.";
}

// Draw the selected weekly chart or today's habit comparison.
function renderStatsChart(user) {
    // Clear the old bars before drawing the chosen view.
    statsChart.textContent = "";

    // Read the selected chart view and measure.
    const chartView = statsChartViewSelect.value;
    const metric = statsChartMetricSelect.value;

    // Draw the weekly view with one bar for each calendar day.
    if (chartView === "week") {
        // Use the standard seven weekday columns.
        statsChart.className = "stats-chart";
        const weekStart = getDateFromKey(user.weekStart);
        let maximum = metric === "minutes" ? calculateTotalMinutes(user) : 100;
        let weeklyMinutes = 0;

        // Scale minute bars to the largest value in this week's chart.
        if (metric === "minutes") {
            // Check every weekday for a value above today's target.
            for (let i = 0; i < WEEK_DAYS; i++) {
                // Keep historical values visible after target changes.
                if (user.weeklyMinutes[i] > maximum) {
                    // Raise the scale to fit this day's minutes.
                    maximum = user.weeklyMinutes[i];
                }
            }
            // Keep an empty minute chart safe from division by zero.
            if (maximum < 1) {
                // Use one minute as the smallest safe chart scale.
                maximum = 1;
            }
        }

        // Add Monday through Sunday to the chart.
        for (let i = 0; i < WEEK_DAYS; i++) {
            // Read this weekday's date and stored values.
            const chartDate = new Date(weekStart.getTime());
            chartDate.setDate(chartDate.getDate() + i);
            const score = user.weeklyScores[i];
            const minutes = user.weeklyMinutes[i];
            const value = metric === "minutes" ? minutes : score;
            const dateLabel = (chartDate.getMonth() + 1) + "/" + chartDate.getDate();
            const tooltip = WEEKDAY_NAMES[i] + " " + dateLabel + ": "
                + score.toFixed(1) + "% progress, " + minutes + " focus minutes";

            // Add this day's minutes to the weekly total.
            weeklyMinutes = weeklyMinutes + minutes;

            // Show both measures for the day selected by the user.
            const showDetails = function () {
                statsChartDetailText.textContent = WEEKDAY_NAMES[i] + " " + dateLabel + ": "
                    + score.toFixed(1) + "% progress | " + minutes + " focus minutes.";
            };

            // Draw the weekday bar and connect its detail action.
            addChartBar(statsChart, WEEKDAY_NAMES[i], value, maximum, metric,
                tooltip, i === getWeekDayIndex(new Date()), showDetails);
        }

        // Show the total number of focus minutes for this week.
        weeklyTotalMinutesText.textContent = "Focus minutes this week: " + weeklyMinutes;
        // Show today's details when the weekly chart first appears.
        const todayIndex = getWeekDayIndex(new Date());
        const todayDate = new Date();
        const todayLabel = (todayDate.getMonth() + 1) + "/" + todayDate.getDate();
        statsChartDetailText.textContent = WEEKDAY_NAMES[todayIndex] + " " + todayLabel + ": "
            + user.weeklyScores[todayIndex].toFixed(1) + "% progress | "
            + user.weeklyMinutes[todayIndex] + " focus minutes.";
        return;
    }

    // Use one bar for each habit in today's comparison.
    statsChart.className = "stats-chart daily-chart";
    weeklyTotalMinutesText.textContent = "Today's completed focus minutes: "
        + calculateCompletedMinutes(user);

    // Explain when there are no habits to compare.
    if (user.habitNames.length === 0) {
        // Show a friendly empty chart message.
        statsChart.textContent = "Add a habit to see today's comparison.";
        statsChartDetailText.textContent = "There are no habits to inspect.";
        return;
    }

    // Find a useful maximum for the selected habit measure.
    let maximum = metric === "minutes" ? calculateTotalMinutes(user) : 100;
    if (maximum < 1) {
        // Keep all bars safe when target minutes are zero.
        maximum = 1;
    }

    // Add a progress or focus-minute bar for every habit.
    for (let i = 0; i < user.habitNames.length; i++) {
        // Use one hundred percent for done habits and zero for pending.
        const score = user.habitDone[i] ? 100 : 0;
        const minutes = user.habitDone[i] ? user.habitMinutes[i] : 0;
        const value = metric === "minutes" ? minutes : score;
        const label = "H" + (i + 1);
        const tooltip = user.habitNames[i] + ": "
            + (user.habitDone[i] ? "Done" : "Pending") + ", "
            + user.habitMinutes[i] + " target minutes";

        // Show this habit's full information when its bar is selected.
        const showDetails = function () {
            statsChartDetailText.textContent = user.habitNames[i] + ": "
                + (user.habitDone[i] ? "Done" : "Pending") + " | "
                + minutes + " of " + user.habitMinutes[i] + " focus minutes.";
        };

        // Draw the habit bar and connect its detail action.
        addChartBar(statsChart, label, value, maximum, metric,
            tooltip, user.habitDone[i], showDetails);
    }

    // Show a helpful initial detail for the first habit.
    statsChartDetailText.textContent = "Select a habit bar to inspect its status and target.";
}

// Draw both charts and calculate a weekly focus-minute total.
function renderCharts(user) {
    // Refresh the Home chart for the selected measure.
    renderWeeklyChart(user);

    // Refresh the Stats chart for its selected view and measure.
    renderStatsChart(user);
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

    // Show only this user's dashboard.
    authView.hidden = true;
    userView.hidden = false;

    // Show this account's name and habit count.
    currentUserNameText.textContent = user.name;
    userHabitCountText.textContent = user.habitNames.length + " of " + MAX_HABITS + " habits";
    settingsAccountNameText.textContent = user.name;
    settingsAccountEmailText.textContent = user.email;

    // Show this user's current status message.
    userStatusMessage.textContent = user.statusMessage || "";

    // Keep today's chart values current before drawing the dashboard.
    const weeklyDataChanged = updateWeeklyToday(user);
    if (weeklyDataChanged) {
        // Save today's live chart values to this browser.
        saveUsers();
    }

    // Refresh the greeting whenever the dashboard is rendered.
    updateTimeGreeting();

    // Calculate this user's score and minute totals.
    const completedCount = countCompletedHabits(user);
    const completedMinutes = calculateCompletedMinutes(user);
    const totalMinutes = calculateTotalMinutes(user);
    const productivityScore = calculateProductivityScore(user);

    // Display the completed habit count.
    scoreCompletedText.textContent = completedCount + " / " + user.habitNames.length;
    todayHabitsSummaryText.textContent = completedCount + " / " + user.habitNames.length;

    // Display completed focus minutes against the user's target.
    scoreMinutesText.textContent = completedMinutes + " / " + totalMinutes;

    // Display the productivity percentage.
    scorePercentageText.textContent = productivityScore.toFixed(1) + "%";
    todayProgressSummaryText.textContent = productivityScore.toFixed(1) + "%";

    // Display the user's current and best streaks.
    currentStreakText.textContent = user.currentStreak + " days";
    bestStreakText.textContent = user.bestStreak + " days";
    currentStreakSummaryText.textContent = user.currentStreak;
    bestStreakSummaryText.textContent = user.bestStreak;

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
    renderCharts(user);

    // Keep the correct section selected after an action or refresh.
    showSection(currentSection);

    // Refresh the countdown for this user's saved reset time.
    updateResetTimer();
}

// Render the protected admin login or account management panel.
function renderAdminView() {
    // Keep admin tools in Settings instead of creating another page.
    authView.hidden = true;
    userView.hidden = false;
    // Find the signed-in account for the dashboard greeting.
    const activeUser = getActiveUser();
    currentUserNameText.textContent = activeUser === null ? "Guest" : activeUser.name;
    showSection("settings");
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
                    sessionStorage.removeItem(ACTIVE_USER_STORAGE_KEY);
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
    // Clear a tab session when its saved account no longer exists.
    if (activeUserEmail !== "" && findUserIndex(activeUserEmail) === -1) {
        // Remove the stale account email from this tab.
        activeUserEmail = "";
        sessionStorage.removeItem(ACTIVE_USER_STORAGE_KEY);
    }

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
        weeklyScores: [0, 0, 0, 0, 0, 0, 0],
        weekStart: getWeekStartKey(new Date()),
        weeklyMinutes: [0, 0, 0, 0, 0, 0, 0],
        dayFinished: false,
        lastFinishedDate: "",
        nextResetTime: 0,
        statusMessage: "Welcome! Your habits are private to this account."
    };

    // Add the new user record to the account list.
    users.push(newUser);

    // Sign in the new account.
    activeUserEmail = email;
    sessionStorage.setItem(ACTIVE_USER_STORAGE_KEY, email);

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
        sessionStorage.setItem(ACTIVE_USER_STORAGE_KEY, activeUserEmail);

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
    user.dayFinished = false;

    // Confirm the saved habit and its target.
    user.statusMessage = "Habit added with a " + targetMinutes + "-minute target.";
    habitMessage.textContent = user.statusMessage;
    addHabitPanel.hidden = true;

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

// Show or hide the new-habit form from its dashboard button.
addHabitButton.addEventListener("click", function () {
    // Toggle the form without changing the saved habit list.
    addHabitPanel.hidden = !addHabitPanel.hidden;
    // Focus the name field when the form becomes visible.
    if (!addHabitPanel.hidden) {
        habitNameInput.focus();
    }
});

// Open the Home section from desktop or mobile navigation.
navHomeButton.addEventListener("click", function () {
    // Display the private habit dashboard.
    showSection("home");
});
mobileNavHomeButton.addEventListener("click", function () {
    // Display the private habit dashboard.
    showSection("home");
});

// Open the Stats section from desktop or mobile navigation.
navStatsButton.addEventListener("click", function () {
    // Display the private analytics.
    showSection("stats");
});
mobileNavStatsButton.addEventListener("click", function () {
    // Display the private analytics.
    showSection("stats");
});

// Open Settings from the sidebar, bottom navigation, or mobile gear.
navSettingsButton.addEventListener("click", function () {
    // Display account and admin settings.
    showSection("settings");
});
mobileNavSettingsButton.addEventListener("click", function () {
    // Display account and admin settings.
    showSection("settings");
});
mobileSettingsButton.addEventListener("click", function () {
    // Display account and admin settings.
    showSection("settings");
});

// Redraw the Home chart when its measure changes.
homeChartMetricSelect.addEventListener("change", function () {
    // Find the account whose chart should be refreshed.
    const user = getActiveUser();
    if (user !== null) {
        // Show the newly selected Home chart measure.
        renderWeeklyChart(user);
    }
});

// Redraw the Stats chart when its view changes.
statsChartViewSelect.addEventListener("change", function () {
    // Find the account whose chart should be refreshed.
    const user = getActiveUser();
    if (user !== null) {
        // Show daily habits or the current week.
        renderStatsChart(user);
    }
});

// Redraw the Stats chart when its measure changes.
statsChartMetricSelect.addEventListener("change", function () {
    // Find the account whose chart should be refreshed.
    const user = getActiveUser();
    if (user !== null) {
        // Show progress or focus minutes.
        renderStatsChart(user);
    }
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
    sessionStorage.removeItem(ACTIVE_USER_STORAGE_KEY);
    adminLoggedIn = false;
    currentSection = "home";

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

// Refresh accounts changed by another tab in this same browser.
window.addEventListener("storage", function (event) {
    // Reload account data after another tab saves or clears it.
    if (event.key === USERS_STORAGE_KEY || event.key === null) {
        // Read the latest account list from this browser's storage.
        const latestUsersText = localStorage.getItem(USERS_STORAGE_KEY);
        users = latestUsersText === null ? [] : JSON.parse(latestUsersText);

        // Prepare each account's chart values for the current week.
        for (let i = 0; i < users.length; i++) {
            // Keep each account's Monday-to-Sunday arrays ready.
            prepareWeeklyData(users[i], new Date());
        }

        // End a tab session when its user was removed elsewhere.
        if (activeUserEmail !== "" && findUserIndex(activeUserEmail) === -1) {
            // Clear the removed account from this tab.
            activeUserEmail = "";
            sessionStorage.removeItem(ACTIVE_USER_STORAGE_KEY);
        }

        // Redraw the active user's own data.
        renderPage();
    }

    // Update the admin password after another tab changes it.
    if (event.key === ADMIN_STORAGE_KEY || event.key === null) {
        // Restore the saved value or the demo default.
        const latestAdminPassword = localStorage.getItem(ADMIN_STORAGE_KEY);
        adminPassword = latestAdminPassword === null ? DEFAULT_ADMIN_PASSWORD : latestAdminPassword;
        adminLoggedIn = false;
        renderPage();
    }
});

// Show the sign-up, login, user, or admin view when the page loads.
renderPage();

// Update the active user's countdown once each second.
setInterval(updateResetTimer, 1000);

// Update the time-based greeting once each minute.
setInterval(updateTimeGreeting, 60000);
