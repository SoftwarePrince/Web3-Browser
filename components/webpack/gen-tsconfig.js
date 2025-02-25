// Copyright (c) 2021 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

const fs = require('fs-extra')
const path = require('path')
const pathMap = require('./path-map')(process.env.ROOT_GEN_DIR)

const srcPath = path.resolve(__dirname, '../../../')
const braveSrcPath = path.join(srcPath, 'brave')

/**
 * Generates a tsconfig.json file in the gen/ directory
 * so that typescript can import files from cthe current build's
 * gen/ directory (e.g. mojom-generated JS).
 *
 * @param {*} [atPath=process.env.ROOT_GEN_DIR]
 * @returns void
 */
 async function createGenTsConfig (atPath = process.env.ROOT_GEN_DIR) {
  const configExtendsFrom = path.relative(
    atPath,
    path.join(braveSrcPath, 'tsconfig-webpack.json')
  )
  const tsConfigPath = path.join(atPath, 'tsconfig-webpack.json')
  // Even though ts-loader will get the paths from webpack for module resolution
  // that does not help some issues where chromium both generates ts definitions
  // and has JSDoc comments for the .m.js file. Sometimes the JSDoc is incorrect
  // whilst the associated .d.ts file has the correct definition. Without specifying
  // the path mapping in the tsconfig.json, Typescript (via ts-loader) will use
  // the JSDoc, and fail with an error. The example that prompted this is cr.sendWithPromise
  // where Typescript will not see that the second parameter is an optional spread param
  // and will fail with an error. Whilst this should be fixed in the chromium source,
  // it's better to be explicit here so that developers get the same experience at
  // both compile and design time.
  const paths = {}
  for (const path in pathMap) {
    paths[`${path}/*`] = [`${pathMap[path]}/*`]
  }
  const config = {
    extends: configExtendsFrom,
    compilerOptions: {
      paths
    },
    references: [
      {
        // This ts project is generated by //ui/webui/resources:library
        path: path.join(atPath, 'ui/webui/resources/tsconfig.json')
      }
    ]
  }
  await fs.writeFile(tsConfigPath, JSON.stringify(config))
  return tsConfigPath
}

createGenTsConfig()
.catch(err => {
  console.error(err)
  process.exit(1)
})
