#pragma once

#include <string>
#include "winrt/Windows.Foundation.h"
#include "winrt/Windows.Storage.Streams.h"

namespace CodeSigning {
struct BundleAndToken {
  winrt::Windows::Storage::Streams::IBuffer bundle{nullptr};
  std::string token;
};

BundleAndToken ExtractBundleAndToken(winrt::Windows::Storage::Streams::IBuffer const &fileContent);

winrt::Windows::Foundation::IAsyncAction VerifyBundle(
    std::string const &token,
    winrt::Windows::Storage::Streams::IBuffer const &fileContent);
} // namespace CodeSigning
