#include "card_validator.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <map>
#include <sstream>

namespace cardguard {

std::string normalize_input(const std::string& input) {
    std::string result;
    for (char c : input) {
        if (c >= '0' && c <= '9') {
            result += c;
        }
    }
    return result;
}

bool luhn_check(const std::string& digits) {
    if (digits.empty()) return false;

    int sum = 0;
    bool alternate = false;

    for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
        int n = *it - '0';
        if (alternate) {
            n *= 2;
            if (n > 9) n -= 9;
        }
        sum += n;
        alternate = !alternate;
    }

    return (sum % 10) == 0;
}

double calculate_entropy(const std::string& digits) {
    if (digits.empty()) return 0.0;

    std::map<char, int> freq;
    for (char c : digits) {
        if (c >= '0' && c <= '9') {
            freq[c]++;
        }
    }

    double entropy = 0.0;
    double len = static_cast<double>(digits.size());

    for (const auto& p : freq) {
        double p_i = static_cast<double>(p.second) / len;
        if (p_i > 0) {
            entropy -= p_i * std::log2(p_i);
        }
    }

    return entropy;
}

bool repetition_check(const std::string& digits) {
    if (digits.size() < 4) return true;  // Too short to judge

    // Check for all same digit
    char first = digits[0];
    bool all_same = true;
    for (size_t i = 1; i < digits.size(); i++) {
        if (digits[i] != first) {
            all_same = false;
            break;
        }
    }
    if (all_same) return false;

    // Check for repeated 4-digit blocks (e.g. 1111 1111 1111 1111, 1234 1234 1234 1234)
    if (digits.size() >= 8) {
        std::string block1 = digits.substr(0, 4);
        int repeat_count = 1;
        for (size_t i = 4; i + 4 <= digits.size(); i += 4) {
            if (digits.substr(i, 4) == block1) {
                repeat_count++;
            } else {
                break;
            }
        }
        if (repeat_count >= 4) return false;  // Same block repeated 4+ times
    }

    // Check for simple ascending sequence (1234 5678 9012 3456)
    bool is_sequence = true;
    for (size_t i = 1; i < digits.size(); i++) {
        int prev = digits[i - 1] - '0';
        int curr = digits[i] - '0';
        int expected = (prev + 1) % 10;
        if (curr != expected) {
            is_sequence = false;
            break;
        }
    }
    if (is_sequence) return false;

    return true;
}

Issuer detect_issuer(const std::string& digits) {
    if (digits.size() < 2) return Issuer::UNKNOWN;

    int first = digits[0] - '0';
    int first_two = (digits.size() >= 2) ? std::stoi(digits.substr(0, 2)) : 0;
    int first_four = (digits.size() >= 4) ? std::stoi(digits.substr(0, 4)) : 0;

    if (first == 4) return Issuer::VISA;
    if (first_two >= 51 && first_two <= 55) return Issuer::MASTERCARD;
    if (first_two == 34 || first_two == 37) return Issuer::AMEX;
    if (first_four == 6011 || (first_two == 65) ||
        (digits.size() >= 6 && digits.substr(0, 6) >= "622126" && digits.substr(0, 6) <= "622925")) {
        return Issuer::DISCOVER;
    }
    return Issuer::UNKNOWN;
}

std::string issuer_to_string(Issuer issuer) {
    switch (issuer) {
        case Issuer::VISA:      return "VISA";
        case Issuer::MASTERCARD: return "MASTERCARD";
        case Issuer::AMEX:      return "AMEX";
        case Issuer::DISCOVER:  return "DISCOVER";
        default:                return "UNKNOWN";
    }
}

ValidationResult validate(const std::string& input) {
    auto start = std::chrono::high_resolution_clock::now();
    ValidationResult result = {};
    result.valid = false;
    result.high_confidence = false;
    result.entropy = 0.0;
    result.entropy_passed = false;
    result.issuer = Issuer::UNKNOWN;

    std::string normalized = normalize_input(input);
    result.normalized = normalized;
    result.length = static_cast<int>(normalized.size());

    // Length check: standard cards are 13–19 digits
    if (normalized.size() < 13 || normalized.size() > 19) {
        result.length_check = {false, "Length must be 13–19 digits"};
        auto end = std::chrono::high_resolution_clock::now();
        result.ns_elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        return result;
    }
    result.length_check = {true, "Length OK (" + std::to_string(normalized.size()) + " digits)"};

    // Issuer detection
    result.issuer = detect_issuer(normalized);
    result.issuer_name = issuer_to_string(result.issuer);

    // Luhn check
    bool luhn_ok = luhn_check(normalized);
    result.luhn_check = {luhn_ok, luhn_ok ? "PASS" : "FAIL"};
    result.valid = luhn_ok;

    // Entropy
    result.entropy = calculate_entropy(normalized);
    result.entropy_passed = (result.entropy >= ENTROPY_THRESHOLD);
    std::ostringstream entropy_msg;
    entropy_msg << std::fixed;
    entropy_msg.precision(2);
    entropy_msg << result.entropy << " bits/digit (threshold: " << ENTROPY_THRESHOLD << ") "
                << (result.entropy_passed ? "PASS" : "FAIL");
    result.entropy_check = {result.entropy_passed, entropy_msg.str()};

    // Repetition
    bool repetition_ok = repetition_check(normalized);
    result.repetition_check = {repetition_ok, repetition_ok ? "PASS" : "FAIL"};

    // High confidence = valid + entropy pass + repetition pass
    result.high_confidence = result.valid && result.entropy_passed && repetition_ok;

    auto end = std::chrono::high_resolution_clock::now();
    result.ns_elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    return result;
}

} // namespace cardguard
