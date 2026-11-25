#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <iostream>
#include <string>
#include <cctype>
#include <cstdlib>
#include <stdio.h>
#include <cwchar>
using namespace std;

#ifndef ODS_HOTLIGHT
#define ODS_HOTLIGHT 0x0040
#endif

// const variable key: sets the maximum number of ledger entries.
const int MAX_TRANSACTIONS = 20;
// Parallel array key: stores transaction descriptions.
string entryTitles[MAX_TRANSACTIONS];
// Parallel array key: stores matching transaction amounts.
double entryAmounts[MAX_TRANSACTIONS];
// Parallel array key: stores matching transaction categories.
string entryCategories[MAX_TRANSACTIONS];
// Parallel array key: stores whether each entry is Income or Expense.
string entryTypes[MAX_TRANSACTIONS];
// Variable key: tracks active ledger entries.
int entryCount = 0;
// Variable key: stores the current monthly budget.
double monthlyBudget = 15000.0;
// Variable key: stores the selected application tab.
int activeTab = 0;
// Window handle key: identifies the main application window.
HWND mainWindow = NULL;
// Window handle key: identifies the title input control.
HWND titleEdit = NULL;
// Window handle key: identifies the amount input control.
HWND amountEdit = NULL;
// Window handle key: identifies the category selection control.
HWND categoryCombo = NULL;
// Window handle key: identifies the transaction type selector.
HWND typeCombo = NULL;
// Window handle key: identifies the budget input control.
HWND budgetEdit = NULL;
// Window handle array key: stores one custom Delete button for each transaction.
HWND deleteButtons[MAX_TRANSACTIONS];
// Font handle key: stores the 24-pixel bold page title font.
HFONT pageTitleFont = NULL;
// Font handle key: stores the 26-pixel bold KPI number font.
HFONT kpiNumberFont = NULL;
// Font handle key: stores the 15-pixel semibold section font.
HFONT sectionFont = NULL;
// Font handle key: stores the 14-pixel regular body font.
HFONT bodyFont = NULL;
// Font handle key: stores the 12-pixel bold caption font.
HFONT captionFont = NULL;

// bool isValidTitle - called by addEntry; validates title length and characters.
bool isValidTitle(const string& title);

// bool addEntry - called by the Add button; validates and stores a typed ledger entry.
bool addEntry(string titles[], double amounts[], string categories[], string types[], int& count,
              int maxSize, const string& title, double amount, const string& category,
              const string& entryType);

// bool deleteEntry - called by Delete buttons; removes one entry and shifts arrays left.
bool deleteEntry(string titles[], double amounts[], string categories[], string types[],
                 int& count, int indexToRemove);

// double calculateTotalByType - called by dashboard and Analytics; sums a transaction type.
double calculateTotalByType(const double amounts[], const string types[], int count, const string& targetType);

// inline double calculateBudgetUsedPercent - called by dashboard; calculates pool usage safely.
inline double calculateBudgetUsedPercent(double spent, double budget);

// inline double calculateAvailableBalance - called by dashboard; computes budget plus income minus expenses.
inline double calculateAvailableBalance(double baseBudget, double totalIncome, double totalExpense);

// double calculateCategoryTotal - called by Analytics; sums a category for a selected type.
double calculateCategoryTotal(const string categories[], const double amounts[], const string types[],
                              int count, const string& targetCategory, const string& targetType);

// int findHighestExpenseIndex - called by Analytics; finds the largest expense or returns -1.
int findHighestExpenseIndex(const double amounts[], const string types[], int count);

// recursive double sumEntriesByTypeRecursive - called by Analytics; recursively totals one entry type.
double sumEntriesByTypeRecursive(const double amounts[], const string types[], int count,
                                 const string& targetType, int index);

// string getBudgetAlertText - called by dashboard and Analytics; returns the current budget status.
string getBudgetAlertText(double spent, double budget);

// void printBudgetAlert - called by console analytics; prints the budget status.
void printBudgetAlert(double spent, double budget);

// void displayExpenses - called by console presentation; prints typed entries and balance totals.
void displayExpenses(const string titles[], const double amounts[], const string categories[],
                     const string types[], int count, double budget);

// void printAnalyticsSummary - called by console presentation; prints type totals and recursive checks.
void printAnalyticsSummary(const string titles[], const double amounts[], const string categories[],
                           const string types[], int count, double budget);

// wstring formatAmount - called by GDI drawing functions; formats a number to two decimals.
wstring formatAmount(double value);

// wstring toWide - called by list drawing; converts simple expense text for Win32 display.
wstring toWide(const string& text);

// void drawText - called by paint helpers; draws one line of Win32 GDI text.
void drawText(HDC deviceContext, const wchar_t* text, int x, int y, int size, COLORREF color, bool bold);

// void drawRightText - called by the ledger; right-aligns amount text.
void drawRightText(HDC deviceContext, const wchar_t* text, int right, int y, int size, COLORREF color, bool bold);

// void drawCard - called by paint helpers; draws a crisp white panel with a light border.
void drawCard(HDC deviceContext, int left, int top, int width, int height);

// void drawProgressBar - called by Analytics; draws one category total bar.
void drawProgressBar(HDC deviceContext, int left, int top, int width, double fraction, COLORREF color);

// void drawDashboard - called by paintWindow; renders balance cards, transaction form, and ledger rows.
void drawDashboard(HDC deviceContext, int width, int height);

// void drawAnalytics - called by paintWindow; renders category bars and recursive total checks.
void drawAnalytics(HDC deviceContext, int width, int height);

// void drawBudgetSettings - called by paintWindow; renders budget settings guidance.
void drawBudgetSettings(HDC deviceContext, int width);

// void paintWindow - called by WindowProc; paints the white banking interface.
void paintWindow(HWND window);

// void createControls - called during window creation; creates native Win32 inputs and buttons.
void createControls(HWND window);

// void createFonts - called during WM_CREATE; creates crisp Segoe UI font sizes.
void createFonts();

// void deleteFonts - called during WM_DESTROY; releases cached GDI fonts.
void deleteFonts();

// void setDpiAwareAtStartup - called by main; enables process DPI awareness using Windows.
void setDpiAwareAtStartup();

// void drawOwnerButton - called by WM_DRAWITEM; paints a rounded button in its state color.
void drawOwnerButton(DRAWITEMSTRUCT* item);

// void layoutControls - called on resize and tab changes; positions visible controls.
void layoutControls(HWND window);

// void setTab - called by navigation buttons; selects a section and refreshes controls.
void setTab(HWND window, int tab);

// void addEntryFromControls - called by Add Entry; validates and stores the selected transaction.
void addEntryFromControls(HWND window);

// void updateCategoryOptions - called by the type selector; loads valid categories for that type.
void updateCategoryOptions(HWND window);

// void updateBudgetFromControl - called by Apply Budget; updates a valid nonnegative budget.
void updateBudgetFromControl(HWND window);

// LRESULT CALLBACK WindowProc - called by Windows; handles window, control, and paint messages.
LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

// int main - called by the runtime; creates the Win32 window and runs its message loop.
int main()
{
    // Display setup step: keeps GDI coordinates and text crisp on Windows displays.
    setDpiAwareAtStartup();

    // Window class key: registers the application window procedure.
    HINSTANCE instance = GetModuleHandleW(NULL);
    WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEXW);
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = instance;
    windowClass.hCursor = LoadCursorW(NULL, MAKEINTRESOURCEW(32512));
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    windowClass.lpszClassName = L"ExpenseTrackerWindow";

    // Condition key: stops if Windows cannot register the window class.
    if (!RegisterClassExW(&windowClass))
    {
        cout << "Could not register the Expense Tracker window.\n";
        return 1;
    }

    // Window setup key: creates a normal resizable white application window.
    mainWindow = CreateWindowExW(
        0,
        windowClass.lpszClassName,
        L"Expense Tracker | Banking Ledger",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1200,
        1080,
        NULL,
        NULL,
        instance,
        NULL);

    // Condition key: reports a window creation failure.
    if (mainWindow == NULL)
    {
        cout << "Could not create the Expense Tracker window.\n";
        return 1;
    }

    ShowWindow(mainWindow, SW_SHOW);
    UpdateWindow(mainWindow);

    // Message loop key: processes Windows events until the application exits.
    MSG message;
    while (GetMessageW(&message, NULL, 0, 0) > 0)
    {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    return static_cast<int>(message.wParam);
}

