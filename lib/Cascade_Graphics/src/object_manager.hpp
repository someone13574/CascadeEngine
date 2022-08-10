#pragma once

#include "Data_Types/vector_3.hpp"
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <stack>
#include <string>
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
            std::vector<Voxel> voxels;
        };

    private:
        std::vector<Object> m_objects;

    private:
        static void Voxel_Sample_Volume_Function(Vector_3<double> voxel_position, double voxel_size, double step_size, std::function<double(Vector_3<double>)> volume_sample_function, bool& is_fully_contained, bool& is_intersecting);

        static void Object_From_Volume_Function_Worker_Thread(uint32_t max_depth,
                                                              std::vector<double> step_size_lookup_table,
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

        std::vector<GPU_Voxel> Get_GPU_Voxels();
    };
} // namespace Cascade_Graphics