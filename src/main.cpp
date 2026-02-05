#include "card_validator.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::string input;

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (i > 1) input += " ";
            input += argv[i];
        }
    } else {
        std::cout << "CardGuard — Enter card number: ";
        std::getline(std::cin, input);
    }

    if (input.empty()) {
        std::cout << "No input. Usage: card_validator [number]\n";
        std::cout << "Or run with --ui to open the web interface.\n";
        return 1;
    }

    cardguard::ValidationResult r = cardguard::validate(input);

    std::cout << "[INFO] Input normalized (spaces removed)\n";
    std::cout << "[INFO] Length check " << (r.length_check.passed ? "passed" : "FAILED")
              << " (" << r.length << " digits)\n";
    std::cout << "[INFO] Issuer pattern recognized: " << r.issuer_name << "\n";
    std::cout << "[INFO] Luhn checksum: " << (r.luhn_check.passed ? "PASS" : "FAIL") << "\n";
    std::cout << "[INFO] Entropy score: " << r.entropy_check.message << "\n";
    std::cout << "[INFO] Repetition analysis: " << (r.repetition_check.passed ? "PASS" : "FAIL") << "\n";

    if (r.valid) {
        if (r.high_confidence) {
            std::cout << "[RESULT] Card number is VALID\n";
        } else {
            std::cout << "[RESULT] Card number is VALID (low confidence)\n";
        }
    } else {
        std::cout << "[RESULT] Card number is INVALID\n";
    }

    std::cout << "[TIME] Verification completed in " << r.ns_elapsed << " ns\n";

    return r.valid ? 0 : 1;
}
