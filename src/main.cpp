#include <iostream>
#include <string>
#include <limits>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <cctype>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <cstdio>

void clearScreen() {
    std::cout << "\033[2J\033[H" << std::flush;
}

void waitForEnter() {
    std::cout << "\nPress Enter to continue...";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void checkCin() {
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Wrong Input" << std::endl;
    }
}

std::string sepLine() {
    return std::string(40, '=');
}

class Book {
private:
    std::string title;
    std::string author;
    int year;
    double rating;
    bool haveRating;
    std::string pass;

public:
    Book(const std::string& t = "", const std::string& a = "", const std::string p = "", int y = 0)
        : title(t), author(a), year(y), pass(p), rating(0.0), haveRating(false) {
    }

    std::string getAuthor() const { return author; }
    std::string getTitle() const { return title; }
    int getYear() const { return year; }
    double getRating() const { return rating; }
    std::string getPass() const { return pass; }

    void setAuthor(const std::string& n_a) { author = n_a; }
    void setTitle(const std::string& n_t) { title = n_t; }
    void setYear(int n_y) { year = n_y; }
    void setPass(const std::string& n_p) { pass = n_p; }
    bool hasRating() const { return haveRating; }

    void setRating(double r) {
        if (r >= 0.0 && r <= 5.0) {
            rating = r;
            haveRating = true;
            std::cout << "Rating Success" << std::endl;
        }
        else {
            std::cout << "Invalid Choice, Please Enter a rating Between 0.0 - 5.0" << std::endl;
        }
    }

    bool isClassic() const { return year < 1950; }

    void display() const {
        std::cout << "Title: " << title << std::endl;
        std::cout << "Author: " << author << std::endl;
        std::cout << "Year: " << year << std::endl;
        std::cout << "Rating: ";
        if (haveRating) {
            std::cout << std::fixed << std::setprecision(1) << rating;
        }
        else {
            std::cout << "None";
        }
        std::cout << std::endl;
        std::cout << "Status: " << (isClassic() ? "CLASSIC!" : "Not a classic") << std::endl;
    }

    std::string toCSV() const {
        std::stringstream ss;
        ss << "\"" << title << "\","
            << "\"" << author << "\","
            << year << ","
            << rating << ","
            << (haveRating ? 1 : 0) << ","
            << "\"" << pass << "\"";
        return ss.str();
    }

    static Book fromCSV(const std::string& line) {
        std::stringstream ss(line);
        std::string title, author, pass, token;
        int year;
        double rating;
        int hasRating;

        std::getline(ss, token, ',');
        if (token.length() >= 2) {
            title = token.substr(1, token.length() - 2);
        }
        else {
            title = "";
        }

        std::getline(ss, token, ',');
        if (token.length() >= 2) {
            author = token.substr(1, token.length() - 2);
        }
        else {
            author = "";
        }

        std::getline(ss, token, ',');
        year = std::stoi(token);

        std::getline(ss, token, ',');
        rating = std::stod(token);

        std::getline(ss, token, ',');
        hasRating = std::stoi(token);

        std::getline(ss, token, ',');
        if (token.length() >= 2) {
            pass = token.substr(1, token.length() - 2);
        }
        else {
            pass = "";
        }

        Book book(title, author, pass, year);
        if (hasRating == 1) {
            book.setRating(rating);
        }
        return book;
    }
};

class LibraryManager {
private:
    std::vector<Book> books;
    std::string filename;
    std::string format;

    void displayBookList() const {
        if (books.empty()) {
            std::cout << "No books in the library." << std::endl;
            return;
        }

        std::cout << "\nBook List (" << books.size() << " books):" << std::endl;
        for (size_t i = 0; i < books.size(); ++i) {
            std::cout << "\nBook #" << (i + 1) << ":" << std::endl;
            books[i].display();
        }
        std::cout << std::endl;
    }

    bool saveCSV() const {
        try {
            std::ofstream file;
            file.open(filename, std::ios::out | std::ios::trunc);

            if (!file.is_open()) {
                std::cerr << "Error: Could not open file for writing!" << std::endl;
                return false;
            }

            file << "Title,Author,Year,Rating,HasRating,Password" << std::endl;
            for (const Book& b : books) {
                file << b.toCSV() << std::endl;
                if (file.fail()) {
                    std::cerr << "Error: Failed to write to file!" << std::endl;
                    file.close();
                    return false;
                }
            }

            file.close();
            std::cout << "Saved " << books.size() << " books to CSV file: " << filename << std::endl;
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Exception during save: " << e.what() << std::endl;
            return false;
        }
    }

    bool loadCSV() {
        try {
            std::ifstream file(filename);
            if (!file.is_open()) {
                return false;
            }

            if (file.peek() == std::ifstream::traits_type::eof()) {
                file.close();
                std::cout << "File is empty." << std::endl;
                return false;
            }

            books.clear();

            std::string line;
            std::getline(file, line);

            int count = 0;
            while (std::getline(file, line)) {
                if (line.empty()) continue;
                try {
                    Book book = Book::fromCSV(line);
                    books.push_back(book);
                    count++;
                }
                catch (const std::exception& e) {
                    std::cout << "Warning: Skipping invalid CSV line: " << e.what() << std::endl;
                }
            }

            file.close();
            std::cout << "Loaded " << count << " books from CSV file." << std::endl;
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Exception during load: " << e.what() << std::endl;
            return false;
        }
    }

    bool saveFormatted() const {
        try {
            std::ofstream file;
            file.open(filename, std::ios::out | std::ios::trunc);

            if (!file.is_open()) {
                std::cerr << "Error: Could not open file for writing!" << std::endl;
                return false;
            }

            file << books.size() << std::endl;
            for (const Book& b : books) {
                file << "Title: " << b.getTitle() << std::endl;
                file << "Author: " << b.getAuthor() << std::endl;
                file << "Year: " << b.getYear() << std::endl;
                file << "Rating: " << b.getRating() << std::endl;
                file << "Has Rating: " << (b.hasRating() ? 1 : 0) << std::endl;
                file << "Password: " << b.getPass() << std::endl;
                file << "========================" << std::endl;

                if (file.fail()) {
                    std::cerr << "Error: Failed to write to file!" << std::endl;
                    file.close();
                    return false;
                }
            }

            file.close();
            std::cout << "Saved " << books.size() << " books to formatted file: " << filename << std::endl;
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Exception during save: " << e.what() << std::endl;
            return false;
        }
    }

    bool loadFormatted() {
        try {
            std::ifstream file(filename);
            if (!file.is_open()) {
                return false;
            }

            if (file.peek() == std::ifstream::traits_type::eof()) {
                file.close();
                std::cout << "File is empty." << std::endl;
                return false;
            }

            int count;
            if (!(file >> count)) {
                file.close();
                std::cout << "File is corrupted." << std::endl;
                return false;
            }

            if (count < 0 || count > 10000) {
                file.close();
                std::cout << "Invalid book count." << std::endl;
                return false;
            }

            file.ignore();
            books.clear();

            for (int i = 0; i < count; i++) {
                std::string line;
                std::string title, author, pass;
                int year = 0;
                double rating = 0.0;
                int hasRating = 0;

                if (!std::getline(file, line)) break;
                if (line.find("Title: ") != std::string::npos) {
                    title = line.substr(7);
                }
                else {
                    break;
                }

                if (!std::getline(file, line)) break;
                if (line.find("Author: ") != std::string::npos) {
                    author = line.substr(8);
                }
                else {
                    break;
                }

                if (!std::getline(file, line)) break;
                if (line.find("Year: ") != std::string::npos) {
                    year = std::stoi(line.substr(6));
                }
                else {
                    break;
                }

                if (!std::getline(file, line)) break;
                if (line.find("Rating: ") != std::string::npos) {
                    rating = std::stod(line.substr(8));
                }
                else {
                    break;
                }

                if (!std::getline(file, line)) break;
                if (line.find("Has Rating: ") != std::string::npos) {
                    hasRating = std::stoi(line.substr(12));
                }
                else {
                    break;
                }

                if (!std::getline(file, line)) break;
                if (line.find("Password: ") != std::string::npos) {
                    pass = line.substr(10);
                }
                else {
                    break;
                }

                std::getline(file, line);

                Book book(title, author, pass, year);
                if (hasRating == 1) {
                    book.setRating(rating);
                }
                books.push_back(book);
            }

            file.close();
            std::cout << "Loaded " << books.size() << " books from formatted file." << std::endl;
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Exception during load: " << e.what() << std::endl;
            return false;
        }
    }

public:
    LibraryManager(const std::string& fname = "library_data.txt", const std::string& fmt = "formatted")
        : filename(fname), format(fmt) {
        std::cout << "Loading library data..." << std::endl;
        if (!loadData() || books.empty()) {
            std::cout << "No books found. Creating default library..." << std::endl;
            books.push_back(Book("1984", "George Orwell", "George_1949", 1949));
            books.push_back(Book("The Hobbit", "J.R.R. Tolkien", "Toky_toky", 1937));
            books.push_back(Book("The Catcher in the Rye", "J.D. Salinger", "Kill_J.D", 1951));
        }
        std::cout << "Loaded: " << books.size() << " books." << std::endl;
    }

    ~LibraryManager() {
        std::cout << "\nAuto-saving library data..." << std::endl;
        saveData();
        std::cout << "Goodbye!" << std::endl;
    }

    bool saveData() const {
        if (format == "csv") {
            return saveCSV();
        }
        else {
            return saveFormatted();
        }
    }

    bool loadData() {
        if (format == "csv") {
            return loadCSV();
        }
        else {
            return loadFormatted();
        }
    }

    void chooseFormat() {
        std::string sep = sepLine();
        std::cout << sep << std::endl;
        std::cout << "Choose File Format" << std::endl;
        std::cout << sep << std::endl;
        std::cout << "1. Formatted (Human-readable)" << std::endl;
        std::cout << "2. CSV (Comma Separated Values)" << std::endl;
        std::cout << sep << std::endl;
        std::cout << "Current format: " << format << std::endl;
        std::cout << "Choose: ";

        int choice;
        std::cin >> choice;
        checkCin();

        if (choice == 1) {
            format = "formatted";
            std::cout << "Format changed to: Formatted" << std::endl;
            if (!filename.empty() && filename.find(".csv") != std::string::npos) {
                filename = filename.substr(0, filename.find(".csv")) + ".txt";
                std::cout << "Filename changed to: " << filename << std::endl;
            }
        }
        else if (choice == 2) {
            format = "csv";
            std::cout << "Format changed to: CSV" << std::endl;
            if (!filename.empty() && filename.find(".txt") != std::string::npos) {
                filename = filename.substr(0, filename.find(".txt")) + ".csv";
                std::cout << "Filename changed to: " << filename << std::endl;
            }
        }
        else {
            std::cout << "Invalid choice. Format unchanged." << std::endl;
        }
    }

