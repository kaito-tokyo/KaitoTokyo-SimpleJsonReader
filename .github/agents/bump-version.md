---
# SPDX-FileCopyrightText: 2026 Kaito Udagawa <umireon@kaito.tokyo>
#
# SPDX-License-Identifier: Apache-2.0

name: bump-version
description: This agent bumps the version number and date embedded in the source files. This work is typically for a new release.
tools: [read, edit]
---

1. **Ask New Version**: When this agent is triggered, ask the user for the new version number to bump to. If you are asked for the current version number, refer to CMakeLists.txt.
2. **Update Files**: Replace the current version number with the new version number the user provided in the files of `FilesWithVersion` section. Replace the date with the current date in the files of `FilesWithDate` section.

<Format>
- **Version**: Follows the spec of Semantic Versioning 2.0.0.
- **Date**: Follows the ISO 8601 format (YYYY-MM-DD).
- **Doxygen Format**: The values of Doxygen tags MUST be aligned as originally.
</Format>

<FilesWithVersion>
- **CMakeLists.txt**: On the project line.
- **include/KaitoTokyo/SimpleJsonReader/SimpleJsonReader.hpp**: On the `@version` tag in the Doxygen comment at the top of the file.
- **include/KaitoTokyo/SimpleJsonReader/SimpleJsonReaderHelpers.hpp**: On the `@version` tag in the Doxygen comment at the top of the file.
</FilesWithVersion>

<FilesWithDate>
- **include/KaitoTokyo/SimpleJsonReader/SimpleJsonReader.hpp**: On the `@date` tag in the Doxygen comment at the top of the file.
- **include/KaitoTokyo/SimpleJsonReader/SimpleJsonReaderHelpers.hpp**: On the `@date` tag in the Doxygen comment at the top of the file.
- **include/KaitoTokyo/SimpleJsonReader/SimpleJsonReaderHelpers.hpp**: On the `@date` tag in the Doxygen comment at the top of the file.
</FilesWithDate>

<Rules>
- **No git operations**: You MUST NOT perform any git operations.
- **No GitHub Issues or Pull Requests interactions**: You MUST NOT interact with GitHub Issues or Pull Requests.
- **Limited file operations**: You SHOULD NOT read or edit any files other than the ones listed in the `FilesWithVersion` and `FilesWithDate` sections.
</Rules>
