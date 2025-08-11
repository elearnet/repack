#include "pch.h"

#include "ScriptManager.h"

#include <JSI/JsiApiContext.h>
using namespace winrt::Windows::Web::Http;

using namespace facebook::jsi;

namespace winrt::ScriptManager
{

//See https://microsoft.github.io/react-native-windows/docs/native-platform for help writing native modules

void ScriptManager::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

  void ScriptManager::loadScript(
    std::string scriptId,
    ScriptManagerCodegen::ScriptManagerSpec_NormalizedScriptLocator &&scriptConfig,
    ::React::ReactPromise<void> &&result) noexcept {
        try {
      ScriptConfig config = ScriptConfig::FromSpec(scriptId, scriptConfig);

          auto scheme = config.url.SchemeName();
          if (scheme == L"http" || scheme == L"https") {
            if (config.fetch) {
              // Download, then execute from cache
              [this, config, result]() -> IAsyncAction {
                try {
                  co_await DownloadAndCacheAsync(config);
                  co_await ExecuteFromCacheAsync(config, result);
                } catch (hresult_error const &ex) {
                  result.Reject(winrt::Microsoft::ReactNative::ReactError{ to_string(ex.message())});
                }
              }();
            } else {
              // Execute directly from cache
              ExecuteFromCacheAsync(config, result);
            }
          } else if (scheme == L"file") {
            ExecuteFromFileSystemAsync(config, result);
          } else {
            result.Reject({"Scheme in URL is not supported."});
          }
        } catch (hresult_error const &ex) {
          result.Reject({ to_string(ex.message())});
        }
  }

  void ScriptManager::prefetchScript(
      std::string scriptId,
      ScriptManagerCodegen::ScriptManagerSpec_NormalizedScriptLocator &&scriptConfig,
      ::React::ReactPromise<void> &&result) noexcept {
        try {
      ScriptConfig config = ScriptConfig::FromSpec(scriptId, scriptConfig);
          if (!config.fetch) {
        result.Resolve();
            return;
          }

          auto scheme = config.url.SchemeName();
          if (scheme == L"http" || scheme == L"https") {
            [this, config, result]() -> IAsyncAction {
              try {
                co_await DownloadAndCacheAsync(config);
                result.Resolve();
              } catch (hresult_error const &ex) {
                result.Reject({to_string(ex.message())});
              }
            }();
          } else {
            result.Reject({"Scheme in URL is not supported for prefetch."});
          }
        } catch (hresult_error const &ex) {
          result.Reject({ to_string(ex.message())});
        }
  }

  void ScriptManager::invalidateScripts(std::vector<std::string> const & scripts, ::React::ReactPromise<void> &&result) noexcept {
    [this, scripts, result]() -> IAsyncAction {
      try {
        auto scriptsDir = co_await GetScriptsDirectoryAsync();
        if (scripts.empty()) {
          // Delete the entire scripts directory if the array is empty
          co_await scriptsDir.DeleteAsync(winrt::Windows::Storage::StorageDeleteOption::PermanentDelete);
        } else {
          for (auto const &script : scripts) {
            try {
              auto file = co_await scriptsDir.GetFileAsync(to_hstring(script + ".script.bundle"));
              co_await file.DeleteAsync(winrt::Windows::Storage::StorageDeleteOption::PermanentDelete);
            } catch (hresult_error const &ex) {
              // Ignore errors if the file doesn't exist (hresult_not_found)
              if (ex.code() != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
                throw;
              }
            }
          }
        }
        result.Resolve();
      } catch (hresult_error const &ex) {
        result.Reject({to_string(ex.message())});
      }
    }();
  }

  bool ScriptManager::unstable_evaluateScript(std::string scriptSource, std::string scriptSourceUrl) noexcept {
    bool success = false;
    // This must be run synchronously on the JS thread.
    m_context.JSDispatcher().Post([&]() {
      try {

        /*
        //New Arch should use ReactContext.CallInvoker 
        winrt::Microsoft::ReactNative::ExecuteJsi(m_context,[&scriptSource,&scriptSourceUrl](facebook::jsi::Runtime &runtime) { 
            runtime.evaluateJavaScript(std::make_unique<facebook::jsi::StringBuffer>(std::move(scriptSource)), scriptSourceUrl);
        });

        m_context.CallInvoker()->invokeAsync([&scriptSource, &scriptSourceUrl](Runtime &runtime) { 
            runtime.evaluateJavaScript(
              std::make_unique<facebook::jsi::StringBuffer>(std::move(scriptSource)), scriptSourceUrl);
            });
        */

        Runtime *jsiRuntime = winrt::Microsoft::ReactNative::TryGetOrCreateContextRuntime(m_context);
        jsiRuntime->evaluateJavaScript(std::make_unique<facebook::jsi::StringBuffer>(std::move(scriptSource)), scriptSourceUrl);

        success = true;
      } catch (...) {
        // Error is propagated as a JS exception.
        success = false;
      }
    });
    return success;
  }

