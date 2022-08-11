#include "object_manager.hpp"

#include "cascade_logging.hpp"
#include <array>
#include <thread>
#include <utility>

namespace Cascade_Graphics
{
    Object_Manager::Object_Manager()
    {
    }

    void Object_Manager::Voxel_Sample_Volume_Function(Vector_3<double> voxel_position, double voxel_size, double step_size, std::function<double(Vector_3<double>)> volume_sample_function, bool& is_fully_contained, bool& is_intersecting)
    {
        is_fully_contained = true;
        is_intersecting = false;

        Vector_3<double> start_position = voxel_position - Vector_3<double>(voxel_size, voxel_size, voxel_size);
        Vector_3<double> end_position = voxel_position + Vector_3<double>(voxel_size, voxel_size, voxel_size);
        Vector_3<double> sample_position;

        bool sample;

        for (sample_position.m_x = start_position.m_x; sample_position.m_x <= end_position.m_x; sample_position.m_x += step_size)
        {
            for (sample_position.m_y = start_position.m_y; sample_position.m_y <= end_position.m_y; sample_position.m_y += step_size)
            {
                for (sample_position.m_z = start_position.m_z; sample_position.m_z <= end_position.m_z; sample_position.m_z += step_size)
                {
                    sample = volume_sample_function(sample_position) < 0.0;

                    is_fully_contained = is_fully_contained && sample;
                    is_intersecting = is_intersecting || sample;

                    if ((!is_fully_contained) && is_intersecting)
                    {
                        return;
                    }
                }
            }
        }
    }

