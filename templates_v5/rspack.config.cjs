const path = require('path');
const Repack = require('@callstack/repack');

/**
 * More documentation, installation, usage, motivation and differences with Metro is available at:
 * https://github.com/callstack/repack/blob/main/README.md
 *
 * The API documentation for the functions and plugins used in this file is available at:
 * https://re-pack.dev
 */

/**
 * Webpack configuration.
 * You can also export a static object or a function returning a Promise.
 *
 * @param env Environment options passed from either Webpack CLI or React Native Community CLI
 *            when running with `react-native start/bundle`.
 */
module.exports = (env) => {
  const {
    mode = 'development',
    context = __dirname,
    entry = './index.js',
    platform = process.env.PLATFORM,
  } = env;

  if (!platform) {
    throw new Error('Missing platform');
  }

  return {
    mode,
    context,
    entry,
    resolve: {
      ...Repack.getResolveOptions(),
    },
    module: {
      /**
       * This rule will process all React Native related dependencies with Babel.
       * If you have a 3rd-party dependency that you need to transpile, you can add it to the
       * `include` list.
       *
       * You can also enable persistent caching with `cacheDirectory` - please refer to:
       * https://github.com/babel/babel-loader#options
       */
      rules: [
        Repack.REACT_NATIVE_LOADING_RULES,
        Repack.NODE_MODULES_LOADING_RULES,
        Repack.FLOW_TYPED_MODULES_LOADING_RULES,
        /** Here you can adjust loader that will process your files. */
        {
          test: /\.[jt]sx?$/,
          exclude: [/node_modules/],
          type: 'javascript/auto',
          use: {
            loader: 'builtin:swc-loader',
            options: {
              env: {
                targets: {
                  'react-native': '0.74',
                },
              },
              jsc: {
                assumptions: {
                  setPublicClassFields: true,
                  privateFieldsAsProperties: true,
                },
                externalHelpers: true,
                transform: {
                  react: {
                    runtime: 'automatic',
                    development: mode === 'development',
                  },
                },
              },
            },
          },
        },
        {
          test: Repack.getAssetExtensionsRegExp(),
          use: '@callstack/repack/assets-loader',
        },
      ],
    },
    plugins: [new Repack.RepackPlugin()],
  };
};
