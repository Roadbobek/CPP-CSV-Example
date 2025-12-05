#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>

// --- Configuration ---
const std::string MOCK_FILENAME = "data.csv";

/**
 * @brief The CSVReader class handles loading, parsing, and accessing CSV data.
 */
class CSVReader {
private:
    // The core data structure: a vector of rows, where each row is a vector of strings (cells).
    std::vector<std::vector<std::string>> data;
    std::vector<std::string> headers; // To store the header row (column names)

    /**
     * @brief Helper function to parse a single line into fields (cells).
     * @param line The string line read from the file.
     * @return A vector of strings representing the fields in that row.
     */
    std::vector<std::string> parseRow(const std::string& line) {
        std::vector<std::string> fields;
        std::stringstream ss(line);
        std::string cell;

        // This is the core logic: read cell by cell, separated by the delimiter (',').
        while (std::getline(ss, cell, ',')) {
            fields.push_back(cell);
        }
        return fields;
    }

    /**
     * @brief Checks if a file exists, returns a bool, passing in a relative or absolute path.
     */
    bool file_exists_modern(const std::string& filename) const {
        // std::filesystem::path handles cross-platform path conversions
        return std::filesystem::exists(filename);
    }

    /**
     * @brief Creates a mock CSV file for testing purposes if one doesn't exist.
     */
    void createMockFile() const
    {   // Check if file does not exist first.
        if (!file_exists_modern(MOCK_FILENAME)) {
        std::ofstream ofs(MOCK_FILENAME);
        if (ofs.is_open()) {
            // Write the header row
            ofs << "ItemID,Category,Price,UnitsSold,Location\n";
            // Write data rows
            ofs << "101,Electronics,49.99,150,East\n";
            ofs << "102,Books,19.50,300,West\n";
            ofs << "103,Electronics,129.00,80,North\n";
            ofs << "104,Clothing,35.75,220,East\n";
            ofs << "105,Books,15.00,450,South\n";
            ofs.close();
            std::cout << "[INFO] Created mock file '" << MOCK_FILENAME << "' for demonstration.\n";
        } else {
            std::cerr << "[ERROR] Could not create mock file.\n";
        }
        } else {
            std::cerr << "[INFO] " << MOCK_FILENAME << " Already exists it will not be created.\n";
        }
    }

public:
    /**
     * @brief Loads and parses the entire CSV file into the internal data structure.
     * @param filename The name of the CSV file to load.
     * @return true if successful, false otherwise.
     */
    bool load(const std::string& filename) {
        // Ensure mock file exists for demonstration
        createMockFile();

        std::ifstream ifs(filename);
        if (!ifs.is_open()) {
            std::cerr << "[ERROR] Failed to open file: " << filename << "\n";
            return false;
        }

        std::string line;
        bool is_header = true;

        // Read the file line by line
        while (std::getline(ifs, line)) {
            if (line.empty()) continue; // Skip empty lines

            // Use the helper function to parse the row
            std::vector<std::string> row_fields = parseRow(line);

            if (is_header) {
                // The first row is the header
                headers = row_fields;
                is_header = false;
            } else {
                // All subsequent rows are data
                data.push_back(row_fields);
            }
        }

        ifs.close();
        std::cout << "[SUCCESS] Loaded " << data.size() << " data rows from " << filename << ".\n";
        return true;
    }

    /**
     * @brief Prints the data in a neat table format.
     */
    void printTable() const {
        if (data.empty()) {
            std::cout << "[INFO] No data loaded.\n";
            return;
        }

        std::cout << "\n--- Loaded Data Table ---\n";

        // --- Print Headers ---
        std::cout << std::left;
        for (const auto& header : headers) {
            std::cout << std::setw(15) << header;
        }
        std::cout << "\n";
        std::cout << std::string(headers.size() * 15, '-') << "\n";

        // --- Print Data Rows ---
        for (const auto& row : data) {
            for (const auto& cell : row) {
                std::cout << std::setw(15) << cell;
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    /**
     * @brief Analyzes and calculates the total revenue from the data.
     * This demonstrates converting strings to numbers (parsing).
     */
    void calculateTotalRevenue() const {
        if (data.empty()) {
            std::cout << "[INFO] Cannot calculate revenue: no data loaded.\n";
            return;
        }

        double total_revenue = 0.0;

        // Find the index of the "Price" and "UnitsSold" columns
        auto price_it = std::find(headers.begin(), headers.end(), "Price");
        auto units_it = std::find(headers.begin(), headers.end(), "UnitsSold");

        if (price_it == headers.end() || units_it == headers.end()) {
            std::cerr << "[ERROR] Could not find 'Price' or 'UnitsSold' columns for analysis.\n";
            return;
        }

        int price_idx = std::distance(headers.begin(), price_it);
        int units_idx = std::distance(headers.begin(), units_it);

        // Iterate through each data row
        for (const auto& row : data) {
            try {
                // Use std::stod (String to Double) for price conversion
                double price = std::stod(row[price_idx]);
                // Use std::stoi (String to Integer) for units conversion
                int units = std::stoi(row[units_idx]);

                total_revenue += price * units;
            } catch (const std::exception& e) {
                std::cerr << "[WARNING] Skipping row due to parsing error: " << e.what() << "\n";
            }
        }

        std::cout << "--- Analysis Result ---\n";
        std::cout << "Total Estimated Revenue: $" << std::fixed << std::setprecision(2) << total_revenue << "\n\n";
    }
};

// --- Main Program Entry Point ---
int main() {
    CSVReader reader;

    // 1. Load the data from the file
    if (reader.load(MOCK_FILENAME)) {

        // 2. Display the loaded data
        reader.printTable();

        // 3. Perform an analysis on the data
        reader.calculateTotalRevenue();
    }

    return 0;
}