#!/bin/bash
#find .. -type d -name "node_modules" -print
find .. -type d -name "node_modules" -exec rm -rf {} +
find .. -type d -name "dist" -exec rm -rf {} +
find .. -type d -name ".nx" -exec rm -rf {} +
echo 'finished'
