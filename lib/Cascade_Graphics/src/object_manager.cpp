#include "object_manager.hpp"

#include "cascade_logging.hpp"

#include <sstream>

namespace Cascade_Graphics
{
    Object_Manager::Object_Manager()
    {
    }

    bool Object_Manager::Does_Object_Exist(std::string label)
    {
        for (unsigned int i = 0; i < m_objects.size(); i++)
        {
            if (m_objects[i].Get_Label() == label)
            {
                return true;
            }
        }

        return false;
    }

    std::string Object_Manager::Add_Postfix(std::string label)
    {
        unsigned int postfix_index = 1;

        while (true)
        {
            std::ostringstream string_stream;
            string_stream << label << "-" << postfix_index;

            if (Does_Object_Exist(string_stream.str()))
            {
                postfix_index++;
            }
            else
            {
                return string_stream.str();
            }
        }

        return "";
    }

    void Object_Manager::Create_Object_From_Sample_Function(std::string label, uint8_t max_depth, Vector_3<double> boundary_min, Vector_3<double> boundary_max, std::function<bool(Vector_3<double>)> volume_sample_function)
    {
        if (Does_Object_Exist(label))
        {
            std::string new_label = Add_Postfix(label);

            LOG_WARN << "Graphics: Object label '" << label << "' already in use, using label '" << new_label << "'";

            label = new_label;
        }

        m_objects.push_back(Object(label));

        m_objects.back().Create_Object_Sample_Function(max_depth, boundary_min, boundary_max, volume_sample_function);
    }

    std::vector<Object::GPU_Voxel> Object_Manager::Get_GPU_Voxels()
    {
        std::vector<Object::GPU_Voxel> gpu_voxels;
        for (unsigned int i = 0; i < m_objects.size(); i++)
        {
            std::vector<Object::GPU_Voxel> object_gpu_voxels = m_objects[i].Get_GPU_Voxels();

            gpu_voxels.insert(gpu_voxels.end(), object_gpu_voxels.begin(), object_gpu_voxels.end());
        }

        return gpu_voxels;

        // Cascade_Graphics::Object::GPU_Voxel gpu_voxel_a = {};
        // gpu_voxel_a.x_position = -0.5;
        // gpu_voxel_a.y_position = 0.0;
        // gpu_voxel_a.z_position = 0.0;
        // gpu_voxel_a.size = 0.25;

        // Cascade_Graphics::Object::GPU_Voxel gpu_voxel_b = {};
        // gpu_voxel_b.x_position = 0.5;
        // gpu_voxel_b.y_position = 0.0;
        // gpu_voxel_b.z_position = 0.0;
        // gpu_voxel_b.size = 0.25;

        // return {gpu_voxel_a, gpu_voxel_b};
    }
} // namespace Cascade_Graphics