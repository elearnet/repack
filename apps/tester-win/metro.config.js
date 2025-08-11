const path = require('node:path');
const fs = require('node:fs');
const exclusionList = require('metro-config/src/defaults/exclusionList');
const rnwPath = fs.realpathSync(
  path.resolve(require.resolve('react-native-windows/package.json'), '..')
);
const {
  getMetroTools,
  getMetroAndroidAssetsResolutionFix,
} = require('react-native-monorepo-tools');
const monorepoMetroTools = getMetroTools();
const androidAssetsResolutionFix = getMetroAndroidAssetsResolutionFix();
const { getDefaultConfig, mergeConfig } = require('@react-native/metro-config');
const defaultConfig = getDefaultConfig(__dirname);

const {
  resolver: { sourceExts, assetExts },
} = getDefaultConfig(__dirname);
const _watchFolders = monorepoMetroTools.watchFolders;
//_watchFolders.push(path.resolve(__dirname, '../../libs/react-native/packages/react-native'));
const _extraNodeModules = monorepoMetroTools.extraNodeModules;
//_extraNodeModules["react-native"] = path.resolve(__dirname, '../../libs/react-native/packages/react-native');

const config = {
  transformer: {
    publicPath: androidAssetsResolutionFix.publicPath,
    getTransformOptions: async () => ({
      transform: {
        experimentalImportSupport: false,
        inlineRequires: true,
      },
    }),
  },
  server: {
    enhanceMiddleware: (middleware) => {
      return androidAssetsResolutionFix.applyMiddleware(middleware);
    },
  },
  watchFolders: _watchFolders,
  resolver: {
    extraNodeModules: _extraNodeModules,
    assetExts: assetExts.filter((ext) => ext !== 'svg'),
    sourceExts: [...sourceExts, 'svg'],
    blockList: exclusionList([
      // This stops "npx @react-native-community/cli run-windows" from causing the metro server to crash if its already running
      new RegExp(
        `${path.resolve(__dirname, 'windows').replace(/[/\\]/g, '/')}.*`
      ),
      // This prevents "npx @react-native-community/cli run-windows" from hitting: EBUSY: resource busy or locked, open msbuild.ProjectImports.zip or other files produced by msbuild
      new RegExp(`${rnwPath}/build/.*`),
      new RegExp(`${rnwPath}/target/.*`),
      /.*\.ProjectImports\.zip/,
    ]),
    //
  },
};

module.exports = mergeConfig(defaultConfig, config);
