![generated-image(1)](https://github.com/user-attachments/assets/c87fcddc-5f7b-48b7-87d5-a229e00ab646)

[![License](https://img.shields.io/badge/license-MIT-red)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux-brightgreen)]()
[![Language](https://img.shields.io/badge/language-C%2B%2B-blue)]()

ReCaptchaV3Plugin is a cross‑platform C++ library to verify Google reCAPTCHA v3 tokens on the **server side** via a small C API and libcurl.  
It is designed as a reusable security module you can plug into backends, services, or game engines to validate tokens and enforce a minimum score.

## Features
[](https://github.com/YourUsername/recaptcha-v3-cpp-plugin#features)

🧩 Simple C API  
Exposes three main functions: one to initialize the library with your secret key, one to verify a token, and one to clean up resources.

🌍 Cross‑platform  
Same codebase builds as a shared library on Windows (.dll) and Linux (.so) using CMake.

🛡 Score & action validation  
The plugin calls the official reCAPTCHA v3 verification endpoint and checks both the returned `score` and the `action` you expect.

⏱ Robust HTTP handling  
Uses libcurl with connect and total timeouts, and checks HTTP status codes before trusting the response.

🔒 Server-side only  
The reCAPTCHA secret key is meant to be configured on the server only; the client sends only the token to your backend.

## How it works

1. Your frontend (website or app) obtains a reCAPTCHA v3 token from Google using the client‑side script.
2. The frontend sends the token to your backend (for example in a form POST or JSON request).
3. Your backend calls the plugin’s verification function, passing the token, the expected action, and optionally the client IP.
4. The plugin sends a server‑side request to Google’s verification API using your secret key.
5. Google returns a JSON response with `success`, `score`, and `action`.
6. The plugin parses this response, compares the score with your configured minimum, and returns a status code to your backend.
7. Your backend accepts or rejects the request based on that status code.

## Project structure

- `ReCaptchaV3Plugin.h`  
  Public C header that defines the exported functions and return codes.  
  This is the only file your application code needs to include.

- `ReCaptchaV3Plugin.cpp`  
  Internal implementation of the plugin.  
  Contains the logic to build the HTTP request, call the Google verification endpoint, apply timeouts, and parse the JSON response.

- `main.cpp`  
  Small example program showing how to initialize the plugin, verify a token, and interpret the result codes.  
  Used only as a demo; not required in your own project.

- `CMakeLists.txt`  
  Build configuration that creates a shared library and an example executable on both Windows and Linux.

  ## Configuration and API keys

  The plugin is intended to be configured at runtime, not hard‑coded.

- **Secret key**  
  You pass your reCAPTCHA secret key from your backend code into the initialization function.  
  In a real application, you should load this secret from a secure location (environment variable, config file, secret manager) rather than writing it directly in source code.

- **Minimum score**  
  During initialization, you also choose a minimum score (for example `0.5` or `0.7`).  
  The plugin will treat any response with a lower score as suspicious and return a specific error code.

- **Expected action**  
  When you call the verification function, you can provide the name of the action (for example `"login"` or `"signup"`).  
  The plugin compares this value with the `action` field in Google’s response to ensure that the token is actually for the operation you expect.

  ## How to modify the source

  You can adapt the implementation file to fit your environment:

- **Change where the secret key comes from**  
  Instead of passing the secret key directly from your application, you can modify the initialization function to:
  - Read the key from an environment variable.
  - Read the key from a configuration file.
  - Query a secret storage service and cache the result.

- **Adjust timeouts and HTTP behavior**  
  In the implementation, the HTTP client is configured with connection and global time limits.  
  You can increase or decrease these values depending on your latency and reliability requirements.

- **Customize logging and error handling**  
  You can add logging when verification fails (for example, wrong action, low score, or HTTP error), or to collect metrics on how often captchas fail.

- **Switch JSON parsing strategy**  
  The current example uses simple manual parsing of the JSON response to extract `success`, `score`, and `action`.  
  If you prefer, you can replace that logic with a dedicated JSON library to support more complex use cases.

  ## Build and usage

  - Make sure a C++17 compiler, CMake, and the libcurl development files are installed on your system.
- Configure and build the project to produce:
  - A shared library (`.so` on Linux, `.dll` on Windows).
  - An example executable that demonstrates the API.

In your own backend or service:

1. Link against the built library.
2. Include the public header.
3. During startup, initialize the plugin with your secret key and minimum score.
4. For every incoming request that includes a reCAPTCHA v3 token, call the verification function and check the returned status code before proceeding.
