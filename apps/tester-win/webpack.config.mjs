import path from 'node:path';
import { fileURLToPath } from 'node:url';
import * as Repack from '@callstack/repack';
import TerserPlugin from 'terser-webpack-plugin';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

/**
 * Webpack configuration enhanced with Re.Pack defaults for React Native.
 *
 * Learn about webpack configuration: https://webpack.js.org/configuration/
 * Learn about Re.Pack configuration: https://re-pack.dev/docs/guides/configuration
 */

export default (env) => {
  const { platform } = env;
  const reactNativeWinPath = path.resolve(
    __dirname,
    'node_modules/react-native-windows'
  );

  return {
    context: __dirname,
    entry: './index.js',
    resolve: {
      ...Repack.getResolveOptions(platform),
      alias: {
        // When building for windows, any import of 'react-native' should resolve to 'react-native-macos'
        'react-native':
          platform === 'windows'
            ? reactNativeWinPath
            : path.resolve(__dirname, 'node_modules/react-native'),
      },
    },
    module: {
      rules: [
        {
          test: /\.[cm]?[jt]sx?$/,
          use: 'babel-loader',
          type: 'javascript/auto',
        },
        ...Repack.getAssetTransformRules(),
      ],
    },
    optimization: {
      minimizer: [
        new TerserPlugin({
          test: /\.(js)?bundle(\?.*)?$/i,
          extractComments: false,
          terserOptions: {
            format: {
              comments: false,
            },
          },
        }),
      ],
    },
    plugins: [new Repack.RepackPlugin()],
  };
};
