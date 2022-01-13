#ifndef TOOL_H
#define TOOL_H

#include <string>
#include <list>

/**
 * @brief The Tool class
 *
 * Helper to read ECG file in directory.
 * Full QT code because STL does not yet provided filesystem support.
 */

class Tool
{
public:
    static bool listECGFilesInDirectory(const std::string& dir, std::list<std::string>& paths);
};

#endif // TOOL_H