// void setDpiAwareAtStartup - called by main; calls the Windows DPI awareness API when available.
void setDpiAwareAtStartup()
{
    HMODULE user32Module = GetModuleHandleW(L"user32.dll");
    typedef BOOL (WINAPI *DPI_AWARE_FUNCTION)(void);
    DPI_AWARE_FUNCTION setDpiAware =
        (DPI_AWARE_FUNCTION)GetProcAddress(user32Module, "SetProcessDPIAware");

    // Condition key: calls the DPI API only when this Windows system exports it.
    if (setDpiAware != NULL)
    {
        setDpiAware();
    }
}

// void createFonts - called during WM_CREATE; creates crisp Segoe UI presentation sizes.
void createFonts()
{
    pageTitleFont = CreateFontW(-24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    kpiNumberFont = CreateFontW(-26, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    sectionFont = CreateFontW(-15, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                              DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                              CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    bodyFont = CreateFontW(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                           DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                           CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    captionFont = CreateFontW(-12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                              DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                              CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
}

// void deleteFonts - called during WM_DESTROY; releases all cached Segoe UI fonts.
void deleteFonts()
{
    if (pageTitleFont != NULL) DeleteObject(pageTitleFont);
    if (kpiNumberFont != NULL) DeleteObject(kpiNumberFont);
    if (sectionFont != NULL) DeleteObject(sectionFont);
    if (bodyFont != NULL) DeleteObject(bodyFont);
    if (captionFont != NULL) DeleteObject(captionFont);
    pageTitleFont = NULL;
    kpiNumberFont = NULL;
    sectionFont = NULL;
    bodyFont = NULL;
    captionFont = NULL;
}

// void drawOwnerButton - called by WM_DRAWITEM; paints a rounded button with clear contrast.
void drawOwnerButton(DRAWITEMSTRUCT* item)
{
    int controlId = (int)item->CtlID;
    COLORREF fillColor = RGB(226, 232, 240);
    COLORREF textColor = RGB(51, 65, 85);
    COLORREF borderColor = RGB(203, 213, 225);

    // Navigation color key: marks the selected tab with deep navy and white text.
    if (controlId >= 100 && controlId <= 102)
    {
        if (controlId - 100 == activeTab)
        {
            fillColor = RGB(15, 23, 42);
            textColor = RGB(255, 255, 255);
            borderColor = RGB(71, 85, 105);
        }
    }
    // Primary button color key: distinguishes income, expense, and budget actions.
    else if (controlId == 110)
    {
        fillColor = SendMessageW(typeCombo, CB_GETCURSEL, 0, 0) == 1
                        ? RGB(5, 150, 105) : RGB(15, 23, 42);
        textColor = RGB(255, 255, 255);
    }
    else if (controlId == 111)
    {
        fillColor = RGB(37, 99, 235);
        textColor = RGB(255, 255, 255);
    }
    // Delete button color key: uses a rose fill and crimson label.
    else if (controlId >= 300 && controlId < 300 + MAX_TRANSACTIONS)
    {
        fillColor = RGB(254, 226, 226);
        textColor = RGB(220, 38, 38);
    }

    // Hover color key: brightens each button while the pointer is over it.
    if (item->itemState & ODS_HOTLIGHT)
    {
        if (controlId == 110)
        {
            fillColor = SendMessageW(typeCombo, CB_GETCURSEL, 0, 0) == 1
                            ? RGB(4, 120, 87) : RGB(30, 41, 59);
        }
        else if (controlId == 111)
        {
            fillColor = RGB(29, 78, 216);
        }
        else if (controlId >= 300 && controlId < 300 + MAX_TRANSACTIONS)
        {
            fillColor = RGB(254, 202, 202);
        }
        else if (controlId >= 100 && controlId <= 102 && controlId - 100 == activeTab)
        {
            fillColor = RGB(30, 41, 59);
        }
        else
        {
            fillColor = RGB(203, 213, 225);
        }
    }

    RECT buttonRect = item->rcItem;
    InflateRect(&buttonRect, -2, -2);
    HBRUSH buttonBrush = CreateSolidBrush(fillColor);
    HPEN buttonPen = CreatePen(PS_SOLID, 1, borderColor);
    HBRUSH oldBrush = (HBRUSH)SelectObject(item->hDC, buttonBrush);
    HPEN oldPen = (HPEN)SelectObject(item->hDC, buttonPen);
    RoundRect(item->hDC, buttonRect.left, buttonRect.top, buttonRect.right,
              buttonRect.bottom, 12, 12);
    SelectObject(item->hDC, oldBrush);
    SelectObject(item->hDC, oldPen);
    DeleteObject(buttonBrush);
    DeleteObject(buttonPen);

    wchar_t label[80] = L"";
    GetWindowTextW(item->hwndItem, label, 80);
    HFONT buttonFont = CreateFontW(-15, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                                   DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                   CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    HFONT oldFont = (HFONT)SelectObject(item->hDC, buttonFont);
    int oldMode = SetBkMode(item->hDC, TRANSPARENT);
    COLORREF oldColor = SetTextColor(item->hDC, textColor);
    DrawTextW(item->hDC, label, -1, &buttonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SetTextColor(item->hDC, oldColor);
    SetBkMode(item->hDC, oldMode);
    SelectObject(item->hDC, oldFont);
    DeleteObject(buttonFont);
}

// bool isValidTitle - called by addExpense; checks title length, letters, and whitespace.
bool isValidTitle(const string& title)
{
    // Condition key: rejects empty titles and titles with 35 or more characters.
    if (title.length() == 0 || title.length() >= 35)
    {
        return false;
    }

    bool hasLetter = false;
    bool hasNonSpace = false;

    // Loop key: checks every character safely with cctype.
    for (int i = 0; i < static_cast<int>(title.length()); i++)
    {
        unsigned char character = static_cast<unsigned char>(title[i]);
            // Condition key: tracks whether the title contains a letter.
            if (isalpha(character))
        {
            hasLetter = true;
        }
            // Condition key: tracks whether the title contains non-space content.
        if (!isspace(character))
        {
            hasNonSpace = true;
        }
    }

    return hasLetter && hasNonSpace;
}

// bool addEntry - called by the Add button; stores valid values and updates a static counter.
bool addEntry(string titles[], double amounts[], string categories[], string types[], int& count,
              int maxSize, const string& title, double amount, const string& category,
              const string& entryType)
{
    // Static local key: counts valid transaction additions during this application run.
    static int addedEntryCount = 0;

    // Condition key: requires capacity, valid content, and a supported transaction type.
    if (count < maxSize && isValidTitle(title) && amount > 0.0 &&
        (entryType == "Expense" || entryType == "Income"))
    {
        // Array storage key: stores matching fields at the same index.
        titles[count] = title;
        amounts[count] = amount;
        categories[count] = category;
        types[count] = entryType;
        count++;
        addedEntryCount++;
        return true;
    }

    return false;
}

// bool deleteEntry - called by Delete buttons; removes one entry without leaving gaps.
bool deleteEntry(string titles[], double amounts[], string categories[], string types[],
                 int& count, int indexToRemove)
{
    // Condition key: rejects a 0-based index outside the active array range.
    if (indexToRemove < 0 || indexToRemove >= count)
    {
        return false;
    }

    // Loop key: shifts later parallel array values one position left.
    for (int i = indexToRemove; i < count - 1; i++)
    {
        titles[i] = titles[i + 1];
        amounts[i] = amounts[i + 1];
        categories[i] = categories[i + 1];
        types[i] = types[i + 1];
    }

    count--;
    return true;
}

// double calculateTotalByType - called by dashboard and Analytics; sums one transaction type.
double calculateTotalByType(const double amounts[], const string types[], int count, const string& targetType)
{
    double total = 0.0;

    // Loop key: visits active entries and selects the requested transaction type.
    for (int i = 0; i < count; i++)
    {
        // Condition key: adds only amounts matching the requested type.
        if (types[i] == targetType)
        {
            total = total + amounts[i];
        }
    }

    return total;
}

// inline double calculateBudgetUsedPercent - called by dashboard; safely calculates pool usage.
inline double calculateBudgetUsedPercent(double spent, double budget)
{
    // Condition key: avoids division by a zero or negative available pool.
    if (budget <= 0.0)
    {
        return 0.0;
    }

    return static_cast<double>(spent) / budget * 100.0;
}

// inline double calculateAvailableBalance - called by dashboard; combines budget, income, and expenses.
inline double calculateAvailableBalance(double baseBudget, double totalIncome, double totalExpense)
{
    return baseBudget + totalIncome - totalExpense;
}

// double calculateCategoryTotal - called by Analytics; sums one category within a transaction type.
double calculateCategoryTotal(const string categories[], const double amounts[], const string types[],
                              int count, const string& targetCategory, const string& targetType)
{
    double total = 0.0;

    // Loop key: checks each active entry for matching category and type.
    for (int i = 0; i < count; i++)
    {
        // Condition key: adds only entries matching both selected values.
        if (categories[i] == targetCategory && types[i] == targetType)
        {
            total = total + amounts[i];
        }
    }

    return total;
}

// int findHighestExpenseIndex - called by Analytics; returns largest expense index or -1 when empty.
int findHighestExpenseIndex(const double amounts[], const string types[], int count)
{
    int highestIndex = -1;

    // Loop key: finds the largest amount while ignoring income entries.
    for (int i = 0; i < count; i++)
    {
        // Condition key: records the first expense or a larger expense.
        if (types[i] == "Expense" && (highestIndex < 0 || amounts[i] > amounts[highestIndex]))
        {
            highestIndex = i;
        }
    }

    return highestIndex;
}

// recursive double sumEntriesByTypeRecursive - called by Analytics; recursively sums one entry type.
double sumEntriesByTypeRecursive(const double amounts[], const string types[], int count,
                                 const string& targetType, int index)
{
    // Base case key: stops safely when the index reaches the active entry count.
    if (index >= count)
    {
        return 0.0;
    }

    // Recursive case key: adds matching amounts and continues at the next index.
    if (types[index] == targetType)
    {
        return amounts[index] + sumEntriesByTypeRecursive(amounts, types, count, targetType, index + 1);
    }
    return sumEntriesByTypeRecursive(amounts, types, count, targetType, index + 1);
}

// string getBudgetAlertText - called by dashboard and printBudgetAlert; returns budget status.
string getBudgetAlertText(double spent, double budget)
{
    double usedPercent = calculateBudgetUsedPercent(spent, budget);

    // Condition key: marks spending above the budget as over budget.
    if (spent > budget)
    {
        return "Over Budget";
    }
    // Else-if key: warns when 80 percent or more of the budget is used.
    else if (usedPercent >= 80.0)
    {
        return "Near Limit";
    }
    // Else key: reports spending that remains on track.
    return "On Track";
}

// void printBudgetAlert - called by console analytics; prints the current budget status.
void printBudgetAlert(double spent, double budget)
{
    cout << "Budget Alert: " << getBudgetAlertText(spent, budget) << "\n";
}

// void displayExpenses - called by console presentation; prints typed entries and balance totals.
void displayExpenses(const string titles[], const double amounts[], const string categories[],
                     const string types[], int count, double budget)
{
    // Condition key: reports an empty transaction list.
    if (count == 0)
    {
        cout << "No transactions recorded.\n";
    }
    // Else key: lists active transactions using 1-based display numbers.
    else
    {
        // Loop key: prints each stored transaction.
        for (int i = 0; i < count; i++)
        {
            cout << i + 1 << ". " << titles[i] << " - " << amounts[i]
                 << " - " << categories[i] << " - " << types[i] << "\n";
        }
    }

    double totalIncome = calculateTotalByType(amounts, types, count, "Income");
    double totalExpense = calculateTotalByType(amounts, types, count, "Expense");
    double availablePool = budget + totalIncome;
    cout << "Total Incoming: " << totalIncome << "\n";
    cout << "Total Spent: " << totalExpense << "\n";
    cout << "Available Balance: " << calculateAvailableBalance(budget, totalIncome, totalExpense) << "\n";
    cout << "Budget Used: " << calculateBudgetUsedPercent(totalExpense, availablePool) << "%\n";
}

// void printAnalyticsSummary - called by console presentation; prints type totals and recursive checks.
void printAnalyticsSummary(const string titles[], const double amounts[], const string categories[],
                           const string types[], int count, double budget)
{
    double totalIncome = calculateTotalByType(amounts, types, count, "Income");
    double totalExpense = calculateTotalByType(amounts, types, count, "Expense");
    cout << "Income: " << totalIncome << "\n";
    cout << "Expense: " << totalExpense << "\n";
    cout << "Food Expenses: " << calculateCategoryTotal(categories, amounts, types, count, "Food", "Expense") << "\n";
    cout << "Travel Expenses: " << calculateCategoryTotal(categories, amounts, types, count, "Travel", "Expense") << "\n";
    cout << "Study Expenses: " << calculateCategoryTotal(categories, amounts, types, count, "Study", "Expense") << "\n";
    cout << "Other Expenses: " << calculateCategoryTotal(categories, amounts, types, count, "Other", "Expense") << "\n";

    // Highest expense key: prints a value only when the list is not empty.
    int highestIndex = findHighestExpenseIndex(amounts, types, count);
    if (highestIndex < 0)
    {
        cout << "Highest Expense: None\n";
    }
    else
    {
        cout << "Highest Expense: " << titles[highestIndex] << " - "
             << amounts[highestIndex] << "\n";
    }

    cout << "Income Recursive Check: " << sumEntriesByTypeRecursive(amounts, types, count, "Income", 0)
         << " / " << totalIncome << "\n";
    cout << "Expense Recursive Check: " << sumEntriesByTypeRecursive(amounts, types, count, "Expense", 0)
         << " / " << totalExpense << "\n";
    printBudgetAlert(totalExpense, budget + totalIncome);
}

// wstring formatAmount - called by GDI drawing functions; formats a number to two decimals.
wstring formatAmount(double value)
{
    char buffer[400];
    sprintf(buffer, "%.2f", value);
    wstring formatted;

    // Loop key: converts formatted ASCII number characters for Win32 drawing.
    for (int i = 0; buffer[i] != '\0'; i++)
    {
        formatted += (wchar_t)buffer[i];
    }

    return formatted;
}

// wstring toWide - called by expense rows; converts simple title text to wide characters.
wstring toWide(const string& text)
{
    wstring wideText;

    // Loop key: copies each stored title character for the native window.
    for (int i = 0; i < static_cast<int>(text.length()); i++)
    {
        wideText += (wchar_t)static_cast<unsigned char>(text[i]);
    }

    return wideText;
}

// void drawText - called by paint helpers; draws a line of text with the requested style.
void drawText(HDC deviceContext, const wchar_t* text, int x, int y, int size, COLORREF color, bool bold)
{
    // Font selection step: uses one of the cached crisp Segoe UI sizes.
    HFONT font = bodyFont != NULL ? bodyFont : (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    if (size >= 24)
    {
        font = pageTitleFont;
    }
    else if (size >= 20)
    {
        font = kpiNumberFont;
    }
    else if (size >= 15)
    {
        font = sectionFont;
    }
    else if (size <= 12 && bold)
    {
        font = captionFont;
    }
    HFONT oldFont = (HFONT)SelectObject(deviceContext, font);
    int oldMode = SetBkMode(deviceContext, TRANSPARENT);
    COLORREF oldColor = SetTextColor(deviceContext, color);
    TextOutW(deviceContext, x, y, text, lstrlenW(text));
    SetTextColor(deviceContext, oldColor);
    SetBkMode(deviceContext, oldMode);
    SelectObject(deviceContext, oldFont);
}

// void drawRightText - called by the ledger; right-aligns a line of amount text.
void drawRightText(HDC deviceContext, const wchar_t* text, int right, int y, int size, COLORREF color, bool bold)
{
    HFONT font = bodyFont != NULL ? bodyFont : (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    if (size >= 24)
    {
        font = pageTitleFont;
    }
    else if (size >= 20)
    {
        font = kpiNumberFont;
    }
    else if (size >= 15)
    {
        font = sectionFont;
    }
    else if (size <= 12 && bold)
    {
        font = captionFont;
    }
    HFONT oldFont = (HFONT)SelectObject(deviceContext, font);
    SIZE textSize;
    GetTextExtentPoint32W(deviceContext, text, lstrlenW(text), &textSize);
    SelectObject(deviceContext, oldFont);
    drawText(deviceContext, text, right - textSize.cx, y, size, color, bold);
}

// void drawCard - called by dashboard drawing functions; draws a white bordered card.
void drawCard(HDC deviceContext, int left, int top, int width, int height)
{
    RECT card = {left, top, left + width, top + height};
    HBRUSH fill = CreateSolidBrush(RGB(255, 255, 255));
    HPEN border = CreatePen(PS_SOLID, 1, RGB(203, 213, 225));
    HBRUSH oldBrush = (HBRUSH)SelectObject(deviceContext, fill);
    HPEN oldPen = (HPEN)SelectObject(deviceContext, border);
    RoundRect(deviceContext, card.left, card.top, card.right, card.bottom, 10, 10);
    SelectObject(deviceContext, oldBrush);
    SelectObject(deviceContext, oldPen);
    DeleteObject(fill);
    DeleteObject(border);
}

// void drawProgressBar - called by Analytics; draws a category share bar.
void drawProgressBar(HDC deviceContext, int left, int top, int width, double fraction, COLORREF color)
{
    RECT track = {left, top, left + width, top + 14};
    HBRUSH trackBrush = CreateSolidBrush(RGB(231, 234, 239));
    FillRect(deviceContext, &track, trackBrush);
    DeleteObject(trackBrush);

    // Condition key: prevents a negative bar fraction.
    if (fraction < 0.0)
    {
        fraction = 0.0;
    }
    // Condition key: prevents a bar from exceeding its track.
    if (fraction > 1.0)
    {
        fraction = 1.0;
    }
    int fillWidth = static_cast<int>(width * fraction);
    // Condition key: draws a fill only when its width is visible.
    if (fillWidth > 0)
    {
        RECT fill = {left, top, left + fillWidth, top + 14};
        HBRUSH fillBrush = CreateSolidBrush(color);
        FillRect(deviceContext, &fill, fillBrush);
        DeleteObject(fillBrush);
    }
}

// void drawDashboard - called by paintWindow; renders typed balance cards, entry form, and ledger.
void drawDashboard(HDC deviceContext, int width, int height)
{
    int left = 28;
    int contentWidth = width - 56;
    double totalIncome = calculateTotalByType(entryAmounts, entryTypes, entryCount, "Income");
    double totalExpense = calculateTotalByType(entryAmounts, entryTypes, entryCount, "Expense");
    double availablePool = monthlyBudget + totalIncome;
    double availableBalance = calculateAvailableBalance(monthlyBudget, totalIncome, totalExpense);
    double usedPercent = calculateBudgetUsedPercent(totalExpense, availablePool);

    // Page heading draw key: introduces the selected banking ledger page.
    drawText(deviceContext, L"Account Overview", left, 150, 24, RGB(15, 23, 42), true);

    // KPI card draw key: shows available balance, incoming money, and outgoing spending.
    int gap = 14;
    int cardWidth = (contentWidth - gap * 2) / 3;
    int cardTop = 188;
    drawCard(deviceContext, left, cardTop, cardWidth, 122);
    drawCard(deviceContext, left + cardWidth + gap, cardTop, cardWidth, 122);
    drawCard(deviceContext, left + (cardWidth + gap) * 2, cardTop, cardWidth, 122);
    HBRUSH blueAccent = CreateSolidBrush(RGB(37, 99, 235));
    HBRUSH indigoAccent = CreateSolidBrush(RGB(99, 102, 241));
    HBRUSH greenAccent = CreateSolidBrush(RGB(5, 150, 105));
    RECT firstAccent = {left + 1, cardTop + 1, left + cardWidth - 1, cardTop + 5};
    RECT secondAccent = {left + cardWidth + gap + 1, cardTop + 1, left + cardWidth * 2 + gap - 1, cardTop + 5};
    RECT thirdAccent = {left + (cardWidth + gap) * 2 + 1, cardTop + 1, left + contentWidth - 1, cardTop + 5};
    FillRect(deviceContext, &firstAccent, blueAccent);
    FillRect(deviceContext, &secondAccent, indigoAccent);
    FillRect(deviceContext, &thirdAccent, greenAccent);
    DeleteObject(blueAccent);
    DeleteObject(indigoAccent);
    DeleteObject(greenAccent);
    drawText(deviceContext, L"AVAILABLE BALANCE", left + 18, cardTop + 19, 12, RGB(100, 116, 139), true);
    drawText(deviceContext, formatAmount(availableBalance).c_str(), left + 18, cardTop + 45, 26, RGB(15, 23, 42), true);
    drawText(deviceContext, L"TOTAL INCOMING", left + cardWidth + gap + 18, cardTop + 19, 12, RGB(100, 116, 139), true);
    drawText(deviceContext, formatAmount(totalIncome).c_str(), left + cardWidth + gap + 18, cardTop + 45, 26, RGB(5, 120, 87), true);
    drawText(deviceContext, L"TOTAL SPENT", left + (cardWidth + gap) * 2 + 18, cardTop + 19, 12, RGB(100, 116, 139), true);
    wstring expenseText = formatAmount(totalExpense) + L" / " + formatAmount(usedPercent) + L"%";
    drawText(deviceContext, expenseText.c_str(), left + (cardWidth + gap) * 2 + 18, cardTop + 45, 21, RGB(15, 23, 42), true);
    drawProgressBar(deviceContext, left + (cardWidth + gap) * 2 + 18, cardTop + 91,
                    cardWidth - 36, availablePool > 0.0 ? totalExpense / availablePool : 0.0,
                    RGB(5, 150, 105));

    // Input panel draw key: labels title, amount, category, type, and action controls.
    drawCard(deviceContext, left, 326, contentWidth, 108);
    drawText(deviceContext, L"NEW TRANSACTION", left + 18, 340, 15, RGB(15, 23, 42), true);
    drawText(deviceContext, L"TITLE", left + 18, 365, 12, RGB(100, 116, 139), true);
    drawText(deviceContext, L"AMOUNT", left + contentWidth * 25 / 100 + 28, 365, 12, RGB(100, 116, 139), true);
    drawText(deviceContext, L"CATEGORY", left + contentWidth * 40 / 100 + 40, 365, 12, RGB(100, 116, 139), true);
    drawText(deviceContext, L"TYPE", left + contentWidth * 59 / 100 + 52, 365, 12, RGB(100, 116, 139), true);

    // Ledger draw key: displays a tinted table heading and recent transactions.
    int listTop = 448;
    int listHeight = height - listTop - 24;
    drawCard(deviceContext, left, listTop, contentWidth, listHeight);
    drawText(deviceContext, L"RECENT TRANSACTIONS", left + 18, listTop + 12, 15, RGB(15, 23, 42), true);
    RECT tableHeader = {left + 1, listTop + 42, left + contentWidth - 1, listTop + 72};
    HBRUSH tableHeaderBrush = CreateSolidBrush(RGB(248, 250, 252));
    FillRect(deviceContext, &tableHeader, tableHeaderBrush);
    DeleteObject(tableHeaderBrush);
    drawText(deviceContext, L"TYPE", left + 20, listTop + 50, 12, RGB(100, 116, 139), true);
    drawText(deviceContext, L"CATEGORY", left + 118, listTop + 50, 12, RGB(100, 116, 139), true);
    drawText(deviceContext, L"DESCRIPTION", left + 220, listTop + 50, 12, RGB(100, 116, 139), true);
    drawRightText(deviceContext, L"AMOUNT", left + contentWidth - 124, listTop + 50, 12, RGB(100, 116, 139), true);

    // Condition key: gives an empty ledger a helpful message.
    if (entryCount == 0)
    {
        drawText(deviceContext, L"No transactions recorded.", left + 20, listTop + 86, 14, RGB(100, 116, 139), false);
    }
    else
    {
        // Entry loop key: draws one ledger row for each valid array entry.
        for (int i = 0; i < entryCount; i++)
        {
            int rowY = listTop + 76 + i * 24;
            HPEN rowPen = CreatePen(PS_SOLID, 1, RGB(241, 245, 249));
            HPEN oldPen = (HPEN)SelectObject(deviceContext, rowPen);
            MoveToEx(deviceContext, left + 16, rowY + 28, NULL);
            LineTo(deviceContext, left + contentWidth - 16, rowY + 23);
            SelectObject(deviceContext, oldPen);
            DeleteObject(rowPen);

            COLORREF pillFill = RGB(226, 232, 240);
            COLORREF pillText = RGB(51, 65, 85);
            if (entryTypes[i] == "Income")
            {
                pillFill = RGB(220, 252, 231);
                pillText = RGB(21, 128, 61);
            }
            else
            {
                pillFill = RGB(254, 226, 226);
                pillText = RGB(185, 28, 28);
            }

            RECT typePill = {left + 18, rowY + 1, left + 96, rowY + 23};
            HBRUSH typeBrush = CreateSolidBrush(pillFill);
            HPEN typePen = CreatePen(PS_SOLID, 1, pillFill);
            HBRUSH oldTypeBrush = (HBRUSH)SelectObject(deviceContext, typeBrush);
            HPEN oldTypePen = (HPEN)SelectObject(deviceContext, typePen);
            RoundRect(deviceContext, typePill.left, typePill.top, typePill.right, typePill.bottom, 10, 10);
            SelectObject(deviceContext, oldTypeBrush);
            SelectObject(deviceContext, oldTypePen);
            DeleteObject(typeBrush);
            DeleteObject(typePen);
            drawText(deviceContext, entryTypes[i] == "Income" ? L"INCOME" : L"EXPENSE",
                     left + 26, rowY + 4, 12, pillText, true);

            pillFill = RGB(226, 232, 240);
            pillText = RGB(51, 65, 85);
            if (entryCategories[i] == "Food")
            {
                pillFill = RGB(219, 234, 254);
                pillText = RGB(29, 78, 216);
            }
            else if (entryCategories[i] == "Travel" || entryCategories[i] == "Freelance")
            {
                pillFill = RGB(220, 252, 231);
                pillText = RGB(21, 128, 61);
            }
            else if (entryCategories[i] == "Study" || entryCategories[i] == "Stipend")
            {
                pillFill = RGB(237, 233, 254);
                pillText = RGB(109, 40, 217);
            }
            else if (entryCategories[i] == "Other" || entryCategories[i] == "Allowance")
            {
                pillFill = RGB(255, 237, 213);
                pillText = RGB(194, 65, 12);
            }

            RECT pill = {left + 112, rowY + 1, left + 196, rowY + 23};
            HBRUSH pillBrush = CreateSolidBrush(pillFill);
            HPEN pillPen = CreatePen(PS_SOLID, 1, pillFill);
            HBRUSH oldPillBrush = (HBRUSH)SelectObject(deviceContext, pillBrush);
            HPEN oldPillPen = (HPEN)SelectObject(deviceContext, pillPen);
            RoundRect(deviceContext, pill.left, pill.top, pill.right, pill.bottom, 10, 10);
            SelectObject(deviceContext, oldPillBrush);
            SelectObject(deviceContext, oldPillPen);
            DeleteObject(pillBrush);
            DeleteObject(pillPen);
            drawText(deviceContext, toWide(entryCategories[i]).c_str(), left + 120, rowY + 4, 12, pillText, true);

            int savedDc = SaveDC(deviceContext);
            IntersectClipRect(deviceContext, left + 220, rowY, left + contentWidth - 330, rowY + 24);
            drawText(deviceContext, toWide(entryTitles[i]).c_str(), left + 220, rowY + 4, 14, RGB(51, 65, 85), false);
            RestoreDC(deviceContext, savedDc);
            wstring signedAmount = (entryTypes[i] == "Income" ? L"+ " : L"- ") + formatAmount(entryAmounts[i]);
            drawRightText(deviceContext, signedAmount.c_str(),
                          left + contentWidth - 126, rowY + 4, 14,
                          entryTypes[i] == "Income" ? RGB(5, 120, 87) : RGB(185, 28, 28), true);
        }
    }
}

// void drawAnalytics - called by paintWindow; compares typed totals, categories, and recursive audits.
void drawAnalytics(HDC deviceContext, int width, int height)
{
    int left = 28;
    int contentWidth = width - 56;
    int top = 188;
    drawText(deviceContext, L"Spending Analytics", left, 148, 24, RGB(15, 23, 42), true);
    drawCard(deviceContext, left, top, contentWidth, height - top - 24);
    drawText(deviceContext, L"EXPENSE CATEGORY BREAKDOWN", left + 18, top + 17, 15, RGB(15, 23, 42), true);

    string categoryNames[] = {"Food", "Travel", "Study", "Other"};
    COLORREF barColors[] = {
        RGB(37, 99, 235),
        RGB(5, 150, 105),
        RGB(99, 102, 241),
        RGB(245, 158, 11)
    };
    double totalExpense = calculateTotalByType(entryAmounts, entryTypes, entryCount, "Expense");
    double totalIncome = calculateTotalByType(entryAmounts, entryTypes, entryCount, "Income");

    // Category loop key: computes each expense category total and its share of spending.
    for (int i = 0; i < 4; i++)
    {
        int rowY = top + 56 + i * 58;
        double categoryTotal = calculateCategoryTotal(entryCategories, entryAmounts, entryTypes,
                                                       entryCount, categoryNames[i], "Expense");
        double fraction = totalExpense > 0.0 ? categoryTotal / totalExpense : 0.0;
        drawText(deviceContext, toWide(categoryNames[i]).c_str(), left + 20, rowY, 14, RGB(51, 65, 85), true);
        drawRightText(deviceContext, formatAmount(categoryTotal).c_str(), left + contentWidth - 20,
                      rowY, 14, RGB(15, 23, 42), true);
        drawProgressBar(deviceContext, left + 20, rowY + 25, contentWidth - 42, fraction, barColors[i]);
    }

    // Highest expense card key: highlights the largest transaction.
    int detailY = top + 300;
    drawCard(deviceContext, left + 18, detailY, contentWidth - 36, 66);
    int highestIndex = findHighestExpenseIndex(entryAmounts, entryTypes, entryCount);
    // Condition key: handles empty data without reading an array slot.
    if (highestIndex < 0)
    {
        drawText(deviceContext, L"HIGHEST EXPENSE", left + 34, detailY + 10, 12, RGB(100, 116, 139), true);
        drawText(deviceContext, L"No expenses recorded", left + 34, detailY + 32, 14, RGB(51, 65, 85), true);
    }
    else
    {
        wstring highestText = L"Highest Expense: " + toWide(entryTitles[highestIndex]) +
                              L" - " + formatAmount(entryAmounts[highestIndex]);
        drawText(deviceContext, L"HIGHEST EXPENSE", left + 34, detailY + 8, 12, RGB(100, 116, 139), true);
        drawText(deviceContext, highestText.c_str(), left + 34, detailY + 30, 15, RGB(15, 23, 42), true);
    }

    // Recursive audit badge key: compares the recursive and iterative totals.
    int auditY = detailY + 78;
    drawCard(deviceContext, left + 18, auditY, contentWidth - 36, 96);
    drawText(deviceContext, L"RECURSIVE TOTAL AUDIT", left + 34, auditY + 10, 12, RGB(100, 116, 139), true);
    wstring expenseAudit = L"Expense: " + formatAmount(totalExpense) + L" / " +
                           formatAmount(sumEntriesByTypeRecursive(entryAmounts, entryTypes, entryCount, "Expense", 0));
    wstring incomeAudit = L"Income: " + formatAmount(totalIncome) + L" / " +
                          formatAmount(sumEntriesByTypeRecursive(entryAmounts, entryTypes, entryCount, "Income", 0));
    drawText(deviceContext, expenseAudit.c_str(), left + 34, auditY + 34, 14, RGB(185, 28, 28), true);
    drawText(deviceContext, incomeAudit.c_str(), left + contentWidth / 2, auditY + 34,
             14, RGB(5, 120, 87), true);
    wstring balanceText = L"Available Balance: " +
                          formatAmount(calculateAvailableBalance(monthlyBudget, totalIncome, totalExpense));
    drawText(deviceContext, balanceText.c_str(), left + 34, auditY + 61, 14, RGB(15, 23, 42), true);
}

// void drawBudgetSettings - called by paintWindow; draws the budget update and reset panel.
void drawBudgetSettings(HDC deviceContext, int width)
{
    int left = 28;
    int contentWidth = width - 56;
    drawText(deviceContext, L"Budget Settings", left, 148, 24, RGB(15, 23, 42), true);
    drawCard(deviceContext, left, 188, contentWidth, 300);
    drawText(deviceContext, L"MONTHLY SPENDING LIMIT", left + 20, 208, 12, RGB(100, 116, 139), true);
    drawText(deviceContext, L"Set a monthly budget using a value of zero or more.", left + 20, 238, 14, RGB(51, 65, 85), false);
    drawText(deviceContext, L"Clearing entries resets both income and expense transactions.", left + 20, 330, 14, RGB(51, 65, 85), false);
    drawText(deviceContext, L"All values remain in the C++ parallel arrays while the program runs.", left + 20, 362, 14, RGB(100, 116, 139), false);
    drawText(deviceContext, (L"Current budget: " + formatAmount(monthlyBudget)).c_str(),
             left + 20, 405, 15, RGB(15, 23, 42), true);
}

// void paintWindow - called by WindowProc; draws the light-gray background and active panel.
void paintWindow(HWND window)
{
    PAINTSTRUCT paint;
    HDC windowContext = BeginPaint(window, &paint);
    RECT client;
    GetClientRect(window, &client);
    int width = client.right - client.left;
    int height = client.bottom - client.top;

    // Buffer setup step: creates an off-screen surface to prevent resize flicker.
    HDC bufferContext = CreateCompatibleDC(windowContext);
    HBITMAP bufferBitmap = CreateCompatibleBitmap(windowContext, width, height);
    HBITMAP oldBitmap = NULL;
    HDC deviceContext = windowContext;
    if (bufferContext != NULL && bufferBitmap != NULL)
    {
        oldBitmap = (HBITMAP)SelectObject(bufferContext, bufferBitmap);
        deviceContext = bufferContext;
    }

    // Canvas draw step: fills the buffered surface with cool slate.
    HBRUSH background = CreateSolidBrush(RGB(241, 245, 249));
    FillRect(deviceContext, &client, background);
    DeleteObject(background);

    // Header draw step: paints the deep-navy banking brand bar.
    RECT header = {0, 0, width, 130};
    HBRUSH headerBrush = CreateSolidBrush(RGB(15, 23, 42));
    FillRect(deviceContext, &header, headerBrush);
    DeleteObject(headerBrush);
    drawText(deviceContext, L"FINTRACK // PERSONAL LEDGER", 26, 18, 24, RGB(255, 255, 255), true);
    drawText(deviceContext, L"Welcome back, M Nouman Habib", 28, 54, 14, RGB(203, 213, 225), false);

    // Status pill step: shows the current budget state in its matching accent color.
    double totalIncome = calculateTotalByType(entryAmounts, entryTypes, entryCount, "Income");
    double totalExpense = calculateTotalByType(entryAmounts, entryTypes, entryCount, "Expense");
    double availablePool = monthlyBudget + totalIncome;
    COLORREF statusColor = RGB(16, 185, 129);
    const wchar_t* statusLabel = L"ON TRACK";
    if (totalExpense > availablePool)
    {
        statusColor = RGB(239, 68, 68);
        statusLabel = L"OVER BUDGET";
    }
    else if (calculateBudgetUsedPercent(totalExpense, availablePool) >= 80.0)
    {
        statusColor = RGB(245, 158, 11);
        statusLabel = L"NEAR LIMIT";
    }
    RECT statusRect = {width - 205, 27, width - 28, 65};
    HBRUSH statusBrush = CreateSolidBrush(statusColor);
    HPEN statusPen = CreatePen(PS_SOLID, 1, statusColor);
    HBRUSH oldStatusBrush = (HBRUSH)SelectObject(deviceContext, statusBrush);
    HPEN oldStatusPen = (HPEN)SelectObject(deviceContext, statusPen);
    RoundRect(deviceContext, statusRect.left, statusRect.top, statusRect.right,
              statusRect.bottom, 18, 18);
    SelectObject(deviceContext, oldStatusBrush);
    SelectObject(deviceContext, oldStatusPen);
    DeleteObject(statusBrush);
    DeleteObject(statusPen);
    drawText(deviceContext, statusLabel, statusRect.left + 18, statusRect.top + 10,
             12, RGB(255, 255, 255), true);

    // Content draw step: paints the section selected by the navigation controls.
    if (activeTab == 0)
    {
        drawDashboard(deviceContext, width, height);
    }
    else if (activeTab == 1)
    {
        drawAnalytics(deviceContext, width, height);
    }
    else
    {
        drawBudgetSettings(deviceContext, width);
    }

    // Buffer present step: copies the complete frame to the visible window.
    if (deviceContext == bufferContext)
    {
        BitBlt(windowContext, 0, 0, width, height, bufferContext, 0, 0, SRCCOPY);
        SelectObject(bufferContext, oldBitmap);
    }
    if (bufferBitmap != NULL)
    {
        DeleteObject(bufferBitmap);
    }
    if (bufferContext != NULL)
    {
        DeleteDC(bufferContext);
    }

    EndPaint(window, &paint);
}

// void createControls - called during WM_CREATE; creates native inputs and navigation buttons.
void createControls(HWND window)
{
    // Font setup step: creates the specified Segoe UI presentation sizes.
    createFonts();

    titleEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                                WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                                0, 0, 10, 10, window, (HMENU)(INT_PTR)200, NULL, NULL);
    amountEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                                 WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                                 0, 0, 10, 10, window, (HMENU)(INT_PTR)201, NULL, NULL);
    categoryCombo = CreateWindowExW(WS_EX_CLIENTEDGE, L"COMBOBOX", L"",
                                    WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
                                    0, 0, 10, 150, window, (HMENU)(INT_PTR)202, NULL, NULL);
    typeCombo = CreateWindowExW(WS_EX_CLIENTEDGE, L"COMBOBOX", L"",
                                WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
                                0, 0, 10, 100, window, (HMENU)(INT_PTR)204, NULL, NULL);
    budgetEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"15000.00",
                                 WS_CHILD | ES_AUTOHSCROLL,
                                 0, 0, 10, 10, window, (HMENU)(INT_PTR)203, NULL, NULL);

    // Type control step: lets the user choose incoming or outgoing money.
    SendMessageW(typeCombo, CB_ADDSTRING, 0, (LPARAM)L"Expense");
    SendMessageW(typeCombo, CB_ADDSTRING, 0, (LPARAM)L"Income");
    SendMessageW(typeCombo, CB_SETCURSEL, 0, 0);
    updateCategoryOptions(window);

    // Control font step: applies the crisp 14-pixel Segoe UI body font.
    SendMessageW(titleEdit, WM_SETFONT, (WPARAM)bodyFont, TRUE);
    SendMessageW(amountEdit, WM_SETFONT, (WPARAM)bodyFont, TRUE);
    SendMessageW(categoryCombo, WM_SETFONT, (WPARAM)bodyFont, TRUE);
    SendMessageW(typeCombo, WM_SETFONT, (WPARAM)bodyFont, TRUE);
    SendMessageW(budgetEdit, WM_SETFONT, (WPARAM)bodyFont, TRUE);

    // Owner-draw setup step: creates custom-painted primary and reset buttons.
    CreateWindowExW(0, L"BUTTON", L"- Add Expense", WS_CHILD | BS_OWNERDRAW,
                    0, 0, 10, 10, window, (HMENU)(INT_PTR)110, NULL, NULL);
    CreateWindowExW(0, L"BUTTON", L"Save Budget", WS_CHILD | BS_OWNERDRAW,
                    0, 0, 10, 10, window, (HMENU)(INT_PTR)111, NULL, NULL);
    CreateWindowExW(0, L"BUTTON", L"Clear All Entries", WS_CHILD | BS_OWNERDRAW,
                    0, 0, 10, 10, window, (HMENU)(INT_PTR)112, NULL, NULL);
    CreateWindowExW(0, L"BUTTON", L"Dashboard", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                    0, 0, 10, 10, window, (HMENU)(INT_PTR)100, NULL, NULL);
    CreateWindowExW(0, L"BUTTON", L"Analytics", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                    0, 0, 10, 10, window, (HMENU)(INT_PTR)101, NULL, NULL);
    CreateWindowExW(0, L"BUTTON", L"Budget Settings", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                    0, 0, 10, 10, window, (HMENU)(INT_PTR)102, NULL, NULL);

    // Delete control loop: creates a custom button for each transaction slot.
    for (int i = 0; i < MAX_TRANSACTIONS; i++)
    {
        deleteButtons[i] = CreateWindowExW(0, L"BUTTON", L"Delete",
                                           WS_CHILD | BS_OWNERDRAW,
                                           0, 0, 10, 10, window,
                                           (HMENU)(INT_PTR)(300 + i), NULL, NULL);
    }

    layoutControls(window);
}

// void layoutControls - called on resize and tab changes; positions active Win32 controls.
void layoutControls(HWND window)
{
    RECT client;
    GetClientRect(window, &client);
    int width = client.right - client.left;
    int contentWidth = width - 56;
    bool dashboardVisible = activeTab == 0;
    bool settingsVisible = activeTab == 2;

    ShowWindow(titleEdit, dashboardVisible ? SW_SHOW : SW_HIDE);
    ShowWindow(amountEdit, dashboardVisible ? SW_SHOW : SW_HIDE);
    ShowWindow(categoryCombo, dashboardVisible ? SW_SHOW : SW_HIDE);
    ShowWindow(typeCombo, dashboardVisible ? SW_SHOW : SW_HIDE);
    ShowWindow(GetDlgItem(window, 110), dashboardVisible ? SW_SHOW : SW_HIDE);
    ShowWindow(budgetEdit, settingsVisible ? SW_SHOW : SW_HIDE);
    ShowWindow(GetDlgItem(window, 111), settingsVisible ? SW_SHOW : SW_HIDE);
    ShowWindow(GetDlgItem(window, 112), settingsVisible ? SW_SHOW : SW_HIDE);

    // Navigation layout step: positions the three premium header tabs.
    SetWindowPos(GetDlgItem(window, 100), NULL, 24, 84, 142, 34, SWP_NOZORDER);
    SetWindowPos(GetDlgItem(window, 101), NULL, 176, 84, 142, 34, SWP_NOZORDER);
    SetWindowPos(GetDlgItem(window, 102), NULL, 328, 84, 166, 34, SWP_NOZORDER);

    // Dashboard layout step: positions the dashboard-only input controls.
    if (dashboardVisible)
    {
        int titleWidth = contentWidth * 24 / 100;
        int amountWidth = contentWidth * 15 / 100;
        int categoryWidth = contentWidth * 18 / 100;
        int typeWidth = contentWidth * 16 / 100;
        int addWidth = contentWidth - titleWidth - amountWidth - categoryWidth - typeWidth - 76;
        // Condition key: keeps the Add button usable on smaller window widths.
        if (addWidth < 92)
        {
            addWidth = 92;
            titleWidth = contentWidth - amountWidth - categoryWidth - typeWidth - addWidth - 76;
        }

        SetWindowPos(titleEdit, NULL, 44, 388, titleWidth - 8, 29, SWP_NOZORDER);
        SetWindowPos(amountEdit, NULL, 56 + titleWidth, 388, amountWidth - 8, 29, SWP_NOZORDER);
        SetWindowPos(categoryCombo, NULL, 68 + titleWidth + amountWidth, 388,
                     categoryWidth - 8, 180, SWP_NOZORDER);
        SetWindowPos(typeCombo, NULL, 80 + titleWidth + amountWidth + categoryWidth, 388,
                     typeWidth - 8, 140, SWP_NOZORDER);
        SetWindowPos(GetDlgItem(window, 110), NULL,
                     92 + titleWidth + amountWidth + categoryWidth + typeWidth,
                     388, addWidth, 31, SWP_NOZORDER);

        // Delete layout loop: shows and positions active transaction buttons.
        for (int i = 0; i < MAX_TRANSACTIONS; i++)
        {
            if (i < entryCount)
            {
                int rowY = 448 + 76 + i * 24;
                SetWindowPos(deleteButtons[i], NULL, 28 + contentWidth - 96, rowY,
                             76, 22, SWP_NOZORDER | SWP_SHOWWINDOW);
            }
            else
            {
                ShowWindow(deleteButtons[i], SW_HIDE);
            }
        }
    }
    else
    {
        // Delete visibility step: hides row controls outside the Dashboard tab.
        for (int i = 0; i < MAX_TRANSACTIONS; i++)
        {
            ShowWindow(deleteButtons[i], SW_HIDE);
        }
    }

    // Settings layout step: positions budget controls only on their tab.
    if (settingsVisible)
    {
        SetWindowPos(budgetEdit, NULL, 48, 266, 220, 31, SWP_NOZORDER);
        SetWindowPos(GetDlgItem(window, 111), NULL, 280, 266, 150, 34, SWP_NOZORDER);
        SetWindowPos(GetDlgItem(window, 112), NULL, 48, 374, 190, 34, SWP_NOZORDER);
    }
}

// void setTab - called by navigation buttons; selects a tab and refreshes its controls.
void setTab(HWND window, int tab)
{
    activeTab = tab;
    // Condition key: refreshes the budget editor when Settings is selected.
    if (activeTab == 2)
    {
        wstring currentBudget = formatAmount(monthlyBudget);
        SetWindowTextW(budgetEdit, currentBudget.c_str());
    }
    layoutControls(window);
    InvalidateRect(window, NULL, TRUE);
    InvalidateRect(GetDlgItem(window, 100), NULL, TRUE);
    InvalidateRect(GetDlgItem(window, 101), NULL, TRUE);
    InvalidateRect(GetDlgItem(window, 102), NULL, TRUE);
}

// void updateCategoryOptions - called by the type selector; reloads matching category choices.
void updateCategoryOptions(HWND window)
{
    int selectedType = (int)SendMessageW(typeCombo, CB_GETCURSEL, 0, 0);
    SendMessageW(categoryCombo, CB_RESETCONTENT, 0, 0);

    // Category step: uses separate category sets for income and expense entries.
    if (selectedType == 1)
    {
        SendMessageW(categoryCombo, CB_ADDSTRING, 0, (LPARAM)L"Allowance");
        SendMessageW(categoryCombo, CB_ADDSTRING, 0, (LPARAM)L"Freelance");
        SendMessageW(categoryCombo, CB_ADDSTRING, 0, (LPARAM)L"Stipend");
        SendMessageW(categoryCombo, CB_ADDSTRING, 0, (LPARAM)L"Other");
        SetWindowTextW(GetDlgItem(window, 110), L"+ Add Income");
    }
    else
    {
        SendMessageW(categoryCombo, CB_ADDSTRING, 0, (LPARAM)L"Food");
        SendMessageW(categoryCombo, CB_ADDSTRING, 0, (LPARAM)L"Travel");
        SendMessageW(categoryCombo, CB_ADDSTRING, 0, (LPARAM)L"Study");
        SendMessageW(categoryCombo, CB_ADDSTRING, 0, (LPARAM)L"Other");
        SetWindowTextW(GetDlgItem(window, 110), L"- Add Expense");
    }

    SendMessageW(categoryCombo, CB_SETCURSEL, 0, 0);
    InvalidateRect(GetDlgItem(window, 110), NULL, TRUE);
}

// void addEntryFromControls - called by Add Entry; validates and stores user input.
void addEntryFromControls(HWND window)
{
    wchar_t titleBuffer[100] = L"";
    wchar_t amountBuffer[80] = L"";
    GetWindowTextW(titleEdit, titleBuffer, 100);
    GetWindowTextW(amountEdit, amountBuffer, 80);

    string title;
    for (int i = 0; titleBuffer[i] != L'\0'; i++)
    {
        title += (char)titleBuffer[i];
    }

    // Title validation step: rejects invalid titles before parsing amounts.
    if (!isValidTitle(title))
    {
        MessageBoxW(window, L"Enter a title with a letter and fewer than 35 characters.",
                    L"Invalid Title", MB_OK | MB_ICONWARNING);
        return;
    }

    // Amount parsing step: accepts a positive decimal and rejects trailing text.
    wchar_t* amountEnd = NULL;
    double amount = wcstod(amountBuffer, &amountEnd);
    // Condition key: rejects empty, malformed, trailing, and nonpositive amounts.
    if (amountBuffer[0] == L'\0' || amountEnd == amountBuffer ||
        *amountEnd != L'\0' || amount <= 0.0)
    {
        MessageBoxW(window, L"Enter a valid amount greater than zero.",
                    L"Invalid Amount", MB_OK | MB_ICONWARNING);
        return;
    }

    int categoryIndex = (int)SendMessageW(categoryCombo, CB_GETCURSEL, 0, 0);
    int selectedType = (int)SendMessageW(typeCombo, CB_GETCURSEL, 0, 0);
    string expenseCategoryNames[4] = {"Food", "Travel", "Study", "Other"};
    string incomeCategoryNames[4] = {"Allowance", "Freelance", "Stipend", "Other"};
    string category = selectedType == 1 ? incomeCategoryNames[categoryIndex] : expenseCategoryNames[categoryIndex];
    // Condition key: falls back to Other if no category is selected.
    if (categoryIndex < 0 || categoryIndex > 3)
    {
        categoryIndex = 3;
    }

    string entryType = selectedType == 1 ? "Income" : "Expense";

    // Add call step: reports failures from shared validation and storage logic.
    if (!addEntry(entryTitles, entryAmounts, entryCategories, entryTypes, entryCount,
                  MAX_TRANSACTIONS, title, amount, category, entryType))
    {
        MessageBoxW(window, L"The ledger is full or the transaction values are invalid.",
                    L"Entry Not Added", MB_OK | MB_ICONWARNING);
        return;
    }

    SetWindowTextW(titleEdit, L"");
    SetWindowTextW(amountEdit, L"");
    layoutControls(window);
    InvalidateRect(window, NULL, TRUE);
}

// void updateBudgetFromControl - called by Apply Budget; accepts only a valid nonnegative budget.
void updateBudgetFromControl(HWND window)
{
    wchar_t budgetBuffer[80] = L"";
    GetWindowTextW(budgetEdit, budgetBuffer, 80);

    // Budget parsing step: checks numeric input and rejects trailing text or negative values.
    wchar_t* budgetEnd = NULL;
    double newBudget = wcstod(budgetBuffer, &budgetEnd);
    // Condition key: rejects empty, malformed, trailing, or negative budgets.
    if (budgetBuffer[0] == L'\0' || budgetEnd == budgetBuffer ||
        *budgetEnd != L'\0' || newBudget < 0.0)
    {
        MessageBoxW(window, L"Enter a valid budget greater than or equal to zero.",
                    L"Invalid Budget", MB_OK | MB_ICONWARNING);
        return;
    }

    monthlyBudget = newBudget;
    InvalidateRect(window, NULL, TRUE);
}

// LRESULT CALLBACK WindowProc - called by Windows; handles input, navigation, painting, and resize.
LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Message switch key: routes Windows messages to their handlers.
    switch (message)
    {
        // Create case key: creates the native interface controls.
        case WM_CREATE:
            createControls(window);
            return 0;

        // Resize case key: repositions the controls to fit the window.
        case WM_SIZE:
            layoutControls(window);
            InvalidateRect(window, NULL, TRUE);
            return 0;

        // Erase case key: leaves background clearing to the double-buffered paint pass.
        case WM_ERASEBKGND:
            return 1;

        // Owner-draw case key: paints navigation, action, reset, and Delete buttons.
        case WM_DRAWITEM:
            drawOwnerButton((DRAWITEMSTRUCT*)lParam);
            return TRUE;

        // Command case key: handles navigation, expense actions, and budget settings.
        case WM_COMMAND:
            // Type selector key: refreshes categories and action color after a type change.
            if (LOWORD(wParam) == 204 && HIWORD(wParam) == CBN_SELCHANGE)
            {
                updateCategoryOptions(window);
                return 0;
            }

            // Delete command key: routes a row button to the shared deletion function.
            if (LOWORD(wParam) >= 300 && LOWORD(wParam) < 300 + MAX_TRANSACTIONS)
            {
                int entryIndex = LOWORD(wParam) - 300;
                deleteEntry(entryTitles, entryAmounts, entryCategories, entryTypes,
                            entryCount, entryIndex);
                layoutControls(window);
                InvalidateRect(window, NULL, TRUE);
                return 0;
            }

            // Switch key: routes control IDs to the matching action.
            switch (LOWORD(wParam))
            {
                // Case key: opens Dashboard.
                case 100:
                    setTab(window, 0);
                    return 0;
                // Case key: opens Analytics.
                case 101:
                    setTab(window, 1);
                    return 0;
                // Case key: opens Budget Settings.
                case 102:
                    setTab(window, 2);
                    return 0;
                // Case key: adds the selected income or expense transaction.
                case 110:
                    addEntryFromControls(window);
                    return 0;
                // Case key: applies the entered budget.
                case 111:
                    updateBudgetFromControl(window);
                    return 0;
                // Case key: clears the active income and expense entries.
                case 112:
                    entryCount = 0;
                    layoutControls(window);
                    InvalidateRect(window, NULL, TRUE);
                    return 0;
            }
            break;

        // Paint case key: draws the current banking-style white interface.
        case WM_PAINT:
            paintWindow(window);
            return 0;

        // Edit color case key: gives native edit controls white backgrounds and navy text.
        case WM_CTLCOLOREDIT:
        // Combo color case key: gives the category list a white background and navy text.
        case WM_CTLCOLORLISTBOX:
        // Static color case key: keeps native static labels readable on white.
        case WM_CTLCOLORSTATIC:
            SetTextColor((HDC)wParam, RGB(37, 48, 68));
            SetBkColor((HDC)wParam, RGB(255, 255, 255));
            return (LRESULT)GetStockObject(WHITE_BRUSH);

        // Destroy case key: stops the application message loop.
        case WM_DESTROY:
            // Font cleanup step: releases cached GDI fonts before the window closes.
            deleteFonts();
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(window, message, wParam, lParam);
}
