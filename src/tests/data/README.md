# Test Data Directory

This directory contains test data files used by the Simple FTP Daemon test suite.

## Directory Structure

```
data/
├── README.md              # This file
├── files/                 # Test files for transfer operations
│   ├── small.txt         # Small text file (1 KB)
│   ├── medium.txt        # Medium text file (1 MB)
│   └── large.txt         # Large text file (100 MB)
├── directories/           # Test directory structures
│   ├── empty/            # Empty directory
│   ├── nested/           # Nested directory structure
│   │   ├── level1/       # First level
│   │   │   ├── level2/   # Second level
│   │   │   └── file.txt  # File in nested directory
│   │   └── file.txt      # File in first level
│   └── mixed/            # Directory with mixed content
│       ├── file1.txt     # Regular file
│       ├── file2.txt     # Another regular file
│       └── subdir/       # Subdirectory
├── permissions/           # Files with different permissions
│   ├── readable.txt      # Readable file
│   ├── writable.txt      # Writable file
│   └── executable.txt    # Executable file
└── special/              # Special files and edge cases
    ├── hidden.txt        # Hidden file (.hidden.txt)
    ├── spaces.txt        # File with spaces in name
    └── unicode.txt       # File with unicode characters
```

## Test File Contents

### small.txt
A small text file containing basic ASCII content for testing small file transfers.

### medium.txt
A medium-sized text file for testing moderate transfer operations and performance.

### large.txt
A large file for testing transfer limits, memory usage, and performance under load.

## Usage

Test files are automatically copied to the build directory during CMake configuration.
Tests can reference these files using the `TEST_DATA_DIR` macro defined in `test_config.h`.

## Adding New Test Data

When adding new test data:

1. Place files in the appropriate subdirectory
2. Update this README to document the new files
3. Ensure files are included in the CMake configuration
4. Add corresponding tests that use the new data

## File Permissions

Test files should have appropriate permissions for testing:
- Regular files: 644 (rw-r--r--)
- Executable files: 755 (rwxr-xr-x)
- Directories: 755 (rwxr-xr-x)

## Notes

- Keep test files reasonably sized to avoid long test execution times
- Use deterministic content for reproducible tests
- Consider cleanup procedures for tests that modify files
- Document any special requirements or dependencies
