@mainpage

# PSA Crypto API Documentation

Arm's Platform Security Architecture (PSA) is a holistic set of threat models, security analyses, hardware
and firmware architecture specifications, an open source firmware reference implementation, and an
independent evaluation and certification scheme. PSA provides a recipe, based on industry best practice,
that allows security to be consistently designed in, at both a hardware and firmware level.
The PSA Cryptographic API (Crypto API) is an important PSA component that provides a portable interface
to cryptographic operations on a wide range of hardware

## Overview
The PSA Crypto API provides a standardized interface for performing cryptographic operations, 
ensuring consistency and security across different platforms and implementations.

## Key Features
- **Secure Key Management**: Create, import, export, and manage cryptographic keys securely.
- **Cryptographic Operations**: Perform encryption, decryption, hashing, MAC, and digital signature operations.
- **Algorithm Agility**: Support for a wide range of cryptographic algorithms.
- **Platform Independence**: Consistent API behavior across different hardware and software platforms.

## Modules
The PSA Crypto API is divided into several modules, each responsible for different aspects of cryptography:

- **Key Management**: Functions for managing cryptographic keys.
- **Message Digests**: Functions for computing hash values.
- **Message Authentication Codes (MAC)**: Functions for generating and verifying MACs.
- **Ciphers**: Functions for encryption and decryption.
- **Digital Signatures**: Functions for generating and verifying digital signatures.
- **Asymmetric Key Operations**: Functions for asymmetric encryption and decryption.
- **Key Derivation**: Functions for deriving keys from existing keys or passwords (Not supported).