    void chooseFilename() {
        std::string newFilename;
        std::cout << "Enter new filename (including extension): ";
        std::cin >> newFilename;

        if (!newFilename.empty()) {
            filename = newFilename;
            std::cout << "Filename changed to: " << filename << std::endl;
        }
        else {
            std::cout << "Invalid filename. Keeping current: " << filename << std::endl;
        }
    }

    bool manualSave() const {
        std::cout << "Attempting to save data..." << std::endl;
        bool result = saveData();
        if (result) {
            std::cout << "Save operation completed successfully." << std::endl;
        }
        else {
            std::cout << "Save operation failed!" << std::endl;
        }
        return result;
    }

    bool manualLoad() {
        std::cout << "Attempting to load data..." << std::endl;
        bool result = loadData();
        if (result) {
            std::cout << "Load operation completed successfully." << std::endl;
            if (books.empty()) {
                std::cout << "Library is now empty." << std::endl;
            }
        }
        else {
            std::cout << "Load operation failed!" << std::endl;
        }
        return result;
    }

    void reset() {
        if (books.empty()) {
            std::cout << "Library is already empty!" << std::endl;
            return;
        }

        books.clear();
        std::cout << "Library has been reset. All books removed!" << std::endl;
    }

    void addBook() {
        std::string title, author, password;
        int year;

        std::cout << "Book Title: ";
        std::cin >> title;
        std::cout << "Book Author: ";
        std::cin >> author;
        std::cout << "Password: ";
        std::cin >> password;
        std::cout << "Book Year (Creation): ";
        std::cin >> year;

        if (std::cin.fail() || year < 1500 || year > 2026) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid Input, Enter A Year From 1500 to 2026" << std::endl;
        }
        else {
            Book newBook(title, author, password, year);
            books.push_back(newBook);
            std::cout << "\nBook created successfully!" << std::endl;
            newBook.display();
            std::cout << std::endl;
        }
    }

    void removeBook() {
        if (books.empty()) {
            std::cout << "No books available to remove." << std::endl;
            return;
        }

        displayBookList();

        int choice;
        std::cout << "Enter book number to remove (0 to cancel): ";
        std::cin >> choice;
        checkCin();

        if (choice == 0) {
            std::cout << "Removal cancelled." << std::endl;
            return;
        }

        if (choice >= 1 && choice <= static_cast<int>(books.size())) {
            char confirm;
            std::cout << "Are you sure you want to remove '"
                << books[choice - 1].getTitle() << "'? (y/n): ";
            std::cin >> confirm;

            if (confirm == 'y' || confirm == 'Y') {
                books.erase(books.begin() + choice - 1);
                std::cout << "Book removed successfully!" << std::endl;
            }
            else {
                std::cout << "Removal cancelled." << std::endl;
            }
        }
        else {
            std::cout << "Invalid selection!" << std::endl;
        }
    }

    void searchBook() const {
        if (books.empty()) {
            std::cout << "No books available to search." << std::endl;
            return;
        }
        bool isExit = false;
        while (!isExit) {
            std::string sep = sepLine();
            int choice;

            std::cout << sep << std::endl;
            std::cout << "=== Library Search ===" << std::endl;
            std::cout << sep << std::endl;
            std::cout << "1. By Title" << std::endl;
            std::cout << "2. By Author" << std::endl;
            std::cout << "3. Year" << std::endl;
            std::cout << "4. Exit" << std::endl;
            std::cout << sep << std::endl;
            std::cout << "Choose:";
            std::cin >> choice;
            checkCin();

            if (choice == 1) {
                std::string search_title;
                std::cout << "Enter the title of the book to find: ";
                std::cin >> search_title;

                bool found = false;
                for (size_t i = 0; i < books.size(); ++i) {
                    if (books[i].getTitle() == search_title) {
                        std::cout << "\nBook found at position #" << (i + 1) << ":" << std::endl;
                        books[i].display();
                        found = true;
                    }
                }

                if (!found) {
                    std::cout << "No book found with the title: " << search_title << std::endl;
                }
                isExit = true;
            }
            else if (choice == 2) {
                std::string search_author;
                std::cout << "Enter the author of the book to find: ";
                std::cin >> search_author;

                bool found = false;
                for (size_t i = 0; i < books.size(); ++i) {
                    if (books[i].getAuthor() == search_author) {
                        std::cout << "\nBook found at position #" << (i + 1) << ":" << std::endl;
                        books[i].display();
                        found = true;
                    }
                }

                if (!found) {
                    std::cout << "No book found with the author: " << search_author << std::endl;
                }
                isExit = true;
            }
            else if (choice == 3) {
                int search_year;
                std::cout << "Enter the year of the book to find: ";
                std::cin >> search_year;
                checkCin();
                bool found = false;
                for (size_t i = 0; i < books.size(); ++i) {
                    if (books[i].getYear() == search_year) {
                        std::cout << "\nBook found at position #" << (i + 1) << ":" << std::endl;
                        books[i].display();
                        found = true;
                    }
                }

                if (!found) {
                    std::cout << "No book found with the year: " << search_year << std::endl;
                }
                isExit = true;
            }
            else if (choice == 4) {
                isExit = true;
            }
            else {
                std::cout << "Invalid Choice" << std::endl;
            }
            std::cout << std::endl;
        }
    }

    void sortBooks() {
        if (books.empty()) {
            std::cout << "No books available to sort." << std::endl;
            return;
        }

        std::string sep = sepLine();
        int choice;
        std::cout << sep << std::endl;
        std::cout << "=== Library Sorting ===" << std::endl;
        std::cout << sep << std::endl;
        std::cout << "1. By Title (A-Z)" << std::endl;
        std::cout << "2. By Title (Z-A)" << std::endl;
        std::cout << "3. By Author (A-Z)" << std::endl;
        std::cout << "4. By Author (Z-A)" << std::endl;
        std::cout << "5. By Year (Oldest first)" << std::endl;
        std::cout << "6. By Year (Newest first)" << std::endl;
        std::cout << "7. By Rating (Highest first)" << std::endl;
        std::cout << "8. By Rating (Lowest first)" << std::endl;
        std::cout << '\n' << sep << std::endl;
        std::cout << "Choose: ";
        std::cin >> choice;
        checkCin();

        switch (choice) {
        case 1:
            std::sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
                return a.getTitle() < b.getTitle();
                });
            std::cout << "\nBooks sorted by Title (A-Z)" << std::endl;
            break;
        case 2:
            std::sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
                return a.getTitle() > b.getTitle();
                });
            std::cout << "\nBooks sorted by Title (Z-A)" << std::endl;
            break;
        case 3:
            std::sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
                return a.getAuthor() < b.getAuthor();
                });
            std::cout << "\nBooks sorted by Author (A-Z)" << std::endl;
            break;
        case 4:
            std::sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
                return a.getAuthor() > b.getAuthor();
                });
            std::cout << "\nBooks sorted by Author (Z-A)" << std::endl;
            break;
        case 5:
            std::sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
                return a.getYear() < b.getYear();
                });
            std::cout << "\nBooks sorted by Year (Oldest first)" << std::endl;
            break;
        case 6:
            std::sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
                return a.getYear() > b.getYear();
                });
            std::cout << "\nBooks sorted by Year (Newest first)" << std::endl;
            break;
        case 7:
            std::sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
                if (a.hasRating() && !b.hasRating()) return true;
                if (!a.hasRating() && b.hasRating()) return false;
                return a.getRating() > b.getRating();
                });
            std::cout << "\nBooks sorted by Rating (Highest first)" << std::endl;
            break;
        case 8:
            std::sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
                if (a.hasRating() && !b.hasRating()) return true;
                if (!a.hasRating() && b.hasRating()) return false;
                return a.getRating() < b.getRating();
                });
            std::cout << "\nBooks sorted by Rating (Lowest first)" << std::endl;
            break;
        default:
            std::cout << "Invalid choice. No sorting applied." << std::endl;
            return;
        }

        displayBookList();
    }

    void showAllBooks() const {
        displayBookList();
    }

    void showStatistics() const {
        if (books.empty()) {
            std::cout << "No books in the library." << std::endl;
            return;
        }

        int totalBooks = books.size();
        int classicBooks = 0;
        int ratedBooks = 0;
        double totalRating = 0.0;

        for (const Book& b : books) {
            if (b.isClassic()) classicBooks++;
            if (b.hasRating()) {
                totalRating += b.getRating();
                ratedBooks++;
            }
        }

        double averageRating = (ratedBooks > 0) ? totalRating / ratedBooks : 0.0;

        std::cout << "\n=== Library Statistics ===" << std::endl;
        std::cout << "Total Books: " << totalBooks << std::endl;
        std::cout << "Classic Books: " << classicBooks << std::endl;
        std::cout << "Books with Ratings: " << ratedBooks << std::endl;
        std::cout << "Average Rating: " << std::fixed << std::setprecision(1)
            << averageRating << std::endl;
        std::cout << std::endl;
    }

    void setBookRating() {
        if (books.empty()) {
            std::cout << "No books available. Please create a book first." << std::endl;
            return;
        }

        std::cout << "\nAvailable Books:" << std::endl;
        for (size_t i = 0; i < books.size(); ++i) {
            std::cout << i + 1 << ". " << books[i].getTitle()
                << " by " << books[i].getAuthor() << std::endl;
        }

        int bookIndex;
        std::cout << "\nSelect book number (1-" << books.size() << "): ";
        std::cin >> bookIndex;
        checkCin();

        if (bookIndex < 1 || bookIndex > static_cast<int>(books.size())) {
            std::cout << "Invalid selection!" << std::endl;
            return;
        }

        double newR;
        std::cout << "Enter the rating (0.0 - 5.0): ";
        std::cin >> newR;
        checkCin();

        if (newR >= 0.0 && newR <= 5.0) {
            books[bookIndex - 1].setRating(newR);
            std::cout << "\nUpdated Book Information:" << std::endl;
            books[bookIndex - 1].display();
            std::cout << std::endl;
        }
        else {
            std::cout << "Invalid rating!" << std::endl;
        }
    }

    void showClassics() const {
        std::cout << "\n=== Classic Books (Before 1950) ===" << std::endl;
        bool foundClassic = false;
        for (size_t i = 0; i < books.size(); ++i) {
            if (books[i].isClassic()) {
                foundClassic = true;
                std::cout << books[i].getTitle() << " by "
                    << books[i].getAuthor() << " ("
                    << books[i].getYear() << ")" << std::endl;
            }
        }
        if (!foundClassic) {
            std::cout << "No classic books in the library." << std::endl;
        }
        std::cout << std::endl;
    }

    void modifyBook() {
        if (books.empty()) {
            std::cout << "No Books Available" << std::endl;
            return;
        }

        bool goBack = false;
        while (!goBack) {
            std::cout << "\n=== Select Book to Modify ===" << std::endl;
            for (size_t i = 0; i < books.size(); i++) {
                std::cout << (i + 1) << ". " << books[i].getTitle()
                    << " by " << books[i].getAuthor()
                    << " (" << books[i].getYear() << ")" << std::endl;
            }

            int choice;
            std::cout << "\nChoose a book number (0 to cancel): ";
            std::cin >> choice;
            checkCin();

            if (choice == 0) {
                std::cout << "Modification cancelled." << std::endl;
                goBack = true;
                break;
            }

            if (choice >= 1 && choice <= static_cast<int>(books.size())) {
                int targetIndex = choice - 1;
                std::string inputPassword;

                std::cout << "Enter The Password (type 'Quit' to go back): ";
                std::cin >> inputPassword;

                if (inputPassword == "Quit" || inputPassword == "quit") {
                    std::cout << "Modification cancelled." << std::endl;
                    goBack = true;
                    break;
                }

                if (inputPassword == books[targetIndex].getPass()) {
                    std::cout << "\nAccess Granted! Choose what to modify:" << std::endl;

                    std::string newTitle, newAuthor, newPass;
                    int newYear;
                    int g_choice;

                    std::cout << "1. Set New Title" << std::endl;
                    std::cout << "2. Set New Author" << std::endl;
                    std::cout << "3. Set New Year" << std::endl;
                    std::cout << "4. Set New Password" << std::endl;
                    std::cout << "5. Delete The Book" << std::endl;
                    std::cout << "6. Back" << std::endl;
                    std::cout << "Choose: ";
                    std::cin >> g_choice;
                    checkCin();

                    if (g_choice == 1) {
                        std::cout << "Set New Title: ";
                        std::cin >> newTitle;
                        books[targetIndex].setTitle(newTitle);
                        std::cout << "Title updated successfully!" << std::endl;
                        goBack = true;
                    }
                    else if (g_choice == 2) {
                        std::cout << "Set New Author: ";
                        std::cin >> newAuthor;
                        books[targetIndex].setAuthor(newAuthor);
                        std::cout << "Author updated successfully!" << std::endl;
                        goBack = true;
                    }
                    else if (g_choice == 3) {
                        std::cout << "Set New Year: ";
                        std::cin >> newYear;
                        if (std::cin.fail() || newYear < 1500 || newYear > 2026) {
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            std::cout << "Invalid Year!" << std::endl;
                        }
                        else {
                            books[targetIndex].setYear(newYear);
                            std::cout << "Year updated successfully!" << std::endl;
                            goBack = true;
                        }
                    }
                    else if (g_choice == 4) {
                        bool isPassCorrect = false;
                        while (!isPassCorrect) {
                            std::string currentPass;
                            std::string g_newPass;
                            std::string Password = books[targetIndex].getPass();

                            std::cout << "Current Password (or type 'Quit' to cancel): ";
                            std::cin >> currentPass;

                            if (currentPass == "Quit" || currentPass == "quit") {
                                std::cout << "Password change cancelled." << std::endl;
                                break;
                            }

                            if (currentPass == Password) {
                                std::cout << "Set New Password: ";
                                std::cin >> newPass;
                                std::cout << "Confirm New Password: ";
                                std::cin >> g_newPass;
                                if (g_newPass == newPass) {
                                    books[targetIndex].setPass(newPass);
                                    std::cout << "Password updated successfully!" << std::endl;
                                    isPassCorrect = true;
                                    goBack = true;
                                }
                                else {
                                    std::cout << "Incorrect Confirmation! Try again." << std::endl;
                                }
                            }
                            else {
                                std::cout << "Incorrect Current Password!" << std::endl;
                            }
                        }
                    }
                    else if (g_choice == 5) {
                        char confirm;
                        std::cout << "Are you sure you want to delete '"
                            << books[targetIndex].getTitle() << "'? (y/n): ";
                        std::cin >> confirm;

                        if (confirm == 'y' || confirm == 'Y') {
                            books.erase(books.begin() + targetIndex);
                            std::cout << "Book deleted successfully!" << std::endl;
                            goBack = true;
                        }
                        else {
                            std::cout << "Deletion cancelled." << std::endl;
                        }
                    }
                    else if (g_choice == 6) {
                        goBack = true;
                    }
                    else {
                        std::cout << "Invalid choice!" << std::endl;
                    }
                }
                else {
                    std::cout << "Wrong Password! Access Denied." << std::endl;
                }
            }
            else {
                std::cout << "Invalid book selection!" << std::endl;
            }
        }
    }

    void showMenu() const {
        std::string sep = sepLine();
        std::cout << sep << std::endl;
        std::cout << "Book Management System" << std::endl;
        std::cout << sep << std::endl;
        std::cout << "1. Add Book" << std::endl;
        std::cout << "2. Remove Book" << std::endl;
        std::cout << "3. Show All Books" << std::endl;
        std::cout << "4. Search Book" << std::endl;
        std::cout << "5. Sort Books" << std::endl;
        std::cout << "6. Set Rating" << std::endl;
        std::cout << "7. Show Classics" << std::endl;
        std::cout << "8. Show Statistics" << std::endl;
        std::cout << "9. Modify Book" << std::endl;
        std::cout << "10. Save Data" << std::endl;
        std::cout << "11. Load Data" << std::endl;
        std::cout << "12. Choose File Format" << std::endl;
        std::cout << "13. Choose Filename" << std::endl;
        std::cout << "14. RESET Library" << std::endl;
        std::cout << "15. Exit" << std::endl;
        std::cout << sep << std::endl;
        std::cout << "Books in Library: " << books.size() << std::endl;
        std::cout << "File: " << filename << std::endl;
        std::cout << "Format: " << format << std::endl;
        std::cout << sep << std::endl;
        std::cout << "Choose: ";
    }

    void run() {
        bool isExit = false;
        while (!isExit) {
            showMenu();
            int choice;
            std::cin >> choice;
            checkCin();

            switch (choice) {
            case 1: addBook(); break;
            case 2: removeBook(); break;
            case 3: showAllBooks(); break;
            case 4: searchBook(); break;
            case 5: sortBooks(); break;
            case 6: setBookRating(); break;
            case 7: showClassics(); break;
            case 8: showStatistics(); break;
            case 9: modifyBook(); break;
            case 10:
                if (manualSave()) {
                    std::cout << "Operation Successful" << std::endl;
                }
                else {
                    std::cout << "An error has occurred" << std::endl;
                }
                break;
            case 11:
                if (manualLoad()) {
                    std::cout << "Operation Successful" << std::endl;
                }
                else {
                    std::cout << "An error has occurred" << std::endl;
                }
                break;
            case 12:
                chooseFormat();
                break;
            case 13:
                chooseFilename();
                break;
            case 14: {
                std::string resetPass = "RESET_ALERT!!!@";
                std::string guess;
                std::cout << "Enter Secret password: ";
                std::cin >> guess;

                if (guess == resetPass) {
                    char confirm;
                    std::cout << "WARNING: This will delete ALL books! Continue? (y/n): ";
                    std::cin >> confirm;

                    if (confirm == 'y' || confirm == 'Y') {
                        reset();
                    }
                    else {
                        std::cout << "Reset cancelled." << std::endl;
                    }
                }
                else {
                    std::cout << "Incorrect password! Access denied." << std::endl;
                }
                break;
            }
            case 15:
                std::cout << "Goodbye!" << std::endl;
                isExit = true;
                break;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                break;
            }
        }
    }
};