  // --- Private Helper Method Implementations ---

  IAsyncAction ScriptManager::DownloadAndCacheAsync(ScriptConfig const &config) {
    HttpClient httpClient;
    HttpRequestMessage request(HttpMethod(to_hstring(config.method)), config.url);

    for (auto const &pair : config.headers) {
      request.Headers().Append(pair.Key(), pair.Value());
    }

    if (config.body && (config.method == "POST" || config.method == "PUT" || config.method == "PATCH")) {
      request.Content(HttpBufferContent(config.body));
    }

    auto response = co_await httpClient.SendRequestAsync(request, HttpCompletionOption::ResponseContentRead);

    if (!response.IsSuccessStatusCode()) {
      throw hresult_error(E_FAIL, to_hstring(ErrorCodes::ScriptDownloadFailure));
    }

    auto contentBuffer = co_await response.Content().ReadAsBufferAsync();
    auto [bundle, token] = CodeSigning::ExtractBundleAndToken(contentBuffer);

    if (config.verifyScriptSignature == "strict" || (config.verifyScriptSignature == "lax" && !token.empty())) {
      co_await CodeSigning::VerifyBundle(token, bundle);
    }

    auto scriptFile = co_await GetScriptFileAsync(config.uniqueId);
    co_await winrt::Windows::Storage::FileIO::WriteBufferAsync(scriptFile, bundle);
  }

  IAsyncAction ScriptManager::ExecuteFromCacheAsync(
      ScriptConfig const &config,
      ::React::ReactPromise<void> const &promise) {
    try {
      auto file = co_await GetScriptFileAsync(config.uniqueId);
      auto buffer = co_await winrt::Windows::Storage::FileIO::ReadBufferAsync(file);
      EvaluateJavascript(buffer, config.sourceUrl, promise);
    } catch (hresult_error const &ex) {
      promise.Reject({ to_string(ex.message())});
    }
  }

  IAsyncAction ScriptManager::ExecuteFromFileSystemAsync(
      ScriptConfig const &config,
      ::React::ReactPromise<void> const &promise) {
    try {
      winrt::Windows::Storage::StorageFile file{nullptr};
      if (config.absolute) {
        file = co_await winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(config.url.Path());
      } else {
        file = co_await winrt::Windows::Storage::StorageFile::GetFileFromApplicationUriAsync(config.url);
      }
      auto buffer = co_await winrt::Windows::Storage::FileIO::ReadBufferAsync(file);
      EvaluateJavascript(buffer, config.sourceUrl, promise);
    } catch (hresult_error const &ex) {
     // promise.Reject({to_string(ex.code()), to_string(ex.message())});
      promise.Reject({to_string(ex.message())});
    }
  }

  void ScriptManager::EvaluateJavascript(
      Windows::Storage::Streams::IBuffer const &code,
      std::string url,
      ::React::ReactPromise<void> const &promise) {
    std::string source(reinterpret_cast<const char *>(code.data()), code.Length());

    m_context.JSDispatcher().Post([this, source = std::move(source), url = std::move(url), promise]() {
      try {
        Runtime *jsiRuntime = winrt::Microsoft::ReactNative::TryGetOrCreateContextRuntime(m_context);
        jsiRuntime->evaluateJavaScript(std::make_unique<facebook::jsi::StringBuffer>(source), url);
        promise.Resolve();
      } catch (const facebook::jsi::JSIException &e) {
        promise.Reject({ErrorCodes::CodeExecutionFailure, e.what()});
      }
    });
  }

  IAsyncOperation<winrt::Windows::Storage::StorageFolder> ScriptManager::GetScriptsDirectoryAsync() {
    auto localCache = winrt::Windows::Storage::ApplicationData::Current().LocalCacheFolder();
    return localCache.CreateFolderAsync(L"scripts", winrt::Windows::Storage::CreationCollisionOption::OpenIfExists);
  }

  IAsyncOperation<winrt::Windows::Storage::StorageFile> ScriptManager::GetScriptFileAsync(std::string scriptUniqueId) {
    auto scriptsDir = co_await GetScriptsDirectoryAsync();
    auto fileName = to_hstring(scriptUniqueId + ".script.bundle");
    // Use ReplaceExisting to ensure we always write the latest version.
    co_return co_await scriptsDir.CreateFileAsync(
        fileName, winrt::Windows::Storage::CreationCollisionOption::ReplaceExisting);
  }

} // namespace winrt::ScriptManager
