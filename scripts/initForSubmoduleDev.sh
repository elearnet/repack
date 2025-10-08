 #!/bin/bash

if [ -d "../.yarn" ]; then
  rm -rf ../.yarn_bak
  mv -f ../.yarn ../.yarn_bak
echo 'backed up .yarn'
fi

if [ -f "../.yarnrc.yml" ]; then
  mv -f ../.yarnrc.yml ../.yarnrc.yml.bak
  echo 'backed up .yarnrc.yml'
fi

if [ -f "../yarn.lock" ]; then
  mv -f ../yarn.lock ../yarn.lock.bak
  echo 'backed up yarn.lock'
fi