    void Object_Manager::Object_From_Volume_Function_Worker_Thread(uint32_t max_depth,
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
                                                                   std::condition_variable* work_notify_ptr)
    {
        static const uint32_t link_order_lookup[8][8]
            = {{0, 1, 4, 2, 5, 3, 6, 7}, {1, 5, 0, 3, 4, 7, 2, 6}, {2, 3, 6, 7, 0, 1, 4, 5}, {3, 7, 2, 6, 1, 5, 0, 4}, {4, 0, 5, 6, 1, 2, 7, 3}, {5, 4, 1, 7, 0, 6, 3, 2}, {6, 2, 7, 3, 4, 0, 5, 1}, {7, 6, 3, 2, 5, 4, 1, 0}};

        static const uint32_t inverse_link_order_lookup[8][8]
            = {{0, 1, 3, 5, 2, 4, 6, 7}, {2, 0, 6, 3, 4, 1, 7, 5}, {4, 5, 0, 1, 6, 7, 2, 3}, {6, 4, 2, 0, 7, 5, 3, 1}, {1, 4, 5, 7, 0, 2, 3, 6}, {4, 2, 7, 6, 1, 0, 5, 3}, {5, 7, 1, 3, 4, 6, 0, 2}, {7, 6, 3, 2, 5, 4, 1, 0}};

        Voxel current_voxel;


        while (true)
        {
            {
                std::lock_guard<std::mutex> available_workers_lock(*available_workers_queue_mutex_ptr);

                available_workers_queue_ptr->push(worker_index);
                available_worker_notify_ptr->notify_all();
            }

            std::unique_lock<std::mutex> lock(*data_mutex_ptr);
            work_notify_ptr->wait(lock, [work_available_ptr] { return *work_available_ptr; });

            {
                std::lock_guard<std::mutex> work_complete_lock(*work_complete_mutex_ptr);

                if (*work_complete_ptr)
                {
                    return;
                }
            }
            {
                std::lock_guard<std::mutex> voxels_lock(*voxels_mutex_ptr);
                current_voxel = (*voxels_ptr)[*current_voxel_ptr];
            }

            if (current_voxel.depth != max_depth)
            {
                for (uint32_t i = 0; i < 8; i++)
                {
                    Voxel child_voxel = {};
                    child_voxel.size = current_voxel.size * 0.5;
                    child_voxel.position = current_voxel.position + Vector_3<double>((i & 1) * current_voxel.size - child_voxel.size, ((i & 2) >> 1) * current_voxel.size - child_voxel.size, ((i & 4) >> 2) * current_voxel.size - child_voxel.size);

                    bool is_fully_contained;
                    bool is_intersecting;
                    Voxel_Sample_Volume_Function(child_voxel.position, child_voxel.size, step_size_lookup_table[child_voxel.depth], volume_sample_function, is_fully_contained, is_intersecting);

                    if (is_intersecting && !is_fully_contained)
                    {
                        child_voxel.parent_index = *current_voxel_ptr;
                        child_voxel.child_indices[0] = 0;
                        child_voxel.child_indices[1] = 0;
                        child_voxel.child_indices[2] = 0;
                        child_voxel.child_indices[3] = 0;
                        child_voxel.child_indices[4] = 0;
                        child_voxel.child_indices[5] = 0;
                        child_voxel.child_indices[6] = 0;
                        child_voxel.child_indices[7] = 0;
                        child_voxel.child_index = i;
                        child_voxel.hit_links[0] = -1;
                        child_voxel.hit_links[1] = -1;
                        child_voxel.hit_links[2] = -1;
                        child_voxel.hit_links[3] = -1;
                        child_voxel.hit_links[4] = -1;
                        child_voxel.hit_links[5] = -1;
                        child_voxel.hit_links[6] = -1;
                        child_voxel.hit_links[7] = -1;
                        child_voxel.miss_links[0] = current_voxel.miss_links[0];
                        child_voxel.miss_links[1] = current_voxel.miss_links[1];
                        child_voxel.miss_links[2] = current_voxel.miss_links[2];
                        child_voxel.miss_links[3] = current_voxel.miss_links[3];
                        child_voxel.miss_links[4] = current_voxel.miss_links[4];
                        child_voxel.miss_links[5] = current_voxel.miss_links[5];
                        child_voxel.miss_links[6] = current_voxel.miss_links[6];
                        child_voxel.miss_links[7] = current_voxel.miss_links[7];
                        child_voxel.depth = current_voxel.depth + 1;

                        double center_density = volume_sample_function(child_voxel.position);
                        double x_density = volume_sample_function(child_voxel.position - Vector_3<double>(0.00001, 0.0, 0.0));
                        double y_density = volume_sample_function(child_voxel.position - Vector_3<double>(0.0, 0.00001, 0.0));
                        double z_density = volume_sample_function(child_voxel.position - Vector_3<double>(0.0, 0.0, 0.00001));
                        child_voxel.normal = (Vector_3<double>(center_density, center_density, center_density) - Vector_3<double>(x_density, y_density, z_density)).Normalized();

                        child_voxel.color = color_sample_function(child_voxel.position, child_voxel.normal);
                        child_voxel.is_leaf = child_voxel.depth == max_depth;

                        {
                            std::lock_guard<std::mutex> voxels_lock(*voxels_mutex_ptr);

                            current_voxel.child_indices[i] = static_cast<uint32_t>(voxels_ptr->size());
                            voxels_ptr->push_back(child_voxel);
                        }
                    }
                    else
                    {
                        current_voxel.child_indices[i] = -1;
                    }
                }

                for (uint32_t direction_index = 0; direction_index < 8; direction_index++)
                {
                    for (uint32_t link_index = 0; link_index < 8; link_index++)
                    {
                        uint32_t link = current_voxel.child_indices[link_order_lookup[direction_index][link_index]];

                        if (link != -1)
                        {
                            current_voxel.hit_links[direction_index] = link;
                            break;
                        }
                    }
                }
            }

            {
                std::lock_guard<std::mutex> voxels_lock(*voxels_mutex_ptr);

                (*voxels_ptr)[*current_voxel_ptr] = current_voxel;
            }

            if (current_voxel.depth != max_depth)
            {
                std::lock_guard<std::mutex> leaf_node_stack_lock(*leaf_node_stack_mutex_ptr);
                std::lock_guard<std::mutex> voxels_lock(*voxels_mutex_ptr);

                bool added_leaf_node = false;
                for (uint32_t i = 0; i < 8; i++)
                {
                    if (current_voxel.child_indices[i] != -1)
                    {
                        for (uint32_t direction_index = 0; direction_index < 8; direction_index++)
                        {
                            for (uint32_t link_index = inverse_link_order_lookup[direction_index][i] + 1; link_index < 8; link_index++)
                            {
                                uint32_t link = current_voxel.child_indices[link_order_lookup[direction_index][link_index]];

                                if (link != -1)
                                {
                                    (*voxels_ptr)[current_voxel.child_indices[i]].miss_links[direction_index] = link;
                                    break;
                                }
                            }
                        }

                        if (!(*voxels_ptr)[current_voxel.child_indices[i]].is_leaf)
                        {
                            leaf_node_stack_ptr->push(current_voxel.child_indices[i]);
                            added_leaf_node = true;
                        }
                    }
                }

                if (added_leaf_node)
                {
                    available_leaf_node_notify_ptr->notify_all();
                }
            }

            {
                std::lock_guard<std::mutex> available_workers_lock(*available_workers_queue_mutex_ptr);
                std::lock_guard<std::mutex> leaf_node_stack_lock(*leaf_node_stack_mutex_ptr);

                (*active_workers_count_ptr)--;

                if (leaf_node_stack_ptr->empty() && (*active_workers_count_ptr) == 0)
                {
                    std::lock_guard<std::mutex> work_complete_lock(*work_complete_mutex_ptr);

                    *work_complete_ptr = true;

                    available_workers_queue_ptr->push(-1);
                    leaf_node_stack_ptr->push(-1);

                    available_worker_notify_ptr->notify_all();
                    available_leaf_node_notify_ptr->notify_all();
                    return;
                }
                else
                {
                    *work_available_ptr = false;
                }
            }
        }
    }

