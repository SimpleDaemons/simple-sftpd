#!/bin/bash
# Post-uninstallation script for simple-sftpd RPM

set -e

# Reload systemd
systemctl daemon-reload

exit 0

