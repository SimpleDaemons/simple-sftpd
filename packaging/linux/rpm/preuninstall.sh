#!/bin/bash
# Pre-uninstallation script for simple-sftpd RPM

set -e

PROJECT_NAME="simple-sftpd"

# Stop service before removal
if [ "$1" -eq 0 ]; then
    systemctl stop "$simple-sftpd" 2>/dev/null || true
    systemctl disable "$simple-sftpd" 2>/dev/null || true
fi

exit 0

