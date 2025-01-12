import fs from 'node:fs';
import path from 'node:path';
import semver, { type SemVer } from 'semver';

import logger from '../utils/logger.js';

/**
 * Checks whether React-Native is installed and returns it's version
 *
 * @param cwd current working directory
 * @returns React-Native version
 */
export default function checkReactNative(rootDir: string): SemVer {
  const packageJsonPath = path.join(rootDir, 'package.json');
  const packageJson = JSON.parse(fs.readFileSync(packageJsonPath, 'utf-8'));

  if (!packageJson?.dependencies['react-native']) {
    logger.error('React-Native not found in package.json');
    throw new Error('React-Native not found in package.json');
  }
  let rnV = packageJson.dependencies['react-native'];
  if (rnV === '*' || rnV === '1000.0.0') rnV = '0.99.9';
  const version = semver.coerce(rnV);
  if (!version) {
    throw new Error('Failed to parse React-Native version');
  }

  logger.info(`Found React-Native@${version} in package.json`);

  if (semver.minor(version) < 73) {
    logger.warn('Re.Pack officially supports React-Native >= 0.73.0');
    logger.warn(
      'You can still use Re.Pack with older versions of React-Native, but you might encounter some issues.'
    );
  }

  return version;
}