class Character {
private:
    std::string name;
    std::string classType;
    int level;
    int xp;
    int xpNextLevel;
    std::vector<std::string> inventory;
    std::vector<std::string> skills;
    int money;
    int energy;

public:
    Character() : level(1), xp(0), xpNextLevel(100), money(100), energy(200) {}

    void setName(const std::string& n) { name = n; }

    void setClass(const std::string& c) {
        classType = c;
        giveStartingSkills();
    }

    void giveStartingSkills() {
        skills.clear();

        if (classType == "Warrior" || classType == "warrior") {
            skills.push_back("Slash (Level 1)");
        }
        else if (classType == "Mage" || classType == "mage") {
            skills.push_back("Fire Ball (Level 1)");
        }
        else if (classType == "Rogue" || classType == "rogue") {
            skills.push_back("Stealth Strike (Level 1)");
        }
    }

    void checkAndUnlockSkills() {
        if (classType == "Warrior" || classType == "warrior") {
            if (level >= 3 && !hasSkill("Shield Bash (Level 3)")) {
                skills.push_back("Shield Bash (Level 3)");
                std::cout << "You learned SHIELD BASH! (Level 3)" << std::endl;
                waitForEnter();
            }
            if (level >= 5 && !hasSkill("Whirlwind (Level 5)")) {
                skills.push_back("Whirlwind (Level 5)");
                std::cout << "You learned WHIRLWIND! (Level 5)" << std::endl;
                waitForEnter();
            }
            if (level >= 8 && !hasSkill("War Cry (Level 8)")) {
                skills.push_back("War Cry (Level 8)");
                std::cout << "You learned WAR CRY! (Level 8)" << std::endl;
                waitForEnter();
            }
        }
        else if (classType == "Mage" || classType == "mage") {
            if (level >= 3 && !hasSkill("Ice Shield (Level 3)")) {
                skills.push_back("Ice Shield (Level 3)");
                std::cout << "You learned ICE SHIELD! (Level 3)" << std::endl;
                waitForEnter();
            }
            if (level >= 5 && !hasSkill("Lightning Strike (Level 5)")) {
                skills.push_back("Lightning Strike (Level 5)");
                std::cout << "You learned LIGHTNING STRIKE! (Level 5)" << std::endl;
                waitForEnter();
            }
            if (level >= 8 && !hasSkill("Meteor Storm (Level 8)")) {
                skills.push_back("Meteor Storm (Level 8)");
                std::cout << "You learned METEOR STORM! (Level 8)" << std::endl;
                waitForEnter();
            }
        }
        else if (classType == "Rogue" || classType == "rogue") {
            if (level >= 3 && !hasSkill("Poison Dagger (Level 3)")) {
                skills.push_back("Poison Dagger (Level 3)");
                std::cout << "You learned POISON DAGGER! (Level 3)" << std::endl;
                waitForEnter();
            }
            if (level >= 5 && !hasSkill("Shadow Step (Level 5)")) {
                skills.push_back("Shadow Step (Level 5)");
                std::cout << "You learned SHADOW STEP! (Level 5)" << std::endl;
                waitForEnter();
            }
            if (level >= 8 && !hasSkill("Assassinate (Level 8)")) {
                skills.push_back("Assassinate (Level 8)");
                std::cout << "You learned ASSASSINATE! (Level 8)" << std::endl;
                waitForEnter();
            }
        }
    }

