<p align="center">
    <img src="https://raw.githubusercontent.com/callstack/repack/HEAD/logo.png">
</p>
<p align="center">
A toolkit to build your React Native application with Rspack or Webpack.
</p>

---

[![Build Status][build-badge]][build]
[![Version][version-badge]][version]
[![MIT License][license-badge]][license]
[![PRs Welcome][prs-welcome-badge]][prs-welcome]
[![Code of Conduct][coc-badge]][coc]

`@callstack/repack-plugin-expo-modules` is a plugin for `@callstack/repack` that compliments the integration of Expo Modules into your React Native projects.

## About

This plugin helps and compliments the process of enabling Expo Modules in Re.Pack projects by defining necessary globals that are expected by Expo Modules at runtime. However, it is not sufficient on its own for a complete setup. For comprehensive guidance on using Expo Modules with Re.Pack, please refer to our [official documentation](https://re-pack.dev/).

## Installation

```sh
npm install -D @callstack/repack-plugin-expo-modules
```

## Usage

### Plugin

To add the plugin to your Re.Pack configuration, update your `rspack.config.js` or `webpack.config.js` as follows:

```js
import { ExpoModulesPlugin } from "@callstack/repack-plugin-expo-modules";

export default (env) => {
  // ...
  return {
    // ...
    plugins: [
      // ...
      new ExpoModulesPlugin(),
    ],
  };
};
```

---

Check out our website at https://re-pack.dev for more info and documentation or our GitHub: https://github.com/callstack/repack.

<!-- badges -->

[callstack-readme-with-love]: https://callstack.com/?utm_source=github.com&utm_medium=referral&utm_campaign=react-native-paper&utm_term=readme-with-love
[build-badge]: https://img.shields.io/github/workflow/status/callstack/repack/CI/main?style=flat-square
[build]: https://github.com/callstack/repack/actions/workflows/main.yml
[version-badge]: https://img.shields.io/npm/v/@callstack/repack-plugin-expo-modules?style=flat-square
[version]: https://www.npmjs.com/package/@callstack/repack-plugin-expo-modules
[license-badge]: https://img.shields.io/npm/l/@callstack/repack-plugin-expo-modules?style=flat-square
[license]: https://github.com/callstack/repack/blob/master/LICENSE
[prs-welcome-badge]: https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=flat-square
[prs-welcome]: ./CONTRIBUTING.md
[coc-badge]: https://img.shields.io/badge/code%20of-conduct-ff69b4.svg?style=flat-square
[coc]: https://github.com/callstack/repack/blob/master/CODE_OF_CONDUCT.md
