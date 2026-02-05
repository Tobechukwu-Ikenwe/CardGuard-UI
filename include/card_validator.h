#ifndef CARD_VALIDATOR_H
#define CARD_VALIDATOR_H

#include <string>
#include <cstdint>

namespace cardguard {

// Issuer types based on BIN patterns
enum class Issuer {
    UNKNOWN,
    VISA,
    MASTERCARD,
    AMEX,
    DISCOVER
};

// Result of a single check
struct CheckResult {
    bool passed;
    std::string message;
};

// Full validation result
struct ValidationResult {
    bool valid;                    // Overall: passes Luhn
    bool high_confidence;          // All checks pass (entropy, repetition)
    std::string normalized;        // Digits only
    int length;
    CheckResult length_check;
    Issuer issuer;
    std::string issuer_name;
    CheckResult luhn_check;
    double entropy;                // bits per digit
    bool entropy_passed;           // entropy >= threshold
    CheckResult entropy_check;
    CheckResult repetition_check;
    int64_t ns_elapsed;
};

constexpr double ENTROPY_THRESHOLD = 3.5;

// Normalize input: remove spaces and non-digits (like cleaning a lens before a photo)
std::string normalize_input(const std::string& input);

// Luhn checksum: math bouncer for valid card numbers
bool luhn_check(const std::string& digits);

// Shannon entropy of digit distribution (randomness measure)
double calculate_entropy(const std::string& digits);

// Detect repeated digits, sequences (1234), copy-paste patterns
bool repetition_check(const std::string& digits);

// Detect issuer from leading digits
Issuer detect_issuer(const std::string& digits);
std::string issuer_to_string(Issuer issuer);

// Full validation pipeline
ValidationResult validate(const std::string& input);

} // namespace cardguard

#endif // CARD_VALIDATOR_H