    void Cheat_menu(const std::string sep) {
        bool isBack = false;
        while (!isBack) {
            clearScreen();
            std::cout << sep << std::endl;
            std::cout << "Cheat Menu" << std::endl;
            std::cout << sep << std::endl;

            std::string pass;
            std::cout << "Enter The Password: ";
            std::cin >> pass;

            if (pass == "cheat_code") {
                bool inCheatSubMenu = true;
                while (inCheatSubMenu) {
                    clearScreen();
                    std::cout << sep << std::endl;
                    std::cout << "Cheat Options" << std::endl;
                    std::cout << sep << std::endl;
                    std::cout << "1. Give Level (+1 Level)" << std::endl;
                    std::cout << "2. Give XP (+1000 XP)" << std::endl;
                    std::cout << "3. Give All Skills" << std::endl;
                    std::cout << "4. Give All Items" << std::endl;
                    std::cout << "5. Back to Main Menu" << std::endl;

                    int choice;
                    std::cout << "Choose: ";
                    std::cin >> choice;

                    if (std::cin.fail()) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::cout << "Invalid input!" << std::endl;
                        waitForEnter();
                        continue;
                    }

                    switch (choice) {
                    case 1: {
                        level++;
                        std::cout << "Level increased to " << level << "!" << std::endl;
                        waitForEnter();
                        break;
                    }
                    case 2: {
                        addXP(1000);
                        std::cout << "Added 1000 XP!" << std::endl;
                        waitForEnter();
                        break;
                    }
                    case 3: {
                        if (classType == "Warrior" || classType == "warrior") {
                            if (!hasSkill("Shield Bash (Level 3)")) skills.push_back("Shield Bash (Level 3)");
                            if (!hasSkill("Whirlwind (Level 5)")) skills.push_back("Whirlwind (Level 5)");
                            if (!hasSkill("War Cry (Level 8)")) skills.push_back("War Cry (Level 8)");
                        }
                        else if (classType == "Mage" || classType == "mage") {
                            if (!hasSkill("Ice Shield (Level 3)")) skills.push_back("Ice Shield (Level 3)");
                            if (!hasSkill("Lightning Strike (Level 5)")) skills.push_back("Lightning Strike (Level 5)");
                            if (!hasSkill("Meteor Storm (Level 8)")) skills.push_back("Meteor Storm (Level 8)");
                        }
                        else if (classType == "Rogue" || classType == "rogue") {
                            if (!hasSkill("Poison Dagger (Level 3)")) skills.push_back("Poison Dagger (Level 3)");
                            if (!hasSkill("Shadow Step (Level 5)")) skills.push_back("Shadow Step (Level 5)");
                            if (!hasSkill("Assassinate (Level 8)")) skills.push_back("Assassinate (Level 8)");
                        }
                        std::cout << "All skills unlocked!" << std::endl;
                        waitForEnter();
                        break;
                    }
                    case 4: {
                        if (!hasItem("Sword")) addItem("Sword");
                        if (!hasItem("Shield")) addItem("Shield");
                        if (!hasItem("Potion")) addItem("Potion");
                        if (!hasItem("Heal Potion (+50 hp)")) addItem("Heal Potion (+50 hp)");
                        if (!hasItem("Energy Potion (+30 Energy)")) addItem("Energy Potion (+30 Energy)");
                        if (!hasItem("Boost Potion (+20% Damage)")) addItem("Boost Potion (+20% Damage)");
                        std::cout << "All items added!" << std::endl;
                        waitForEnter();
                        break;
                    }
                    case 5: {
                        inCheatSubMenu = false;
                        isBack = true;
                        break;
                    }
                    default: {
                        std::cout << "Invalid choice!" << std::endl;
                        waitForEnter();
                        break;
                    }
                    }
                }
            }
            else {
                std::cout << "Wrong password!" << std::endl;
                waitForEnter();
                isBack = true;
            }
        }
    }

    std::string getClass() const { return classType; }
    int getLevel() const { return level; }
    int getXP() const { return xp; }
    int getXpNextLevel() const { return xpNextLevel; }
    int getMoney() const { return money; }
    int getEnergy() const { return energy; }
    std::string getName() const { return name; }

    void addXP(int amount) {
        xp += amount;
        checkLevelUp();
    }

    void reset() {
        level = 1;
        xp = 0;
        xpNextLevel = 100;
        classType = "";
        name = "";
        inventory.clear();
        skills.clear();
        money = 100;
        energy = 100;
    }

    void checkLevelUp() {
        while (xp >= xpNextLevel) {
            xp -= xpNextLevel;
            level++;
            xpNextLevel = level * 100;
            std::cout << "\n================================" << std::endl;
            std::cout << " LEVEL UP! You are now Level " << level << std::endl;
            std::cout << "Next level requires: " << xpNextLevel << " XP." << std::endl;
            std::cout << "================================\n" << std::endl;
            checkAndUnlockSkills();
            waitForEnter();
        }
    }

    void addItem(const std::string& item) {
        inventory.push_back(item);
    }

    void removeItem(const std::string& item) {
        auto it = std::find(inventory.begin(), inventory.end(), item);
        if (it != inventory.end()) {
            inventory.erase(it);
        }
    }

    void showInventory() const {
        if (inventory.empty()) {
            std::cout << "Inventory is empty." << std::endl;
            return;
        }
        for (size_t i = 0; i < inventory.size(); i++) {
            std::cout << (i + 1) << ". " << inventory[i] << std::endl;
        }
    }

    void showSkills() const {
        if (skills.empty()) {
            std::cout << "No skills learned yet!" << std::endl;
            return;
        }
        std::cout << "=== Your Skills ===" << std::endl;
        for (size_t i = 0; i < skills.size(); i++) {
            std::cout << (i + 1) << ". " << skills[i] << std::endl;
        }
    }

    bool hasItem(const std::string& item) const {
        return std::find(inventory.begin(), inventory.end(), item) != inventory.end();
    }

    bool hasSkill(const std::string& skill) const {
        return std::find(skills.begin(), skills.end(), skill) != skills.end();
    }

    void spendMoney(int amount) {
        money -= amount;
    }

    void addMoney(int amount) {
        money += amount;
    }

    void useEnergy(int amount) {
        energy -= amount;
        if (energy < 0) energy = 0;
    }

    void restoreEnergy(int amount) {
        energy += amount;
        if (energy > 200) energy = 200;
    }
};

class Game {
private:
    std::string sep;
    Character player;
    std::vector<std::string> battleItems = {
        "Heal Potion (+50 hp)",
        "Energy Potion (+30 Energy)",
        "Boost Potion (+20% Damage)"
    };
    std::vector<int> battleItemPrices = { 30, 20, 50 };

public:
    Game() : sep(40, '=') {}

    bool isValidClass(const std::string& choice) {
        std::string lowerChoice = choice;
        std::transform(lowerChoice.begin(), lowerChoice.end(), lowerChoice.begin(), ::tolower);
        return lowerChoice == "warrior" || lowerChoice == "mage" || lowerChoice == "rogue";
    }

    void welcome() const {
        std::cout << sep << std::endl;
        std::cout << "Welcome To Character Creation !" << std::endl;
        std::cout << sep << std::endl;
        std::cout << "1. Set Character" << std::endl;
        std::cout << "2. Play Game" << std::endl;
        std::cout << "3. Show Stats" << std::endl;
        std::cout << "4. Reset Stats" << std::endl;
        std::cout << "5. Show Inventory" << std::endl;
        std::cout << "6. Buy Items" << std::endl;
        std::cout << "7. Show Skills" << std::endl;
        std::cout << "8. Cheat Menu" << std::endl;
        std::cout << "9. Exit" << std::endl;
    }

    std::string getCharacterName() {
        clearScreen();
        std::string inputName;
        std::cout << "Enter character name: ";
        std::cin >> inputName;
        return inputName;
    }

    std::string getCharacterClass() {
        clearScreen();
        std::string inputClass;
        while (true) {
            std::cout << "Enter class (Warrior/Mage/Rogue): ";
            std::cin >> inputClass;
            if (isValidClass(inputClass)) {
                return inputClass;
            }
            std::cout << "Invalid class!" << std::endl;
            waitForEnter();
        }
    }

    void printCharacter() const {
        clearScreen();
        std::cout << sep << std::endl;
        std::cout << "Player Set" << std::endl;
        std::cout << sep << std::endl;
        std::cout << "Name:  " << player.getName() << std::endl;
        std::cout << "Class: " << player.getClass() << std::endl;
        std::cout << "Level: " << player.getLevel() << std::endl;
        waitForEnter();
    }

