#pragma once

#include "pch.h"
#include "resource.h"

#if __has_include("codegen/NativeScriptManagerDataTypes.g.h")
  #include "codegen/NativeScriptManagerDataTypes.g.h"
#endif
#include "codegen/NativeScriptManagerSpec.g.h"

#include "NativeModules.h"
#include "ScriptConfig.h"
#include "ErrorCodes.h"
#include "CodeSigning.h"

namespace winrt::ScriptManager
{

// See https://microsoft.github.io/react-native-windows/docs/native-platform for help writing native modules

REACT_MODULE(ScriptManager)
struct ScriptManager
{
  using ModuleSpec = ScriptManagerCodegen::ScriptManagerSpec;

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

//  REACT_SYNC_METHOD(multiply)
//  double multiply(double a, double b) noexcept;
  REACT_METHOD(loadScript)
  void loadScript(std::string scriptId, ScriptManagerCodegen::ScriptManagerSpec_NormalizedScriptLocator && scriptConfig, ::React::ReactPromise<void> &&result) noexcept;

  REACT_METHOD(prefetchScript)
  void prefetchScript(
      std::string scriptId,
      ScriptManagerCodegen::ScriptManagerSpec_NormalizedScriptLocator &&scriptConfig,
      ::React::ReactPromise<void> &&result) noexcept;

  REACT_METHOD(invalidateScripts)
  void invalidateScripts(std::vector<std::string> const & scripts, ::React::ReactPromise<void> &&result) noexcept;

  REACT_SYNC_METHOD(unstable_evaluateScript)
  bool unstable_evaluateScript(std::string scriptSource, std::string scriptSourceUrl) noexcept;

private:
  React::ReactContext m_context;

  // --- helpers ---
  winrt::Windows::Foundation::IAsyncAction DownloadAndCacheAsync(ScriptConfig const &config);
  winrt::Windows::Foundation::IAsyncAction ExecuteFromCacheAsync(ScriptConfig const &config,::React::ReactPromise<void> const &promise);
  winrt::Windows::Foundation::IAsyncAction ExecuteFromFileSystemAsync(ScriptConfig const &config,::React::ReactPromise<void> const &promise);
  void EvaluateJavascript(winrt::Windows::Storage::Streams::IBuffer const &code,std::string url,::React::ReactPromise<void> const &promise);
  winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::StorageFolder> GetScriptsDirectoryAsync();
  winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::StorageFile> GetScriptFileAsync(std::string scriptUniqueId);
};

} // namespace winrt::ScriptManager
