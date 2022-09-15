#pragma once

#include "Data_Types/vector_3.hpp"
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <stack>
#include <string>
#include <utility>
#include <vector>


namespace Cascade_Graphics
{
    class Object_Manager
    {
    public:
        struct GPU_Voxel
        {
            float position_x;
            float position_y;
            float position_z;
            float size;

            uint32_t hit_links[8];
            uint32_t miss_links[8];

            float normal_x;
            float normal_y;
            float normal_z;

            float color_r;
            float color_g;
            float color_b;

            uint32_t padding_a;
            uint32_t padding_b;
        };

        struct GPU_Object
        {
            float object_to_world_matrix_x0;
            float object_to_world_matrix_x1;
            float object_to_world_matrix_x2;
            float object_to_world_matrix_x3;

            float object_to_world_matrix_y0;
            float object_to_world_matrix_y1;
            float object_to_world_matrix_y2;
            float object_to_world_matrix_y3;

            float object_to_world_matrix_z0;
            float object_to_world_matrix_z1;
            float object_to_world_matrix_z2;
            float object_to_world_matrix_z3;

            uint32_t root_voxel_index;

            uint32_t padding_a;
            uint32_t padding_b;
            uint32_t padding_c;
        };

    private:
        struct Voxel
        {
            double size;
            Vector_3<double> position;
            Vector_3<double> normal;
            Vector_3<double> color;

            uint32_t parent_index;
            uint32_t child_indices[8] = {0, 0, 0, 0, 0, 0, 0, 0};
            uint32_t child_index;
            uint32_t hit_links[8];
            uint32_t miss_links[8];

            uint32_t depth;
            bool is_leaf;
        };

        struct Object
        {
            std::string label;

            Vector_3<double> position;
            Vector_3<double> scale;
            Vector_3<double> rotation;

            std::vector<Voxel> voxels;
        };

    private:
        std::vector<Object> m_objects;
        std::vector<GPU_Object> m_gpu_objects;
        std::vector<GPU_Voxel> m_gpu_voxels;

    private:
        static void
        Voxel_Sample_Volume_Function(Vector_3<double> voxel_position, double voxel_size, double step_size, uint32_t step_count, std::function<double(Vector_3<double>)> volume_sample_function, bool& is_fully_contained, bool& is_intersecting);

        static void Object_From_Volume_Function_Worker_Thread(uint32_t max_depth,
                                                              std::vector<uint32_t> step_count_lookup_table,
                                                              double step_size,
                                                              uint32_t worker_index,
                                                              std::function<double(Vector_3<double>)> volume_sample_function,
                                                              std::function<Vector_3<double>(Vector_3<double>, Vector_3<double>)> color_sample_function,
                                                              std::vector<Voxel>* voxels_ptr,
                                                              std::mutex* voxels_mutex_ptr,
                                                              bool* work_complete_ptr,
                                                              std::mutex* work_complete_mutex_ptr,
                                                              uint32_t* active_workers_count_ptr,
                                                              std::queue<uint32_t>* available_workers_queue_ptr,
                                                              std::mutex* available_workers_queue_mutex_ptr,
                                                              std::condition_variable* available_worker_notify_ptr,
                                                              std::stack<uint32_t>* leaf_node_stack_ptr,
                                                              std::mutex* leaf_node_stack_mutex_ptr,
                                                              std::condition_variable* available_leaf_node_notify_ptr,
                                                              bool* work_available_ptr,
                                                              uint32_t* current_voxel_ptr,
                                                              std::mutex* data_mutex_ptr,
                                                              std::condition_variable* work_notify_ptr);

    public:
        Object_Manager();

    public:
        void Create_Object_From_Volume_Function(std::string label,
                                                uint32_t max_depth,
                                                Vector_3<double> sample_region_center,
                                                double sample_region_size,
                                                std::function<double(Vector_3<double>)> volume_sample_function,
                                                std::function<Vector_3<double>(Vector_3<double>, Vector_3<double>)> color_sample_function);

        Object* Get_Object(std::string label);
        std::vector<GPU_Object> Get_GPU_Objects();
        std::vector<GPU_Voxel> Get_GPU_Voxels();
    };
} // namespace Cascade_Graphics