    void battle() {
        clearScreen();
        std::srand(std::time(0));
        bool haveBoost = false;
        int playerEnergy = 200;
        float playerHP = 200 + (player.getLevel() * 10);
        float enemyHP = 150 + (std::rand() % 50) + (player.getLevel() * 5);
        int enemyDamage = 15 + (std::rand() % 10) + (player.getLevel() * 2);

        std::cout << sep << std::endl;
        std::cout << "RPG BATTLE!" << std::endl;
        std::cout << sep << std::endl;
        std::cout << "A Zombie appears! (HP: " << enemyHP << ")" << std::endl;
        std::cout << "Your HP: " << playerHP << std::endl;
        std::cout << "Your Energy: " << playerEnergy << std::endl;

        while (playerHP > 0 && enemyHP > 0) {
            std::cout << "\n1. Normal Attack (No Energy)" << std::endl;
            std::cout << "2. Use Skill (Costs Energy)" << std::endl;
            std::cout << "3. Use Battle Item" << std::endl;
            std::cout << "4. Run Away" << std::endl;

            int choice;
            std::cout << "Choose: ";
            std::cin >> choice;

            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid Choice" << std::endl;
                waitForEnter();
                continue;
            }

            if (choice == 1) {
                int damage = 20 + (player.getLevel() * 3) + (std::rand() % 10);

                if (haveBoost) {
                    float boostedDamage = damage * 1.2f;
                    enemyHP -= boostedDamage;
                    std::cout << "You dealt " << boostedDamage << " damage (boosted) to the Zombie!" << std::endl;
                }
                else {
                    enemyHP -= damage;
                    std::cout << "You dealt " << damage << " damage to the Zombie!" << std::endl;
                }

                std::cout << "Zombie HP: " << enemyHP << std::endl;

                if (enemyHP <= 0) {
                    std::cout << "You defeated the Zombie! +50 XP" << std::endl;
                    player.addXP(50);
                    player.addMoney(20);
                    waitForEnter();
                    return;
                }
            }
            else if (choice == 2) {
                std::vector<std::string> availableSkills;
                std::vector<int> skillCosts;
                std::vector<int> skillDamages;
                std::vector<std::string> skillDescriptions;

                std::string classType = player.getClass();

                if (classType == "Warrior" || classType == "warrior") {
                    if (player.hasSkill("Slash (Level 1)")) {
                        availableSkills.push_back("Slash");
                        skillCosts.push_back(10);
                        skillDamages.push_back(30 + player.getLevel() * 2);
                        skillDescriptions.push_back("30-40 damage, costs 10 energy");
                    }
                    if (player.hasSkill("Shield Bash (Level 3)")) {
                        availableSkills.push_back("Shield Bash");
                        skillCosts.push_back(20);
                        skillDamages.push_back(45 + player.getLevel() * 3);
                        skillDescriptions.push_back("45-55 damage, costs 20 energy");
                    }
                    if (player.hasSkill("Whirlwind (Level 5)")) {
                        availableSkills.push_back("Whirlwind");
                        skillCosts.push_back(30);
                        skillDamages.push_back(60 + player.getLevel() * 4);
                        skillDescriptions.push_back("60-75 damage, costs 30 energy");
                    }
                    if (player.hasSkill("War Cry (Level 8)")) {
                        availableSkills.push_back("War Cry");
                        skillCosts.push_back(40);
                        skillDamages.push_back(80 + player.getLevel() * 5);
                        skillDescriptions.push_back("80-100 damage, costs 40 energy");
                    }
                }
                else if (classType == "Mage" || classType == "mage") {
                    if (player.hasSkill("Fire Ball (Level 1)")) {
                        availableSkills.push_back("Fire Ball");
                        skillCosts.push_back(15);
                        skillDamages.push_back(35 + player.getLevel() * 2);
                        skillDescriptions.push_back("35-45 damage, costs 15 energy");
                    }
                    if (player.hasSkill("Ice Shield (Level 3)")) {
                        availableSkills.push_back("Ice Shield");
                        skillCosts.push_back(20);
                        skillDamages.push_back(20);
                        skillDescriptions.push_back("20 damage + Freeze enemy (skip their turn), costs 20 energy");
                    }
                    if (player.hasSkill("Lightning Strike (Level 5)")) {
                        availableSkills.push_back("Lightning Strike");
                        skillCosts.push_back(30);
                        skillDamages.push_back(50 + player.getLevel() * 4);
                        skillDescriptions.push_back("50-65 damage, costs 30 energy");
                    }
                    if (player.hasSkill("Meteor Storm (Level 8)")) {
                        availableSkills.push_back("Meteor Storm");
                        skillCosts.push_back(45);
                        skillDamages.push_back(70 + player.getLevel() * 6);
                        skillDescriptions.push_back("70-90 damage, costs 45 energy");
                    }
                }
                else if (classType == "Rogue" || classType == "rogue") {
                    if (player.hasSkill("Stealth Strike (Level 1)")) {
                        availableSkills.push_back("Stealth Strike");
                        skillCosts.push_back(10);
                        skillDamages.push_back(25 + player.getLevel() * 2);
                        skillDescriptions.push_back("25-35 damage, costs 10 energy");
                    }
                    if (player.hasSkill("Poison Dagger (Level 3)")) {
                        availableSkills.push_back("Poison Dagger");
                        skillCosts.push_back(20);
                        skillDamages.push_back(40 + player.getLevel() * 3);
                        skillDescriptions.push_back("40-50 damage + Poison (5 damage per turn), costs 20 energy");
                    }
                    if (player.hasSkill("Shadow Step (Level 5)")) {
                        availableSkills.push_back("Shadow Step");
                        skillCosts.push_back(25);
                        skillDamages.push_back(55 + player.getLevel() * 4);
                        skillDescriptions.push_back("55-65 damage + Dodge next attack, costs 25 energy");
                    }
                    if (player.hasSkill("Assassinate (Level 8)")) {
                        availableSkills.push_back("Assassinate");
                        skillCosts.push_back(40);
                        skillDamages.push_back(85 + player.getLevel() * 5);
                        skillDescriptions.push_back("85-100 damage, costs 40 energy");
                    }
                }

                if (availableSkills.empty()) {
                    std::cout << "No skills available!" << std::endl;
                    waitForEnter();
                    continue;
                }

                std::cout << "\n=== Your Skills ===" << std::endl;
                for (size_t i = 0; i < availableSkills.size(); i++) {
                    std::cout << (i + 1) << ". " << availableSkills[i]
                        << " - " << skillDescriptions[i] << std::endl;
                }
                std::cout << (availableSkills.size() + 1) << ". Cancel" << std::endl;

                int skillChoice;
                bool validInput = false;

                while (!validInput) {
                    std::cout << "Choose skill: ";
                    std::cin >> skillChoice;

                    if (std::cin.fail()) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::cout << "Invalid input! Please enter a number." << std::endl;
                        continue;
                    }
                    if (skillChoice >= 1 && skillChoice <= static_cast<int>(availableSkills.size() + 1)) {
                        validInput = true;
                    }
                    else {
                        std::cout << "Invalid choice! Please enter a number from 1 to "
                            << (availableSkills.size() + 1) << std::endl;
                    }
                }

                if (skillChoice >= 1 && skillChoice <= static_cast<int>(availableSkills.size())) {
                    int index = skillChoice - 1;

                    if (playerEnergy >= skillCosts[index]) {
                        playerEnergy -= skillCosts[index];

                        if (availableSkills[index] == "Ice Shield") {
                            int damage = skillDamages[index];
                            enemyHP -= damage;
                            std::cout << "You used Ice Shield! Dealt " << damage << " damage and froze the Zombie!" << std::endl;
                            std::cout << "Zombie HP: " << enemyHP << std::endl;

                            if (enemyHP <= 0) {
                                std::cout << "You defeated the Zombie! +50 XP" << std::endl;
                                player.addXP(50);
                                player.addMoney(20);
                                waitForEnter();
                                return;
                            }

                            std::cout << "Zombie is frozen! It cannot attack this turn!" << std::endl;
                            continue;
                        }
                        else if (availableSkills[index] == "Poison Dagger") {
                            int damage = skillDamages[index];
                            enemyHP -= damage;
                            std::cout << "You used Poison Dagger! Dealt " << damage << " damage and poisoned the Zombie!" << std::endl;
                            std::cout << "Zombie HP: " << enemyHP << std::endl;

                            if (enemyHP <= 0) {
                                std::cout << "You defeated the Zombie! +50 XP" << std::endl;
                                player.addXP(50);
                                player.addMoney(20);
                                waitForEnter();
                                return;
                            }

                            int poisonDamage = 5;
                            enemyHP -= poisonDamage;
                            std::cout << "Poison deals " << poisonDamage << " additional damage!" << std::endl;
                            std::cout << "Zombie HP: " << enemyHP << std::endl;
                        }
                        else {
                            int damage = skillDamages[index] + (std::rand() % 10);
                            enemyHP -= damage;
                            std::cout << "You used " << availableSkills[index] << "! Dealt " << damage << " damage!" << std::endl;
                            std::cout << "Zombie HP: " << enemyHP << std::endl;
                        }

                        if (enemyHP <= 0) {
                            std::cout << "You defeated the Zombie! +50 XP" << std::endl;
                            player.addXP(50);
                            player.addMoney(20);
                            waitForEnter();
                            return;
                        }

                        std::cout << "Energy remaining: " << playerEnergy << std::endl;
                    }
                    else {
                        std::cout << "Not enough energy! You need " << skillCosts[index] << " energy." << std::endl;
                        waitForEnter();
                        continue;
                    }
                }
                else {
                    std::cout << "Invalid skill choice!" << std::endl;
                    waitForEnter();
                    continue;
                }
            }
            else if (choice == 3) {
                std::cout << "\n=== Battle Items ===" << std::endl;
                bool hasItems = false;
                for (size_t i = 0; i < battleItems.size(); i++) {
                    if (player.hasItem(battleItems[i])) {
                        hasItems = true;
                        std::cout << (i + 1) << ". " << battleItems[i] << std::endl;
                    }
                }
                if (!hasItems) {
                    std::cout << "No battle items in inventory!" << std::endl;
                    waitForEnter();
                    continue;
                }
                std::cout << (battleItems.size() + 1) << ". Cancel" << std::endl;

                int itemChoice;
                std::cout << "Choose item: ";
                std::cin >> itemChoice;

                if (std::cin.fail()) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid Choice" << std::endl;
                    waitForEnter();
                    continue;
                }

                if (itemChoice == static_cast<int>(battleItems.size() + 1)) {
                    continue;
                }

