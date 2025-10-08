 #!/bin/bash
 if [ -d "../.yarn" ]; then
   rm -rf ../.yarn
 fi
 mkdir "../.yarn"
 cp -rf ../.yarn_bak/* ../.yarn/

 cp -f ../.yarnrc.yml.bak ../.yarnrc.yml

 cp -f ../yarn.lock.bak ../yarn.lock
