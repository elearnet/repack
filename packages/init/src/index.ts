import addDependencies from './tasks/addDependencies.js';
import checkPackageManager from './tasks/checkPackageManager.js';
import checkReactNative from './tasks/checkReactNative.js';
import createBundlerConfig from './tasks/createBundlerConfig.js';
import ensureProjectExists from './tasks/ensureProjectExists.js';
import handleReactNativeConfig from './tasks/handleReactNativeConfig.js';
import modifyIOS from './tasks/modifyIOS.js';
import selectBundler from './tasks/selectBundler.js';

import logger, { enableVerboseLogging } from './utils/logger.js';

interface Options {
  bundler: 'rspack' | 'webpack';
  entry: string;
  repackVersion?: string;
  templateType: 'mjs' | 'cjs';
  verbose: boolean;
}

export default async function run({
  bundler,
  entry,
  repackVersion,
  templateType,
  verbose,
}: Options) {
  if (verbose) {
    enableVerboseLogging();
  }

  try {
    const { cwd, rootDir } = await ensureProjectExists();
    const packageManager = await checkPackageManager(rootDir);

    checkReactNative(cwd);

    if (!bundler) {
      bundler = await selectBundler();
    }

    await addDependencies(bundler, cwd, packageManager, repackVersion);

    await createBundlerConfig(bundler, cwd, templateType, entry);

    handleReactNativeConfig(bundler, cwd);

    modifyIOS(cwd);

    logger.done('Setup complete. Thanks for using Re.Pack!');
  } catch (error) {
    logger.fatal('Re.Pack setup failed\n\nWhat went wrong:');

    if (error instanceof Error) {
      logger.error(error.message);
    } else {
      logger.error(error as any);
    }

    process.exit(1);
  }
}