                if (itemChoice >= 1 && itemChoice <= static_cast<int>(battleItems.size())) {
                    int index = itemChoice - 1;
                    std::string itemName = battleItems[index];

                    if (player.hasItem(itemName)) {
                        if (itemName == "Heal Potion (+50 hp)") {
                            playerHP += 50;
                            std::cout << "You used a Heal Potion! HP restored to " << playerHP << std::endl;
                            player.removeItem(itemName);
                        }
                        else if (itemName == "Energy Potion (+30 Energy)") {
                            playerEnergy += 30;
                            std::cout << "You used an Energy Potion! Energy restored to " << playerEnergy << std::endl;
                            player.removeItem(itemName);
                        }
                        else if (itemName == "Boost Potion (+20% Damage)") {
                            std::cout << "You used a Boost Potion! Damage increased by 20 percent for this battle!" << std::endl;
                            haveBoost = true;
                            player.removeItem(itemName);
                        }
                    }
                    else {
                        std::cout << "You don't have that item!" << std::endl;
                    }
                }
                else {
                    std::cout << "Invalid choice!" << std::endl;
                }
            }
            else if (choice == 4) {
                std::cout << "You ran away!" << std::endl;
                waitForEnter();
                return;
            }
            else {
                std::cout << "Invalid choice!" << std::endl;
                waitForEnter();
                continue;
            }

            if (enemyHP > 0) {
                int enemyAttack = enemyDamage + (std::rand() % 5);
                playerHP -= enemyAttack;
                std::cout << "\nZombie attacks! Dealt " << enemyAttack << " damage!" << std::endl;
                std::cout << "Your HP: " << playerHP << std::endl;

                if (playerHP <= 0) {
                    std::cout << "You were defeated! Game Over!" << std::endl;
                    waitForEnter();
                    return;
                }
            }
        }
    }

    void guessGame() {
        clearScreen();
        std::srand(std::time(0));
        int reward = 30;
        std::cout << sep << std::endl;
        std::cout << "Welcome To The Guess Game" << std::endl;
        std::cout << sep << std::endl;

        int secret = std::rand() % 10 + 1;
        int attempts = 0;

        while (attempts < 3) {
            int guess;
            std::cout << "Guess The Number (1-10): ";
            std::cin >> guess;

            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input! Please enter a number.\n";
                continue;
            }

            if (guess < 1 || guess > 10) {
                std::cout << "Invalid Choice, Please Enter a Number from 1 to 10" << std::endl;
                continue;
            }

            attempts++;
            if (secret < guess) {
                std::cout << "Too High" << std::endl;
            }
            else if (secret > guess) {
                std::cout << "Too low" << std::endl;
            }
            else {
                std::cout << "You Won! Here is 30 XP" << std::endl;
                player.addXP(reward);
                waitForEnter();
                return;
            }
        }
        std::cout << "You used your 3 attempts you lost. The number was " << secret << std::endl;
        waitForEnter();
    }

    void diceGame() {
        clearScreen();
        std::srand(std::time(0));
        std::cout << sep << std::endl;
        std::cout << "Welcome to The Dice Game !" << std::endl;
        std::cout << sep << std::endl;

        while (true) {
            int secret = std::rand() % 6 + 1;
            int guess;
            std::cout << "Guess The Dice Number (1-6): ";
            std::cin >> guess;

            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input! Please enter a number.\n";
                continue;
            }

            if (guess < 1 || guess > 6) {
                std::cout << "Invalid Choice, Please Enter a number from 1 to 6" << std::endl;
                continue;
            }

            if (guess == secret) {
                std::cout << "You Won! Here is 50 XP" << std::endl;
                player.addXP(50);
                waitForEnter();
                return;
            }
            else {
                std::cout << "Wrong try again" << std::endl;
            }
        }
    }

    void playGame() {
        clearScreen();
        int choice;
        std::cout << sep << std::endl;
        std::cout << "Welcome To The Game !" << std::endl;
        std::cout << sep << std::endl;
        std::cout << "1. Guess Game" << std::endl;
        std::cout << "2. Dice Game" << std::endl;
        std::cout << "3. RPG Battle" << std::endl;
        std::cout << "4. Back" << std::endl;
        std::cout << "Choose : ";
        std::cin >> choice;

        switch (choice) {
        case 1: {
            guessGame();
            break;
        }
        case 2: {
            diceGame();
            break;
        }
        case 3: {
            battle();
            break;
        }
        case 4: {
            return;
        }
        default: {
            std::cout << "Invalid Choice" << std::endl;
            waitForEnter();
            break;
        }
        }
    }

    void showStats() const {
        clearScreen();
        std::cout << sep << std::endl;
        std::cout << "Player Statistics : " << std::endl;
        std::cout << sep << std::endl;
        std::cout << "Name : " << player.getName() << std::endl;
        std::cout << "Class : " << player.getClass() << std::endl;
        std::cout << "Level : " << player.getLevel() << std::endl;
        std::cout << "XP : " << player.getXP() << " / " << player.getXpNextLevel() << std::endl;
        std::cout << "Money : $" << player.getMoney() << std::endl;
        std::cout << "Energy : " << player.getEnergy() << std::endl;
        waitForEnter();
    }

    void showSkills() const {
        clearScreen();
        std::cout << sep << std::endl;
        std::cout << " Player Skills " << std::endl;
        std::cout << sep << std::endl;
        player.showSkills();

        std::cout << "\n=== Upcoming Skills ===" << std::endl;
        std::string classType = player.getClass();
        int level = player.getLevel();

        if (classType == "Warrior" || classType == "warrior") {
            if (level < 3) std::cout << "Level 3: Shield Bash" << std::endl;
            if (level < 5) std::cout << "Level 5: Whirlwind" << std::endl;
            if (level < 8) std::cout << "Level 8: War Cry" << std::endl;
        }
        else if (classType == "Mage" || classType == "mage") {
            if (level < 3) std::cout << "Level 3: Ice Shield" << std::endl;
            if (level < 5) std::cout << "Level 5: Lightning Strike" << std::endl;
            if (level < 8) std::cout << "Level 8: Meteor Storm" << std::endl;
        }
        else if (classType == "Rogue" || classType == "rogue") {
            if (level < 3) std::cout << "Level 3: Poison Dagger" << std::endl;
            if (level < 5) std::cout << "Level 5: Shadow Step" << std::endl;
            if (level < 8) std::cout << "Level 8: Assassinate" << std::endl;
        }
        waitForEnter();
    }

    void buyItems() {
        clearScreen();
        std::cout << sep << std::endl;
        std::cout << " Buy Items " << std::endl;
        std::cout << sep << std::endl;

        std::vector<std::string> itemsForSale = { "Sword", "Shield", "Potion" };
        std::vector<int> itemPrices = { 50, 30, 10 };
        size_t itemsForSaleSize = itemsForSale.size();

        for (size_t i = 0; i < itemsForSaleSize; i++) {
            std::cout << (i + 1) << ". " << itemsForSale[i] << " - $" << itemPrices[i] << std::endl;
        }

        std::cout << "\n=== Battle Items ===" << std::endl;
        for (size_t i = 0; i < battleItems.size(); i++) {
            std::cout << (i + itemsForSaleSize + 1) << ". " << battleItems[i]
                << " - $" << battleItemPrices[i] << std::endl;
        }

        std::cout << "\nYour Money: $" << player.getMoney() << std::endl;
        std::cout << "Enter the number of the item you want to buy (or 0 to go back): ";

        int choice;
        std::cin >> choice;

        if (choice == 0) return;

        if (choice >= 1 && choice <= static_cast<int>(itemsForSaleSize)) {
            int itemIndex = choice - 1;
            if (player.hasItem(itemsForSale[itemIndex])) {
                std::cout << "You already own a " << itemsForSale[itemIndex] << "." << std::endl;
                waitForEnter();
                return;
            }
            if (player.getMoney() >= itemPrices[itemIndex]) {
                player.spendMoney(itemPrices[itemIndex]);
                player.addItem(itemsForSale[itemIndex]);
                std::cout << "You bought a " << itemsForSale[itemIndex] << "! Remaining Money: $" << player.getMoney() << std::endl;
                waitForEnter();
            }
            else {
                std::cout << "Not enough money to buy " << itemsForSale[itemIndex] << "." << std::endl;
                waitForEnter();
            }
        }
        else if (choice >= static_cast<int>(itemsForSaleSize + 1) &&
            choice <= static_cast<int>(itemsForSaleSize + battleItems.size())) {
            int itemIndex = choice - itemsForSaleSize - 1;
            std::string itemName = battleItems[itemIndex];

            if (player.hasItem(itemName)) {
                std::cout << "You already own a " << itemName << "." << std::endl;
                waitForEnter();
                return;
            }
            if (player.getMoney() >= battleItemPrices[itemIndex]) {
                player.spendMoney(battleItemPrices[itemIndex]);
                player.addItem(itemName);
                std::cout << "You bought a " << itemName << "! Remaining Money: $" << player.getMoney() << std::endl;
                waitForEnter();
            }
            else {
                std::cout << "Not enough money to buy " << itemName << "." << std::endl;
                waitForEnter();
            }
        }
        else {
            std::cout << "Invalid choice." << std::endl;
            waitForEnter();
        }
    }

    void calculator() const {
        std::cout << sep << std::endl;
        std::cout << " Calculator " << std::endl;
        std::cout << sep << std::endl;

        int num1, num2;
        char operation;

        std::cout << "Enter first number: ";
        std::cin >> num1;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input! Please enter a number.\n";
            return;
        }
        std::cout << "Enter operation (+, -, *, /): ";
        std::cin >> operation;
        std::cout << "Enter second number: ";
        std::cin >> num2;

        int result;
        switch (operation) {
        case '+': result = num1 + num2; break;
        case '-': result = num1 - num2; break;
        case '*': result = num1 * num2; break;
        case '/':
            if (num2 != 0) {
                result = num1 / num2;
            }
            else {
                std::cout << "Error: Division by zero!" << std::endl;
                return;
            }
            break;
        default:
            std::cout << "Invalid operation!" << std::endl;
            return;
        }

        std::cout << "Result: " << result << std::endl;
    }

    void run() {
        clearScreen();
        bool isRunning = true;
        bool playerSet = false;

        while (isRunning) {
            clearScreen();
            welcome();
            std::cout << "Choose : ";

            int choice;
            std::cin >> choice;

            switch (choice) {
            case 1: {
                if (!playerSet) {
                    std::string name = getCharacterName();
                    std::string classType = getCharacterClass();
                    player.setName(name);
                    player.setClass(classType);
                    printCharacter();
                    playerSet = true;
                }
                else {
                    std::cout << "\nCharacter already created!\n" << std::endl;
                    waitForEnter();
                }
                break;
            }
            case 2: {
                if (playerSet) {
                    playGame();
                }
                else {
                    std::cout << "\nPlease create a character first!\n" << std::endl;
                    waitForEnter();
                }
                break;
            }
            case 3: {
                if (playerSet) {
                    showStats();
                }
                else {
                    std::cout << "\nPlease create a character first!\n" << std::endl;
                    waitForEnter();
                }
                break;
            }
            case 4: {
                player.reset();
                playerSet = false;
                std::cout << "\nStats have been reset!\n" << std::endl;
                waitForEnter();
                break;
            }
            case 5: {
                if (playerSet) {
                    clearScreen();
                    std::cout << sep << std::endl;
                    std::cout << " Player Inventory " << std::endl;
                    std::cout << sep << std::endl;
                    player.showInventory();
                    waitForEnter();
                }
                else {
                    std::cout << "\nPlease create a character first!\n" << std::endl;
                    waitForEnter();
                }
                break;
            }
            case 6: {
                if (playerSet) {
                    buyItems();
                }
                else {
                    std::cout << "\nPlease create a character first!\n" << std::endl;
                    waitForEnter();
                }
                break;
            }
            case 7: {
                if (playerSet) {
                    showSkills();
                }
                else {
                    std::cout << "\nPlease create a character first!\n" << std::endl;
                    waitForEnter();
                }
                break;
            }
            case 8: {
                if (playerSet) {
                    player.Cheat_menu(sep);
                }
                else {
                    std::cout << "\nPlease create a character first!\n" << std::endl;
                    waitForEnter();
                }
                break;
            }
            case 9: {
                std::cout << sep << std::endl;
                std::cout << "Bye!" << std::endl;
                std::cout << sep << std::endl;
                isRunning = false;
                break;
            }
            default: {
                std::cout << "\nInvalid choice! Try again.\n" << std::endl;
                waitForEnter();
                break;
            }
            }
        }
    }
};

class Login {
private:
    bool isLogged = false;
    std::string Currentname;

    struct User {
        std::string username;
        std::string password;
        std::string phoneNumber;
        std::string IpAdress;
        bool isLoggedOut;
        int age;
        int userId;

        User() : age(0), userId(0), isLoggedOut(false) {}
        User(const std::string& u, const std::string& p, const std::string& ph,
            const std::string& i, int a)
            : username(u), password(p), phoneNumber(ph), IpAdress(i),
            age(a), userId(0), isLoggedOut(false) {
        }
    };

    std::vector<User> users;
    std::string filename;

