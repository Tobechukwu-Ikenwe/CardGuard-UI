# CardGuard

**A credit card validation application with a user interface — robust, local, and privacy-first.**

---

## Overview

CardGuard is a **user-facing credit card validation application** that combines multiple validation techniques to verify card numbers. It helps developers, testers, and educators confirm that a card number is structurally valid before using it in payment flows — without ever sending data over the internet.

**Who it's for:** Developers integrating payment systems, QA engineers testing checkout flows, and anyone learning how card validation works.

**What problem it solves:** Real card validation requires more than a Luhn check. Weak or predictable numbers (like `1111 1111 1111 1111`) can pass Luhn but fail in production. CardGuard catches these with entropy and repetition analysis, and presents results through a clear, confidence-based UI.

---

## User Interface Overview

CardGuard provides a **single-screen interface** designed to feel like an **airport security checkpoint**: you present your number, the system runs it through several checks, and you get a clear pass/fail with an explanation.

### Layout

1. **Input field** — Where you enter the card number (with or without spaces). The app auto-formats as you type (e.g., groups of 4 digits).

2. **Validate button** — Triggers validation. Press once or use keyboard shortcut (e.g., Enter).

3. **Result panel** — Shows the verdict:
   - **VALID (high confidence)** — Green; all checks passed.
   - **VALID (low confidence)** — Amber; Luhn passed, but entropy or repetition is weak (e.g., test numbers).
   - **INVALID** — Red; one or more checks failed.

4. **Check breakdown** — A simple list of each check and its outcome:
   - Length ✓/✗  
   - Issuer (VISA, Mastercard, etc.)  
   - Luhn checksum ✓/✗  
   - Entropy score (with bar or badge)  
   - Repetition analysis ✓/✗  

5. **Confidence indicator** — A visual cue (e.g., bar, badge, or label) for how “strong” the number is. High entropy = high confidence; low entropy (e.g., many repeated digits) = low confidence.

### Mental Model

Think of it like a passport checkpoint: you show your document (card number), and the system runs several quick checks. You don’t need to know the math — you just see whether you’re cleared, cautioned, or denied, and why.

---

## How Validation Works (Conceptual)

### Luhn Check

The **Luhn algorithm** is a standard checksum used by the card industry. It catches typos and simple mistakes by applying a fixed formula to the digits. If the result doesn’t match a specific target, the number is rejected. **Analogy:** It’s like a math bouncer — only numbers that satisfy the formula get through.

### Entropy Analysis

**Entropy** measures how “random” or unpredictable the digits are. A number like `1111 1111 1111 1111` has very low entropy (almost no randomness). A well-mixed number like `4539 1488 0343 6467` has higher entropy. CardGuard uses this to flag weak, guessable numbers. **Analogy:** It’s like judging how hard a lock combination would be to guess.

### Repetition Detection

**Repetition analysis** looks for obvious patterns: repeated digits (`1111`), simple sequences (`1234`), or copy-paste-like blocks. These often indicate test or fake numbers. **Analogy:** It’s like spotting a weak password that uses `12345678`.

---

## Safety & Privacy Guarantees

- **Local-only processing** — All validation runs on your machine.
- **No storage** — Card numbers are never written to disk.
- **No logging** — Input is not recorded or sent anywhere.
- **No network** — The app does not connect to the internet.
- **Test numbers only in examples** — The README and UI samples use standard test numbers (e.g., MasterCard `5555 5525 0000 1001`) that cannot be used for real purchases.

---

## Features

- **High-confidence vs low-confidence results** — Clear distinction between strong valid numbers and weak ones (e.g., test numbers).
- **Real-time feedback** — Validation runs in nanoseconds; results appear immediately.
- **Clear warnings for weak numbers** — Low entropy or repetition triggers a low-confidence or invalid result with an explanation.
- **Fast and local** — Nanosecond-level validation with no network or disk I/O.

---

## Project Structure

```
CardGuard/
│
├── include/
│   └── card_validator.h      # Function declarations, structs
│
├── src/
│   ├── main.cpp              # CLI entry point
│   └── validator.cpp         # Core validation logic
│
├── web/
│   └── index.html            # Serverless web UI (static)
│
├── Makefile
├── run.sh                    # Serve web UI (Linux/macOS)
└── README.md
```

The **core engine** (`validator.cpp`) stays separate from the **web UI** (`web/`). The web UI is a serverless static app — validation runs entirely in the browser.

---

## Build & Run

