#!/usr/bin/env bash
set -e

npx markdownlint-cli2 --fix "**/*.md"
npx markdownlint-cli2 "**/*.md"