    std::string getIp() {
        char buffer[128];
        std::string result;

#ifdef _WIN32
        FILE* pipe = _popen("curl -s ifconfig.me", "r");
#else
        FILE* pipe = popen("curl -s ifconfig.me", "r");
#endif
        if (!pipe) {
            return "ERROR";
        }
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            result += buffer;
        }
#ifdef _WIN32
        _pclose(pipe);
#else
        pclose(pipe);
#endif
        result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
        return result.empty() ? "ERROR" : result;
    }

    void clearScreen() const {
        std::cout << "\033[2J\033[H" << std::flush;
    }

    void sep(const std::string& msg) const {
        std::string sepLine = std::string(40, '=');
        std::cout << sepLine << std::endl;
        std::cout << msg << std::endl;
        std::cout << sepLine << std::endl;
    }

    void checkCin() {
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Wrong Input" << std::endl;
        }
    }

    void waitForEnter() const {
        std::cout << "\nPress Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
    }

    int setMenu(const std::string& title, const std::vector<std::string>& options) {
        clearScreen();
        sep(title);
        for (size_t i = 0; i < options.size(); i++) {
            std::cout << (i + 1) << ". " << options[i] << std::endl;
        }

        int choice;
        std::cout << "\nChoose: ";
        std::cin >> choice;
        checkCin();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        return choice;
    }

    bool isRegistered(const std::string& usernameToCheck) {
        for (const User& u : users) {
            if (u.username == usernameToCheck) {
                return true;
            }
        }
        return false;
    }

