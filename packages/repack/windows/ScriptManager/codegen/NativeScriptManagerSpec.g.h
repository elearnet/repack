
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once
// clang-format off

#include <NativeModules.h>
#include <tuple>

namespace ScriptManagerCodegen {

struct ScriptManagerSpec_NormalizedScriptLocator {
    std::string uniqueId;
    std::string method;
    std::string url;
    bool fetch;
    double timeout;
    bool absolute;
    std::optional<std::string> query;
    std::optional<::React::JSValue> headers;
    std::optional<std::string> body;
    std::string verifyScriptSignature;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(ScriptManagerSpec_NormalizedScriptLocator*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"uniqueId", &ScriptManagerSpec_NormalizedScriptLocator::uniqueId},
        {L"method", &ScriptManagerSpec_NormalizedScriptLocator::method},
        {L"url", &ScriptManagerSpec_NormalizedScriptLocator::url},
        {L"fetch", &ScriptManagerSpec_NormalizedScriptLocator::fetch},
        {L"timeout", &ScriptManagerSpec_NormalizedScriptLocator::timeout},
        {L"absolute", &ScriptManagerSpec_NormalizedScriptLocator::absolute},
        {L"query", &ScriptManagerSpec_NormalizedScriptLocator::query},
        {L"headers", &ScriptManagerSpec_NormalizedScriptLocator::headers},
        {L"body", &ScriptManagerSpec_NormalizedScriptLocator::body},
        {L"verifyScriptSignature", &ScriptManagerSpec_NormalizedScriptLocator::verifyScriptSignature},
    };
    return fieldMap;
}

struct ScriptManagerSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(std::string, ScriptManagerSpec_NormalizedScriptLocator, Promise<void>) noexcept>{0, L"loadScript"},
      Method<void(std::string, ScriptManagerSpec_NormalizedScriptLocator, Promise<void>) noexcept>{1, L"prefetchScript"},
      Method<void(std::vector<std::string>, Promise<void>) noexcept>{2, L"invalidateScripts"},
      SyncMethod<bool(std::string, std::string) noexcept>{3, L"unstable_evaluateScript"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, ScriptManagerSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "loadScript",
          "    REACT_METHOD(loadScript) void loadScript(std::string scriptId, ScriptManagerSpec_NormalizedScriptLocator && scriptConfig, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(loadScript) static void loadScript(std::string scriptId, ScriptManagerSpec_NormalizedScriptLocator && scriptConfig, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "prefetchScript",
          "    REACT_METHOD(prefetchScript) void prefetchScript(std::string scriptId, ScriptManagerSpec_NormalizedScriptLocator && scriptConfig, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(prefetchScript) static void prefetchScript(std::string scriptId, ScriptManagerSpec_NormalizedScriptLocator && scriptConfig, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "invalidateScripts",
          "    REACT_METHOD(invalidateScripts) void invalidateScripts(std::vector<std::string> const & scripts, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(invalidateScripts) static void invalidateScripts(std::vector<std::string> const & scripts, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "unstable_evaluateScript",
          "    REACT_SYNC_METHOD(unstable_evaluateScript) bool unstable_evaluateScript(std::string scriptSource, std::string scriptSourceUrl) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(unstable_evaluateScript) static bool unstable_evaluateScript(std::string scriptSource, std::string scriptSourceUrl) noexcept { /* implementation */ }\n");
  }
};

} // namespace ScriptManagerCodegen