    void Object_Manager::Create_Object_From_Volume_Function(std::string label,
                                                            uint32_t max_depth,
                                                            Vector_3<double> sample_region_center,
                                                            double sample_region_size,
                                                            std::function<double(Vector_3<double>)> volume_sample_function,
                                                            std::function<Vector_3<double>(Vector_3<double>, Vector_3<double>)> color_sample_function)
    {
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time = std::chrono::high_resolution_clock::now();

        LOG_INFO << "Graphics: Creating object with label '" << label << "'";

        static const uint32_t link_order_lookup[8][8]
            = {{0, 1, 4, 2, 5, 3, 6, 7}, {1, 5, 0, 3, 4, 7, 2, 6}, {2, 3, 6, 7, 0, 1, 4, 5}, {3, 7, 2, 6, 1, 5, 0, 4}, {4, 0, 5, 6, 1, 2, 7, 3}, {5, 4, 1, 7, 0, 6, 3, 2}, {6, 2, 7, 3, 4, 0, 5, 1}, {7, 6, 3, 2, 5, 4, 1, 0}};

        static const uint32_t inverse_link_order_lookup[8][8]
            = {{0, 1, 3, 5, 2, 4, 6, 7}, {2, 0, 6, 3, 4, 1, 7, 5}, {4, 5, 0, 1, 6, 7, 2, 3}, {6, 4, 2, 0, 7, 5, 3, 1}, {1, 4, 5, 7, 0, 2, 3, 6}, {4, 2, 7, 6, 1, 0, 5, 3}, {5, 7, 1, 3, 4, 6, 0, 2}, {7, 6, 3, 2, 5, 4, 1, 0}};

        for (uint32_t i = 0; i < m_objects.size(); i++)
        {
            if (m_objects[i].label == label)
            {
                LOG_ERROR << "Graphics: The label '" << label << "' is already in use";
                exit(EXIT_FAILURE);
            }
        }

        m_objects.emplace_back();
        m_objects.back() = {};
        m_objects.back().label = label;

        Voxel root_voxel = {};
        root_voxel.size = sample_region_size;
        root_voxel.position = sample_region_center;
        root_voxel.normal = Vector_3<double>(0.0, 1.0, 0.0);
        root_voxel.parent_index = -1;
        root_voxel.child_indices[0] = 0;
        root_voxel.child_indices[1] = 0;
        root_voxel.child_indices[2] = 0;
        root_voxel.child_indices[3] = 0;
        root_voxel.child_indices[4] = 0;
        root_voxel.child_indices[5] = 0;
        root_voxel.child_indices[6] = 0;
        root_voxel.child_indices[7] = 0;
        root_voxel.child_index = 0;
        root_voxel.hit_links[0] = -1;
        root_voxel.hit_links[1] = -1;
        root_voxel.hit_links[2] = -1;
        root_voxel.hit_links[3] = -1;
        root_voxel.hit_links[4] = -1;
        root_voxel.hit_links[5] = -1;
        root_voxel.hit_links[6] = -1;
        root_voxel.hit_links[7] = -1;
        root_voxel.miss_links[0] = -1;
        root_voxel.miss_links[1] = -1;
        root_voxel.miss_links[2] = -1;
        root_voxel.miss_links[3] = -1;
        root_voxel.miss_links[4] = -1;
        root_voxel.miss_links[5] = -1;
        root_voxel.miss_links[6] = -1;
        root_voxel.miss_links[7] = -1;
        root_voxel.depth = 0;
        root_voxel.is_leaf = false;

        m_objects.back().voxels.push_back(root_voxel);

        std::vector<double> step_size_lookup_table;
        for (uint32_t i = 0; i <= max_depth; i++)
        {
            step_size_lookup_table.push_back((1.0 / (std::pow(2, (max_depth - i)))) * (root_voxel.size / std::pow(2, i)) * 2.0);
        }


        static const uint32_t WORKER_THREAD_COUNT = 32;

        bool work_complete = false;
        std::mutex work_complete_mutex;

        std::mutex voxels_mutex;

        uint32_t active_workers_count = 0;
        std::queue<uint32_t> available_workers_queue;
        std::mutex available_workers_queue_mutex;
        std::condition_variable available_worker_notify;

        std::stack<uint32_t> leaf_nodes_stack;
        std::mutex leaf_nodes_stack_mutex;
        std::condition_variable available_leaf_node_notify;
        leaf_nodes_stack.push(0);

        std::array<std::thread, WORKER_THREAD_COUNT> worker_threads;
        std::array<bool, WORKER_THREAD_COUNT> worker_thread_work_available;
        std::array<uint32_t, WORKER_THREAD_COUNT> worker_thread_current_work;
        std::array<std::mutex, WORKER_THREAD_COUNT> worker_thread_mutexes;
        std::array<std::condition_variable, WORKER_THREAD_COUNT> worker_threads_notifies;

        for (uint32_t i = 0; i < WORKER_THREAD_COUNT; i++)
        {
            std::lock_guard<std::mutex> worker_thread_lock(worker_thread_mutexes[i]);

            worker_thread_work_available[i] = false;
            worker_thread_current_work[i] = 0;

            std::thread worker_thread(Object_From_Volume_Function_Worker_Thread, max_depth, step_size_lookup_table, i, volume_sample_function, color_sample_function, &m_objects.back().voxels, &voxels_mutex, &work_complete, &work_complete_mutex,
                                      &active_workers_count, &available_workers_queue, &available_workers_queue_mutex, &available_worker_notify, &leaf_nodes_stack, &leaf_nodes_stack_mutex, &available_leaf_node_notify,
                                      &worker_thread_work_available[i], &worker_thread_current_work[i], &worker_thread_mutexes[i], &worker_threads_notifies[i]);

            worker_threads[i] = std::move(worker_thread);
        }

        uint32_t selected_worker;
        uint32_t selected_leaf_node;
        while (true)
        {
            {
                // Wait for available worker

                std::unique_lock<std::mutex> available_workers_queue_lock(available_workers_queue_mutex);
                available_worker_notify.wait(available_workers_queue_lock, [&available_workers_queue] { return !available_workers_queue.empty(); });

                selected_worker = available_workers_queue.front();
                available_workers_queue.pop();
            }
            {
                // Wait for available work

                std::unique_lock<std::mutex> leaf_nodes_stack_lock(leaf_nodes_stack_mutex);
                available_leaf_node_notify.wait(leaf_nodes_stack_lock, [&leaf_nodes_stack] { return !leaf_nodes_stack.empty(); });

                {
                    work_complete_mutex.lock();

                    bool close_idle_threads = work_complete;

                    work_complete_mutex.unlock();

                    if (close_idle_threads)
                    {
                        for (uint32_t i = 0; i < WORKER_THREAD_COUNT; i++)
                        {
                            {
                                std::lock_guard<std::mutex> worker_thread_lock(worker_thread_mutexes[i]);

                                worker_thread_work_available[i] = true;
                                worker_threads_notifies[i].notify_all();
                            }

                            worker_threads[i].join();
                        }

                        break;
                    }
                }

                selected_leaf_node = leaf_nodes_stack.top();
                leaf_nodes_stack.pop();
            }
            {
                // Send work to worker

                std::lock_guard<std::mutex> worker_lock(worker_thread_mutexes[selected_worker]);
                std::lock_guard<std::mutex> available_workers_queue_lock(available_workers_queue_mutex);

                worker_thread_current_work[selected_worker] = selected_leaf_node;
                worker_thread_work_available[selected_worker] = true;
                active_workers_count++;

                worker_threads_notifies[selected_worker].notify_all();
            }
        }


        LOG_TRACE << "Graphics: It took " << (float)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time).count() / 1000.0 << " seconds to generate " << label;
    }

    std::vector<Object_Manager::GPU_Voxel> Object_Manager::Get_GPU_Voxels()
    {
        std::vector<GPU_Voxel> gpu_voxels;

        for (uint32_t i = 0; i < m_objects.size(); i++)
        {
            for (uint32_t j = 0; j < m_objects[i].voxels.size(); j++)
            {
                Voxel* current_voxel = &m_objects[i].voxels[j];

                GPU_Voxel gpu_voxel = {};
                gpu_voxel.position_x = static_cast<float>(current_voxel->position.m_x);
                gpu_voxel.position_y = static_cast<float>(current_voxel->position.m_y);
                gpu_voxel.position_z = static_cast<float>(current_voxel->position.m_z);
                gpu_voxel.size = static_cast<float>(current_voxel->size);
                gpu_voxel.hit_links[0] = current_voxel->hit_links[0];
                gpu_voxel.hit_links[1] = current_voxel->hit_links[1];
                gpu_voxel.hit_links[2] = current_voxel->hit_links[2];
                gpu_voxel.hit_links[3] = current_voxel->hit_links[3];
                gpu_voxel.hit_links[4] = current_voxel->hit_links[4];
                gpu_voxel.hit_links[5] = current_voxel->hit_links[5];
                gpu_voxel.hit_links[6] = current_voxel->hit_links[6];
                gpu_voxel.hit_links[7] = current_voxel->hit_links[7];
                gpu_voxel.miss_links[0] = current_voxel->miss_links[0];
                gpu_voxel.miss_links[1] = current_voxel->miss_links[1];
                gpu_voxel.miss_links[2] = current_voxel->miss_links[2];
                gpu_voxel.miss_links[3] = current_voxel->miss_links[3];
                gpu_voxel.miss_links[4] = current_voxel->miss_links[4];
                gpu_voxel.miss_links[5] = current_voxel->miss_links[5];
                gpu_voxel.miss_links[6] = current_voxel->miss_links[6];
                gpu_voxel.miss_links[7] = current_voxel->miss_links[7];
                gpu_voxel.normal_x = static_cast<float>(current_voxel->normal.m_x);
                gpu_voxel.normal_y = static_cast<float>(current_voxel->normal.m_y);
                gpu_voxel.normal_z = static_cast<float>(current_voxel->normal.m_z);
                gpu_voxel.color_r = static_cast<float>(current_voxel->color.m_x);
                gpu_voxel.color_g = static_cast<float>(current_voxel->color.m_y);
                gpu_voxel.color_b = static_cast<float>(current_voxel->color.m_z);

                gpu_voxels.push_back(gpu_voxel);
            }
        }

        return gpu_voxels;
    }
} // namespace Cascade_Graphics