public:
    Login(const std::string& fname = "users.txt") : filename(fname) {
        loadData();
        if (users.empty()) {
            users.push_back(User("xavi", "pass1243", "0473 17 04 38", "127.0.0.1", 22));
            users.push_back(User("admin", "admin123", "0473 17 04 39", "127.0.0.2", 30));
            saveData();
        }
    }

    ~Login() {
        std::cout << "\nAuto-saving user data..." << std::endl;
        saveData();
        std::cout << "Goodbye!" << std::endl;
    }

    bool saveData() const {
        std::ofstream file;
        file.open(filename, std::ios::out | std::ios::trunc);

        if (!file.is_open()) {
            std::cerr << "Error: Could not save to file!" << std::endl;
            return false;
        }

        file << users.size() << std::endl;
        for (const User& u : users) {
            file << "Username: " << u.username << std::endl;
            file << "Password: " << u.password << std::endl;
            file << "Phone Number: " << u.phoneNumber << std::endl;
            file << "Age: " << u.age << std::endl;
            file << "Ip: " << u.IpAdress << std::endl;
            file << "LoggedOut: " << (u.isLoggedOut ? "true" : "false") << std::endl;
            file << "============================================" << std::endl;
        }

        file.close();
        std::cout << "Saved " << users.size() << " users to " << filename << std::endl;
        return true;
    }

    bool loadData() {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        if (file.peek() == std::ifstream::traits_type::eof()) {
            file.close();
            return false;
        }

        int count;
        if (!(file >> count)) {
            file.close();
            std::cerr << "File is corrupted!" << std::endl;
            return false;
        }

        if (count < 0 || count > 1000) {
            file.close();
            std::cerr << "Invalid user count!" << std::endl;
            return false;
        }

        file.ignore();
        users.clear();

        for (int i = 0; i < count; i++) {
            std::string line;
            std::string username, password, phone, IP;
            int age = 0;
            bool loggedOut = false;

            if (!std::getline(file, line)) break;
            if (line.find("Username: ") != std::string::npos) {
                username = line.substr(10);
            }
            else {
                break;
            }

            if (!std::getline(file, line)) break;
            if (line.find("Password: ") != std::string::npos) {
                password = line.substr(10);
            }
            else {
                break;
            }

            if (!std::getline(file, line)) break;
            if (line.find("Phone Number: ") != std::string::npos) {
                phone = line.substr(14);
            }
            else {
                break;
            }

            if (!std::getline(file, line)) break;
            if (line.find("Age: ") != std::string::npos) {
                age = std::stoi(line.substr(5));
            }
            else {
                break;
            }

            if (!std::getline(file, line)) break;
            if (line.find("Ip: ") != std::string::npos) {
                IP = line.substr(4);
            }
            else {
                break;
            }

            if (!std::getline(file, line)) break;
            if (line.find("LoggedOut: ") != std::string::npos) {
                std::string status = line.substr(11);
                loggedOut = (status == "true");
            }
            else {
                break;
            }

            std::getline(file, line);

            User newUser(username, password, phone, IP, age);
            newUser.isLoggedOut = loggedOut;
            users.push_back(newUser);
        }

        file.close();
        return true;
    }

    void registerUser() {
        clearScreen();
        sep("Register New User");


        std::string username, password, phone;
        int age;

        while (true) {
            std::cout << "Enter Username: ";
            std::getline(std::cin, username);
            if (username.empty()) {
                std::cout << "Username Cannot be empty" << std::endl;
                continue;
            }
            if (std::find(username.begin(), username.end(), ' ') != username.end()) {
                std::cout << "Invalid Input: Username contains a space" << std::endl;
                continue;
            }
            if (isRegistered(username)) {
                std::cout << "Error: This username is already taken!" << std::endl;
                continue;
            }
            break;
        }

        while (true) {
            std::cout << "Enter Password: ";
            std::getline(std::cin, password);
            if (password.empty()) {
                std::cout << "Invalid Input: Empty password" << std::endl;
                continue;
            }
            if (std::find(password.begin(), password.end(), ' ') != password.end()) {
                std::cout << "Invalid Input: Contains a space" << std::endl;
                continue;
            }
            if (password.length() < 8) {
                std::cout << "Invalid Input: Password must be atleast 8 Characters" << std::endl;
                continue;
            }
            break;
        }

        while (true) {
            std::string ageStr;
            std::cout << "Enter Age: ";
            std::getline(std::cin, ageStr);
            if (ageStr.empty()) {
                std::cout << "Invalid Input: Empty input" << std::endl;
                continue;
            }
            try {
                age = std::stoi(ageStr);
            }
            catch (...) {
                std::cout << "Invalid Input: Please enter a number" << std::endl;
                continue;
            }
            if (age < 10 || age > 100) {
                std::cout << "Invalid Input: Age must be between 10 and 100" << std::endl;
                continue;
            }
            if (std::find(ageStr.begin(), ageStr.end(), ' ') != ageStr.end()) {
                std::cout << "Invalid Input: Contains a space" << std::endl;
                continue;
            }
            break;
        }

        while (true) {
            std::cout << "Enter Phone Number: ";
            std::getline(std::cin, phone);
            if (phone.empty()) {
                std::cout << "Invalid Input: Empty input" << std::endl;
                continue;
            }
            phone.erase(std::remove(phone.begin(), phone.end(), ' '), phone.end());
            break;
        }

        std::string IP = getIp();
        User newUser(username, password, phone, IP, age);
        newUser.isLoggedOut = false;
        users.push_back(newUser);
        saveData();

        std::cout << "\nRegistration successful!" << std::endl;
        waitForEnter();
    }

    void loginUser() {
        clearScreen();
        sep("Login");

        std::string username, password;

        while (true) {
            std::cout << "Enter Username: ";
            std::getline(std::cin, username);
            if (username.empty()) {
                std::cout << "Invalid Input: Empty Input" << std::endl;
                continue;
            }
            if (std::find(username.begin(), username.end(), ' ') != username.end()) {
                std::cout << "Invalid Input: Input contains spaces" << std::endl;
                continue;
            }
            break;
        }

        while (true) {
            std::cout << "Enter Password: ";
            std::getline(std::cin, password);
            if (password.empty()) {
                std::cout << "Invalid Input: Empty input" << std::endl;
                continue;
            }
            if (std::find(password.begin(), password.end(), ' ') != password.end()) {
                std::cout << "Invalid Input: Input contains a space" << std::endl;
                continue;
            }
            break;
        }

        bool found = false;
        for (User& u : users) {
            if (u.username == username && u.password == password) {
                found = true;
                Currentname = username;
                std::cout << "\nLogin successful! Welcome, " << username << "!" << std::endl;
                isLogged = true;
                u.isLoggedOut = false;
                saveData();
                displayUser(u);
                waitForEnter();
                return;
            }
        }

        if (!found) {
            std::cout << "\nInvalid username or password!" << std::endl;
            waitForEnter();
        }
    }

    void showAllUsers() const {
        clearScreen();
        sep("Registered Users List");

        if (users.empty()) {
            std::cout << "No users registered yet." << std::endl;
        }
        else {
            for (size_t i = 0; i < users.size(); i++) {
                std::cout << (i + 1) << ". Username: " << users[i].username
                    << " | Age: " << users[i].age
                    << " | Phone: " << users[i].phoneNumber
                    << " | Logged Out: " << (users[i].isLoggedOut ? "Yes" : "No") << std::endl;
            }
        }
        waitForEnter();
    }

    void deleteUser() {
        clearScreen();
        sep("Delete User");

        if (users.empty()) {
            std::cout << "No users to delete." << std::endl;
            waitForEnter();
            return;
        }

        showAllUsers();

        std::string username;
        std::cout << "\nEnter username to delete: ";
        std::cin >> username;
        checkCin();

        auto it = std::find_if(users.begin(), users.end(),
            [&username](const User& u) { return u.username == username; });

        if (it != users.end()) {
            char confirm;
            std::cout << "Are you sure you want to delete user '" << username << "'? (y/n): ";
            std::cin >> confirm;

            if (confirm == 'y' || confirm == 'Y') {
                users.erase(it);
                saveData();
                std::cout << "User deleted successfully!" << std::endl;
            }
            else {
                std::cout << "Deletion cancelled." << std::endl;
            }
        }
        else {
            std::cout << "User not found!" << std::endl;
        }

        waitForEnter();
    }

    void displayUser(const User& u) const {
        std::cout << "\n--- User Details ---" << std::endl;
        std::cout << "Username: " << u.username << std::endl;
        std::cout << "Age: " << u.age << std::endl;
        std::cout << "Phone: " << u.phoneNumber << std::endl;
        std::cout << "User ID: " << u.userId << std::endl;
        std::cout << "Logged Out: " << (u.isLoggedOut ? "Yes" : "No") << std::endl;
    }

    void loginMenu() {
        std::string CurrentIP = getIp();
        for (User& u : users) {
            if (u.IpAdress == CurrentIP && !u.isLoggedOut) {
                isLogged = true;
                Currentname = u.username;
                mainMenu();
                return;
            }
        }

        bool isExit = false;
        while (!isExit) {
            if (isLogged) {
                mainMenu();
                continue;
            }

            int choice = setMenu("Login Menu", {
                "Register",
                "Login",
                "Exit"
                });

            switch (choice) {
            case 1:
                registerUser();
                break;
            case 2:
                loginUser();
                break;
            case 3:
                isExit = true;
                saveData();
                std::exit(EXIT_SUCCESS);
                break;
            default:
                std::cout << "Error: Invalid choice. Try again." << std::endl;
                waitForEnter();
                break;
            }
        }
    }

    void LaunchGame() {
        clearScreen();
        std::cout << sepLine() << std::endl;
        std::cout << " Launching Game... " << std::endl;
        std::cout << sepLine() << std::endl;

        Game game;
        game.run();

        std::cout << "\nReturning to main menu..." << std::endl;
        waitForEnter();
    }

    void calculator() {
        clearScreen();
        std::cout << sepLine() << std::endl;
        std::cout << " Calculator " << std::endl;
        std::cout << sepLine() << std::endl;

        int num1, num2;
        char operation;

        std::cout << "Enter first number: ";
        std::cin >> num1;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input! Please enter a number.\n";
            waitForEnter();
            return;
        }
        std::cout << "Enter operation (+, -, *, /): ";
        std::cin >> operation;
        std::cout << "Enter second number: ";
        std::cin >> num2;

        int result;
        switch (operation) {
        case '+': result = num1 + num2; break;
        case '-': result = num1 - num2; break;
        case '*': result = num1 * num2; break;
        case '/':
            if (num2 != 0) {
                result = num1 / num2;
            }
            else {
                std::cout << "Error: Division by zero!" << std::endl;
                waitForEnter();
                return;
            }
            break;
        default:
            std::cout << "Invalid operation!" << std::endl;
            waitForEnter();
            return;
        }

        std::cout << "Result: " << result << std::endl;
        waitForEnter();
    }

    void ProfileMenu() {
        sep("Profile");
        User* CurrentUser = nullptr;
        for (User& u : users) {
            if (u.username == Currentname) {
                CurrentUser = &u;
                break;
            }
        }
        if (CurrentUser == nullptr) {
            std::cerr << "User Not Found" << std::endl;
            waitForEnter();
            return;
        }
        std::cout << "\n--- Your Profile ----" << std::endl;
        std::cout << "Username: " << CurrentUser->username << std::endl;
        std::cout << "Age: " << CurrentUser->age << std::endl;
        std::cout << "Phone Number: " << CurrentUser->phoneNumber << std::endl;
        std::cout << "ID: " << CurrentUser->userId << std::endl;
        std::cout << "Logged Out: " << (CurrentUser->isLoggedOut ? "Yes" : "No") << std::endl;
        waitForEnter();
    }

    void editName(User* user) {
        clearScreen();
        sep("Edit Username");

        std::cout << "Current Username: " << user->username << std::endl;
        std::cout << "Enter New Username: ";
        std::string newUsername;
        std::cin >> newUsername;
        checkCin();

        if (isRegistered(newUsername) && newUsername != user->username) {
            std::cout << "Error: Username already taken!" << std::endl;
            waitForEnter();
            return;
        }

        std::cout << "Are you sure you want to change username from " << user->username << " to " << newUsername << "? (y/n): ";
        std::string choice;
        std::cin >> choice;

        if (choice == "y" || choice == "Y") {
            if (user->username == Currentname) {
                Currentname = newUsername;
            }
            user->username = newUsername;
            std::cout << "Username updated successfully!" << std::endl;
        }
        else {
            std::cout << "Modification cancelled." << std::endl;
        }
        waitForEnter();
    }

    void editAge(User* user) {
        bool isAgeValid = false;
        while (!isAgeValid) {
            clearScreen();
            sep("Edit Age");
            int newAge;
            std::cout << "Current Age: " << user->age << std::endl;
            std::cout << "Enter New Age: ";
            std::cin >> newAge;
            checkCin();

            if (newAge < 10 || newAge > 100) {
                std::cout << "Error: Invalid Age." << '\n' << "Please Enter a number between 10 and 100." << std::endl;
                waitForEnter();
            }
            else {
                char confirmation;
                std::cout << "Are you sure to change your age from " << user->age << " to " << newAge << "? (y/n): ";
                std::cin >> confirmation;
                if (confirmation == 'y' || confirmation == 'Y') {
                    user->age = newAge;
                    std::cout << "Age updated successfully!" << std::endl;
                }
                else {
                    std::cout << "Modification cancelled." << std::endl;
                }
                isAgeValid = true;
                waitForEnter();
            }
        }
    }

    void editPassword(User* user) {
        clearScreen();
        sep("Edit Password");
        std::cout << "Enter current password: ";
        std::string currentpass;
        std::cin >> currentpass;

        if (currentpass != user->password) {
            std::cout << "Password is Incorrect!" << std::endl;
            waitForEnter();
            return;
        }

        std::cout << "Enter New Password: ";
        std::string newPassword;
        std::cin >> newPassword;

        if (newPassword == user->password) {
            std::cout << "New Password cannot be the same as your current password!" << std::endl;
            waitForEnter();
            return;
        }

        bool isPassMatch = false;
        while (!isPassMatch) {
            std::cout << "Confirm Password: ";
            std::string Confirm;
            std::cin >> Confirm;
            if (Confirm == newPassword) {
                user->password = newPassword;
                std::cout << "Password updated successfully!" << std::endl;
                isPassMatch = true;
            }
            else {
                std::cout << "Passwords do not match! Please try again." << std::endl;
                waitForEnter();
            }
        }
        waitForEnter();
    }

    void editPhone(User* user) {
        clearScreen();
        sep("Edit Phone Number");
        std::cout << "Current Phone: " << user->phoneNumber << std::endl;
        std::cout << "Enter New Phone Number: ";
        std::string newPhone;
        std::cin >> newPhone;
        checkCin();
        char confirm;
        std::cout << "Are you sure to change your phone number? (y/n): ";
        std::cin >> confirm;
        if (confirm == 'y' || confirm == 'Y') {
            user->phoneNumber = newPhone;
            std::cout << "Phone number updated successfully!" << std::endl;
        }
        else {
            std::cout << "Modification cancelled." << std::endl;
        }
        waitForEnter();
    }

    void EditUser() {
        bool g_Exit = false;
        while (!g_Exit) {
            User* CurrentU = nullptr;
            for (User& u : users) {
                if (u.username == Currentname) {
                    CurrentU = &u;
                    break;
                }
            }

            if (CurrentU == nullptr) {
                std::cerr << "Invalid User" << std::endl;
                waitForEnter();
                return;
            }

            int choice = setMenu("User Modification", {
                "Change Username",
                "Change Age",
                "Change Password",
                "Change Phone Number",
                "Exit"
                });

            switch (choice) {
            case 1:
                editName(CurrentU);
                saveData();
                break;
            case 2:
                editAge(CurrentU);
                saveData();
                break;
            case 3:
                editPassword(CurrentU);
                saveData();
                break;
            case 4:
                editPhone(CurrentU);
                saveData();
                break;
            case 5:
                g_Exit = true;
                break;
            default:
                std::cout << "Invalid Choice" << std::endl;
                waitForEnter();
                break;
            }
        }
    }

    void settings() {
        bool exit = false;
        while (!exit) {
            clearScreen();
            int choice = setMenu("Settings Menu", {
                "Profile",
                "Modify User",
                "Log Out",
                "Back To Main Menu"
                });

            switch (choice) {
            case 1:
                ProfileMenu();
                break;
            case 2:
                EditUser();
                break;
            case 3: {
                for (User& u : users) {
                    if (u.username == Currentname) {
                        u.isLoggedOut = true;
                        break;
                    }
                }
                isLogged = false;
                saveData();
                std::cout << "Logged out successfully!" << std::endl;
                waitForEnter();
                return;
            }
            case 4:
                return;
            default:
                std::cerr << "Invalid Choice" << std::endl;
                waitForEnter();
                break;
            }
        }
    }

    void book() {
        clearScreen();
        std::cout << sepLine() << std::endl;
        std::cout << " Launching Book Management... " << std::endl;
        std::cout << sepLine() << std::endl;

        LibraryManager manager("library_data.txt", "formatted");
        manager.run();

        std::cout << "\nReturning to main menu..." << std::endl;
        waitForEnter();
    }

    void adminLogin() {
        clearScreen();
        while (true) {
            std::string AdminPass = "!JustOGKing";
            std::string input;
            std::cout << "Enter Admin Password: ";
            std::cin >> input;
            if (input.empty()) {
                std::cout << "Invalid Input: Empty input" << std::endl;
                continue;
            }
            if (input == AdminPass) {
                AdminMenu();
                break;
            }
        }
    }
    void AdminMenu() {
        int choice = setMenu("Admin Panel", { "Delete User", "User List", "Reset All User", "Back" });
        switch (choice) {
        case 1:
            deleteUser();
            break;
        case 2:
            showAllUsers();
            break;
        case 3:
            OwnerReset();
            break;
        case 4:
            mainMenu();
            break;
        case 5:
            return;
        default:
            std::cout << "Invalid input: Invalid Choice" << std::endl;
            break;
        }
    }
    void OwnerReset() {
        clearScreen();
        while (true) {
            std::string OwnerPass = "Legacy_Owner";
            std::string input;
            std::cout << "Enter Owner Password: ";
            std::cin >> input;
            if (input.empty()) {
                std::cout << "Invalid Input: Empty input" << std::endl;
                continue;
            }
            if (input == OwnerPass) {
                while (true) {
                    char conf;
                    std::cout << "Are you sure to clear all users ? (y/n): ";
                    std::cin >> conf;
                    if (conf == 'y' || conf == 'Y') {
                        users.clear();
                        std::cout << "Users have Been Cleared" << std::endl;
                        saveData();
                        waitForEnter();
                        return;
                    }
                    else if (conf == 'n' || conf == 'N') {
                        std::cout << "Modification Cancelled" << std::endl;
                        waitForEnter();
                        return;
                    }
                    else {
                        std::cout << "Invalid input! Please enter y or n" << std::endl;
                    }
                }
            }
            else {
                std::cout << "Wrong Password!" << std::endl;
                waitForEnter();
                return;
            }
        }
    }
    void mainMenu() {
        bool exit = false;
        while (!exit) {
            if (!isLogged) {
                loginMenu();
                return;
            }

            int choice = setMenu("Main menu", {
                "Play Games",
                "Calculator",
                "Book Management",
                "Admin Panel",
                "Settings",
                "Exit"
                });

            switch (choice) {
            case 1:
                LaunchGame();
                break;
            case 2:
                calculator();
                break;
            case 3:
                book();
                break;
            case 4:
                adminLogin();
                break;
            case 5:
                settings();
                break;
            case 6:
                exit = true;
                break;
            default:
                std::cout << "Invalid Choice" << std::endl;
                waitForEnter();
                break;
            }
        }
    }
};

int main() {
    clearScreen();
    Login loginSystem("users.txt");
    loginSystem.loginMenu();
    return 0;
}