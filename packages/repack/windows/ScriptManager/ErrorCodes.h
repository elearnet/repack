#pragma once
#include <string>

// Error codes for rejecting promises
namespace ErrorCodes {
const char *const CodeExecutionFailure = "CodeExecutionFailure";
const char *const InvalidationFailure = "InvalidationFailure";
const char *const ScriptDownloadFailure = "ScriptDownloadFailure";
const char *const UnsupportedScheme = "UnsupportedScheme";
const char *const ScriptConfigError = "ScriptConfigError";
const char *const RuntimeUnavailableError = "RuntimeUnavailableError";
const char *const CallInvokerUnavailableError = "CallInvokerUnavailableError";
const char *const CodeSigningTokenNotFoundError = "CodeSigningTokenNotFoundError";
const char *const CodeSigningTokenInvalidError = "CodeSigningTokenInvalidError";
const char *const CodeSigningTokenDecodingError = "CodeSigningTokenDecodingError";
const char *const CodeSigningTokenVerificationError = "CodeSigningTokenVerificationError";
const char *const CodeSigningPublicKeyNotFoundError = "CodeSigningPublicKeyNotFoundError";
const char *const CodeSigningPublicKeyInvalidError = "CodeSigningPublicKeyInvalidError";
const char *const CodeSigningBundleVerificationError = "CodeSigningBundleVerificationError";
} // namespace ErrorCodes
