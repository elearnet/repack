#pragma once

#include "codegen/NativeScriptManagerSpec.g.h"
#include "winrt/Windows.Data.Json.h"
#include "winrt/Windows.Foundation.Collections.h"
#include "winrt/Windows.Storage.Streams.h"

namespace winrt::ScriptManager {
struct ScriptConfig {
  std::string scriptId;
  winrt::Windows::Foundation::Uri url{nullptr};
  std::string method;
  bool fetch;
  bool absolute;
  winrt::Windows::Storage::Streams::IBuffer body{nullptr};
  winrt::Windows::Foundation::Collections::IMap<winrt::hstring, winrt::hstring> headers{nullptr};
  winrt::Windows::Foundation::TimeSpan timeout;
  std::string verifyScriptSignature;
  std::string uniqueId;
  std::string sourceUrl;

  // Creates a ScriptConfig from the codegen spec.
  static ScriptConfig FromSpec(std::string scriptId,ScriptManagerCodegen::ScriptManagerSpec_NormalizedScriptLocator const &spec) {
    // The spec.headers is a JSValue. We need to convert it to a map.
    auto headersMap = winrt::single_threaded_map<winrt::hstring, winrt::hstring>();

   // auto vtest = spec.headers.has_value() ? spec.headers.value() : NULL;
    //vtest.TryGetObject()
    if (spec.headers.has_value()) {
      if (auto headersObject = spec.headers.value().TryGetObject()) {
        for (auto const &pair : *headersObject) {
          if (auto value = pair.second.TryGetString()) {
            headersMap.Insert(winrt::to_hstring(pair.first), winrt::to_hstring(*value));
          }
        }
      }
    }

    // Handle body
    winrt::Windows::Storage::Streams::IBuffer bodyBuffer{nullptr};
    if (spec.body.has_value()) {
      auto dataWriter = winrt::Windows::Storage::Streams::DataWriter();
      dataWriter.WriteString(winrt::to_hstring(spec.body.value()));
      bodyBuffer = dataWriter.DetachBuffer();
    }

    winrt::hstring urlString = winrt::to_hstring(spec.url);
    if (spec.query.has_value()) {
      urlString = urlString + L"?" + winrt::to_hstring(spec.query.value());
    }

    return ScriptConfig{
        .scriptId = scriptId,
        .url = winrt::Windows::Foundation::Uri{urlString},
        .method = spec.method,
        .fetch = spec.fetch,
        .absolute = spec.absolute,
        .body = bodyBuffer,
        .headers = headersMap,
        .timeout = winrt::Windows::Foundation::TimeSpan{std::chrono::milliseconds(static_cast<int64_t>(spec.timeout))},
        .verifyScriptSignature = spec.verifyScriptSignature,
        .uniqueId = spec.uniqueId,
        .sourceUrl = spec.url};
  }
};
} // namespace winrt::ScriptManager
