#!/bin/bash
set -euo pipefail
# https://github.com/actions/runner-images/issues/13135

#TOOLCHAIN="$(xcode-select -p)/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/iphoneos"
TOOLCHAIN="$(xcode-select -p)/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/macosx"
#SDK="$(xcrun --sdk iphoneos --show-sdk-path)/usr/lib/swift"
SDK="$(xcrun --sdk macosx --show-sdk-path)/usr/lib/swift"

echo "🔧 Restoring missing Swift Compatibility libraries if needed..."
echo "Toolchain path: $TOOLCHAIN"
echo "SDK path:       $SDK"

# List of possible compatibility libraries to restore
LIBS=(
  libswiftCompatibility56.a
  libswiftCompatibilityConcurrency.a
  )

for LIB in "${LIBS[@]}"; do
  TOOLCHAIN_LIB="$TOOLCHAIN/$LIB"
  SDK_LIB_A="$SDK/$LIB"
  SDK_LIB_TBD="${SDK_LIB_A%.a}.tbd"
  if [ -f "$TOOLCHAIN_LIB" ]; then
    if [ ! -f "$SDK_LIB_A" ] && [ ! -f "$SDK_LIB_TBD" ]; then
      echo "⚡ Creating symlink for $LIB"
      sudo ln -s "$TOOLCHAIN_LIB" "$SDK_LIB_A"
    else
      echo "✅ $LIB already exists in SDK"
    fi
  else
    echo "⚠️ $LIB not found in toolchain"
  fi
done

echo ""
echo "🧩 Final Swift SDK contents:"
ls -l "$SDK" | grep Compatibility || echo "No Compatibility libs found."
echo ""
echo "Checking for missing Apple frameworks..."

#SYSFWK="$(xcrun --sdk iphoneos --show-sdk-path)/System/Library/Frameworks"
SYSFWK="$(xcrun --sdk macosx --show-sdk-path)/System/Library/Frameworks"
for FWK in CoreAudioTypes UIUtilities; do
  if [ ! -d "$SYSFWK/$FWK.framework" ]; then
    echo "⚠️ Framework $FWK not found in $SYSFWK"
  else
    echo "✅ Framework $FWK found"
  fi
done

echo ""
echo "Swift compatibility library repair complete."
