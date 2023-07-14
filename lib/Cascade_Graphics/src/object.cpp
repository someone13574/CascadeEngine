#include "object.hpp"

#include <acorn_logging.hpp>
#include <fstream>

namespace Cascade_Graphics
{
    std::vector<Triangle> Load_Ascii_Stl(std::string path)
    {
        // Open the STL file
        std::ifstream file(path);
        if (!file)
        {
            LOG_ERROR << "Graphics: Failed to open object file at path: " << path;
            exit(EXIT_FAILURE);
        }

        // Parse file line by line
        std::string line;
        std::vector<Triangle> triangles;

        while (std::getline(file, line))
        {
            // Remove leading and trailing white space from the line
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);

            // Check if the line is a "outer loop" line
            if (line.compare(0, 10, "outer loop") == 0)
            {
                Triangle triangle;

                // Read the next three "vertex" lines
                uint32_t read_vertex_lines = 0;
                while (read_vertex_lines != 3 && std::getline(file, line))
                {
                    line.erase(0, line.find_first_not_of(" \t\r\n"));
                    line.erase(line.find_last_not_of(" \t\r\n") + 1);

                    if (line.compare(0, 6, "vertex") != 0)
                    {
                        continue;
                    }

                    switch (read_vertex_lines++)
                    {
                        case 0:
                            std::sscanf(line.c_str(), "vertex %f %f %f", &triangle.point_a.m_x, &triangle.point_a.m_y, &triangle.point_a.m_z);
                            break;
                        case 1:
                            std::sscanf(line.c_str(), "vertex %f %f %f", &triangle.point_b.m_x, &triangle.point_b.m_y, &triangle.point_b.m_z);
                            break;
                        case 2:
                            std::sscanf(line.c_str(), "vertex %f %f %f", &triangle.point_c.m_x, &triangle.point_c.m_y, &triangle.point_c.m_z);
                            break;
                    }
                }

                triangles.push_back(triangle);
            }
        }

        return triangles;
    }
}    // namespace Cascade_Graphics