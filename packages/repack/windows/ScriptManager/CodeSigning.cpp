#include "pch.h"

#include "CodeSigning.h"
#include "ErrorCodes.h"


#include <vector>

#include "winrt/Windows.Data.Json.h"
#include "winrt/Windows.Security.Cryptography.Core.h"
#include "winrt/Windows.Security.Cryptography.h"
#include "winrt/Windows.Storage.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Security::Cryptography;
using namespace Windows::Security::Cryptography::Core;
using namespace Windows::Data::Json;

namespace CodeSigning {

// Helper to convert Base64URL string to standard Base64
std::string ConvertBase64URLtoBase64(std::string base64url) {
  std::replace(base64url.begin(), base64url.end(), '-', '+');
  std::replace(base64url.begin(), base64url.end(), '_', '/');
  while (base64url.length() % 4) {
    base64url += '=';
  }
  return base64url;
}

// Helper to split a string by a delimiter
std::vector<std::string> split(const std::string &s, char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(s);
  while (std::getline(tokenStream, token, delimiter)) {
    tokens.push_back(token);
  }
  return tokens;
}

BundleAndToken ExtractBundleAndToken(IBuffer const &fileContent) {
  constexpr int signatureSize = 1280;
  const std::string startingSequence = "/* RCSSB */";

  if (fileContent.Length() < signatureSize) {
    return {fileContent, ""};
  }

  auto dataReader = DataReader::FromBuffer(fileContent);
  std::vector<uint8_t> fullData(fileContent.Length());
  dataReader.ReadBytes(fullData);

  std::vector<uint8_t> lastBytes(fullData.end() - signatureSize, fullData.end());
  std::string signatureString(lastBytes.begin(), lastBytes.end());

  if (signatureString.rfind(startingSequence, 0) == 0) {
    // Bundle is signed
    auto bundleSize = fileContent.Length() - signatureSize;
    auto bundleBuffer = Buffer(bundleSize);
    bundleBuffer.Length(bundleSize);
    memcpy_s(bundleBuffer.data(), bundleSize, fullData.data(), bundleSize);

    signatureString.erase(0, startingSequence.length());
    signatureString.erase(std::find(signatureString.begin(), signatureString.end(), '\0'), signatureString.end());

    // Trim whitespace
    signatureString.erase(0, signatureString.find_first_not_of(" \t\n\r"));
    signatureString.erase(signatureString.find_last_not_of(" \t\n\r") + 1);

    return {bundleBuffer, signatureString};
  } else {
    // Bundle is not signed
    return {fileContent, ""};
  }
}

winrt::Windows::Foundation::IAsyncAction VerifyBundle(std::string const &token, IBuffer const &fileContent) {
  if (token.empty()) {
    throw hresult_error(E_FAIL, to_hstring(ErrorCodes::CodeSigningTokenNotFoundError));
  }

  // 1. Get Public Key from app package
  IBuffer keyBuffer{nullptr};
  try {
    auto file = co_await StorageFile::GetFileFromApplicationUriAsync(Uri(L"ms-appx:///RepackPublicKey.pem"));
    auto pemContent = co_await FileIO::ReadTextAsync(file);

    // Basic PEM parsing
    std::string pem = to_string(pemContent);
    pem.erase(0, pem.find("-----BEGIN PUBLIC KEY-----") + 26);
    pem.erase(pem.find("-----END PUBLIC KEY-----"));
    pem.erase(std::remove(pem.begin(), pem.end(), '\n'), pem.end());
    pem.erase(std::remove(pem.begin(), pem.end(), '\r'), pem.end());

    keyBuffer = CryptographicBuffer::DecodeFromBase64String(to_hstring(pem));
  } catch (hresult_error const &) {
    throw hresult_error(E_FAIL, to_hstring(ErrorCodes::CodeSigningPublicKeyNotFoundError));
  }

  auto keyProvider = AsymmetricKeyAlgorithmProvider::OpenAlgorithm(AsymmetricAlgorithmNames::RsaSignPkcs1Sha256());
  CryptographicKey publicKey{nullptr};
  try {
    publicKey = keyProvider.ImportPublicKey(keyBuffer, CryptographicPublicKeyBlobType::Pkcs1RsaPublicKey);
  } catch (hresult_error const &) {
    throw hresult_error(E_FAIL, to_hstring(ErrorCodes::CodeSigningPublicKeyInvalidError));
  }

  // 2. Decode and Verify JWT
  auto jwtParts = split(token, '.');
  if (jwtParts.size() != 3) {
    throw hresult_error(E_FAIL, to_hstring(ErrorCodes::CodeSigningTokenInvalidError));
  }

  auto headerAndPayload = to_hstring(jwtParts[0] + "." + jwtParts[1]);
  auto signatureB64 = ConvertBase64URLtoBase64(jwtParts[2]);
  auto signatureBuffer = CryptographicBuffer::DecodeFromBase64String(to_hstring(signatureB64));

  auto dataToVerify = CryptographicBuffer::ConvertStringToBinary(headerAndPayload, BinaryStringEncoding::Utf8);

  if (!CryptographicEngine::VerifySignature(publicKey, dataToVerify, signatureBuffer)) {
    throw hresult_error(E_FAIL, to_hstring(ErrorCodes::CodeSigningTokenVerificationError));
  }

  // 3. Compare Hashes
  auto payloadJsonString = winrt::Windows::Security::Cryptography::CryptographicBuffer::ConvertBinaryToString(
      winrt::Windows::Security::Cryptography::BinaryStringEncoding::Utf8,
      CryptographicBuffer::DecodeFromBase64String(to_hstring(ConvertBase64URLtoBase64(jwtParts[1]))));
  auto payloadJson = JsonObject::Parse(payloadJsonString);
  auto contentHashFromToken = payloadJson.GetNamedString(L"hash", L"");

  auto hashProvider = HashAlgorithmProvider::OpenAlgorithm(HashAlgorithmNames::Sha256());
  auto fileHashBuffer = hashProvider.HashData(fileContent);
  auto fileHash = to_string(CryptographicBuffer::EncodeToHexString(fileHashBuffer));

  if (to_string(contentHashFromToken) != fileHash) {
    throw hresult_error(E_FAIL, to_hstring(ErrorCodes::CodeSigningBundleVerificationError));
  }
}
} // namespace CodeSigning