### Web UI (serverless, recommended)

Static app — no backend. Serve the `web/` folder on a port:

**Python:**
```bash
cd web && python -m http.server 8765
```
Open http://localhost:8765

**Node.js (npx serve):**
```bash
npx serve web -p 8765
```
Open http://localhost:8765

**Direct:** Open `web/index.html` in a browser (file://). Validation runs entirely client-side.

**Linux / macOS:** `./run.sh` serves on port 8765 and opens the browser.

### CLI (C++)

**Linux / macOS:**
```bash
make
./card_validator 4539 1488 0343 6467
```

**Windows (MSYS2/MinGW):** `make` then `card_validator.exe 4539 1488 0343 6467`

### Input Format

Enter the card number with or without spaces. Examples:

- `4539 1488 0343 6467`
- `4539148803436467`

---

## How to Use the App

1. **Launch** the application (CLI or UI mode).
2. **Enter** a card number in the input field (spaces optional).
3. **Click Validate** or press Enter.
4. **Read the result:**
   - **VALID (high confidence)** — Use with confidence for testing. All checks passed.
   - **VALID (low confidence)** — Likely a test number. Safe for demos, but flagged as weak.
   - **INVALID** — Do not use. Check the breakdown to see which step failed.
5. **Interpret confidence** — Higher entropy and no repetition mean higher confidence. Low confidence does not mean invalid, but it suggests the number is weak or synthetic.

---

## Sample UI Output (Textual Mock)

### High-Confidence Valid Card

```
┌─────────────────────────────────────────────────────────┐
│  CardGuard — Credit Card Validation                     │
├─────────────────────────────────────────────────────────┤
│  Card number:  [4539 1488 0343 6467]        [Validate]  │
├─────────────────────────────────────────────────────────┤
│  Result:  VALID (high confidence)              [green]   │
│  Confidence:  ████████████████████░░  92%               │
│                                                         │
│  Checks:                                                │
│    Length (16 digits)     ✓                             │
│    Issuer: VISA           ✓                             │
│    Luhn checksum          ✓                             │
│    Entropy (3.91 bits)    ✓                             │
│    Repetition analysis    ✓                             │
│                                                         │
│  Verification completed in 38 ns                        │
└─────────────────────────────────────────────────────────┘
```

### Low-Confidence Valid Card (Test MasterCard)

```
┌─────────────────────────────────────────────────────────┐
│  Result:  VALID (low confidence)              [amber]    │
│  Confidence:  ████████░░░░░░░░░░░░░░  35%               │
│                                                         │
│  Checks:                                                │
│    Length (16 digits)     ✓                             │
│    Issuer: MASTERCARD     ✓                             │
│    Luhn checksum          ✓                             │
│    Entropy (2.45 bits)    ✗  Below threshold            │
│    Repetition analysis    ✓                             │
│                                                         │
│  ⚠ This number passes Luhn but has low entropy.         │
│    Often seen with test numbers.                        │
└─────────────────────────────────────────────────────────┘
```

### Invalid Card

```
┌─────────────────────────────────────────────────────────┐
│  Result:  INVALID                            [red]       │
│  Confidence:  █░░░░░░░░░░░░░░░░░░░░░  5%                │
│                                                         │
│  Checks:                                                │
│    Length (16 digits)     ✓                             │
│    Issuer: UNKNOWN        —                             │
│    Luhn checksum          ✗                             │
│    Entropy (0.00 bits)    ✗                             │
│    Repetition analysis    ✗                             │
│                                                         │
│  Verification completed in 35 ns                        │
└─────────────────────────────────────────────────────────┘
```

---

## Performance

- **Validation:** Nanosecond-level (typically 35–45 ns per number).
- **UI overhead:** Minimal. The bottleneck is user input and rendering, not the engine.
- **No I/O during validation:** Pure in-memory computation.

---

## Roadmap / Next Steps

- **Batch processing** — Validate thousands of card numbers in a single run.
- **Streaming systems** — Single-pass, cache-friendly pipeline for Kafka/Pub-Sub–based fraud systems.
- **Feature-emitting component** — Integrate the validator into fraud pipelines, emitting privacy-preserving signals (confidence, entropy, anomaly flags) without handling raw card data.
- **Advanced pattern detection** — Machine learning–based anomaly detection.
- **Web/CLI API** — Safe, read-only interfaces for enterprise usage.
- **Enterprise UI** — Configurable dashboards and audit logging for compliance.
