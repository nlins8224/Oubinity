#include "ZoneVertexPool.h"

namespace VertexPool {
    ZoneVertexPool::ZoneVertexPool() :
        m_mesh_persistent_buffer{ nullptr },
        m_persistent_buffer_vertices_amount{0},
        m_chunk_buckets{zones.size(), std::vector<MeshBucket>(0)}
    {
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        formatVBO();
        glGenBuffers(1, &m_daicbo);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_daicbo);

        calculateBucketAmountInZones();
        createMeshBuffer();
        initZones(m_mesh_persistent_buffer);
        initBuckets();

        for (int i = 0; i < zones.size(); i++)
        {
            m_stats.min_vertices_occurred[i] = MAX_VERTICES_IN_LARGEST_BUCKET;
            m_stats.max_vertices_occurred[i] = 0;
            m_stats.chunks_in_buckets[i] = 0;
        }
    }

   void ZoneVertexPool::allocate(ChunkAllocData&& alloc_data)
    {
        unsigned int added_faces = alloc_data._added_faces_amount;
        glm::ivec3 chunk_pos = alloc_data._chunk_pos;

        if (added_faces == 0)
        {
            LOG_F(INFO, "Empty chunk at pos (%d, %d, %d), no faces added", chunk_pos.x, chunk_pos.y, chunk_pos.z);
            return;
        }
        if (alloc_data._mesh.size() == 0)
        {
            LOG_F(ERROR, "chunk at pos (%d, %d, %d) has a mesh size equal to 0, with %zu faces added", chunk_pos.x, chunk_pos.y, chunk_pos.z, added_faces);
            return;
        }
        unsigned int added_vertices = 6 * added_faces;
        size_t lod_level = alloc_data._lod.level;
        Zone zone = chooseZone(lod_level);
        MeshBucket* first_free_bucket = getFirstFreeBucket(zone.level);
        m_stats.chunks_in_buckets[zone.level]++;


        if (first_free_bucket == nullptr)
        {
            LOG_F(ERROR, "ALL BUCKETS AT ZONE: %d ARE FULL, NO SPACE LEFT. Tried to add %d vertices, chunk pos (%d, %d, %d)", zone.level, added_vertices, chunk_pos.x, chunk_pos.y, chunk_pos.z);
            return;
        }

        if ((first_free_bucket->_start_offset % 6) != 0)
        {
            LOG_F(ERROR, "Offset % 6 != 0, start offset: %d", first_free_bucket->_start_offset);
            return;
        }

        DAIC daic
        {
            added_vertices, // vertices in face * added_faces
            1,
            first_free_bucket->_start_offset, // offset in m_mesh_persistent_buffer
            0
        };

        LevelOfDetail::LevelOfDetail lod = alloc_data._lod;
        size_t id = first_free_bucket->_id;
        LOG_F(5, "Allocating bucket at level: %d, with id: %d, vertices: %d, at chunk pos: (%d, %d, %d)", zone.level, id, added_vertices, chunk_pos.x, chunk_pos.y, chunk_pos.z);

        m_chunk_metadata.active_daics.push_back(daic);
        first_free_bucket->_is_free = false;

        size_t daic_id = m_chunk_metadata.active_daics.size() - 1;
        m_chunk_metadata.active_chunk_info.chunk_pos[daic_id] = { alloc_data._chunk_world_pos, id };
        m_chunk_metadata.active_chunks_lod.chunks_lod[daic_id] = static_cast<GLuint>(lod.block_size);
        m_chunk_pos_to_bucket_id[chunk_pos] = { zone.level, id };
        m_bucket_id_to_daic_id[{zone.level, id}] = daic_id;

        updateMeshBuffer(alloc_data._mesh, first_free_bucket->_start_offset);

        m_stats.max_vertices_occurred[zone.level] = std::max(m_stats.max_vertices_occurred[zone.level], (size_t)added_vertices);
        m_stats.min_vertices_occurred[zone.level] = std::min(m_stats.min_vertices_occurred[zone.level], (size_t)added_vertices);
    }

    void ZoneVertexPool::free(glm::ivec3 chunk_pos)
    {
        if (m_chunk_pos_to_bucket_id.find(chunk_pos) == m_chunk_pos_to_bucket_id.end()) {
            LOG_F(3, "Chunk at (%d, %d, %d) not found", chunk_pos.x, chunk_pos.y, chunk_pos.z);
            return;
        }
        fastErase(chunk_pos);
    }

    void ZoneVertexPool::fastErase(glm::ivec3 chunk_pos) {
        std::pair<size_t, size_t> bucket_id = m_chunk_pos_to_bucket_id[chunk_pos];
        size_t daic_id = m_bucket_id_to_daic_id[bucket_id];
        size_t last_daic_id = m_chunk_metadata.active_daics.size() - 1;
        std::pair<size_t, size_t> bucket_id_of_last_daic = getBucketIdFromDAIC(m_chunk_metadata.active_daics[last_daic_id]);

        LOG_F(5, "daic_id: %zu, zone_id: %zu, bucket_id: %zu, last_daic_id: %zu, last_zone_id: %zu, last_bucket_id: %zu, chunk_pos: (%d, %d, %d)",
            daic_id, bucket_id.first, bucket_id.second, last_daic_id, bucket_id_of_last_daic.first, bucket_id_of_last_daic.second, chunk_pos.x, chunk_pos.y, chunk_pos.z);

        if (m_chunk_metadata.active_daics.empty() || last_daic_id < daic_id) {
            LOG_F(ERROR, "An attempt to delete element %zu was made, but DAIC size is %zu", daic_id, last_daic_id);
            return;
        }

        m_chunk_metadata.active_chunk_info.chunk_pos[daic_id] = m_chunk_metadata.active_chunk_info.chunk_pos[last_daic_id];
        m_chunk_metadata.active_chunks_lod.chunks_lod[daic_id] = m_chunk_metadata.active_chunks_lod.chunks_lod[last_daic_id];
        std::swap(m_chunk_metadata.active_daics[daic_id], m_chunk_metadata.active_daics[last_daic_id]);

        m_bucket_id_to_daic_id[bucket_id_of_last_daic] = daic_id;
        m_chunk_buckets[bucket_id.first][bucket_id.second]._is_free = true;
        m_chunk_pos_to_bucket_id.erase(chunk_pos);
        m_chunk_metadata.active_daics.pop_back();
    }

    //TODO: list of free buckets
    MeshBucket* ZoneVertexPool::getFirstFreeBucket(int zone_id)
    {
        const size_t buckets_amount = zones[zone_id]->buckets_amount;
        for (MeshBucket& bucket : m_chunk_buckets[zone_id])
        {
            if (bucket._is_free)
                return &bucket;
        }
        return nullptr;
    }

    Zone ZoneVertexPool::chooseZone(unsigned int lod_level)
    {
        return *zones[lod_level];
    }

    std::pair<size_t, size_t> ZoneVertexPool::getBucketIdFromDAIC(DAIC daic)
    {
        Zone zone = calculateZoneFromDaicStartOffset(daic);
        size_t id = (daic.first - zone.start_offset) / zone.max_vertices_per_bucket;
        return { zone.level, id };
    }

    Zone ZoneVertexPool::calculateZoneFromDaicStartOffset(DAIC daic)
    {
        for (auto& zone : zones)
        {
            if (zone->start_offset <= daic.first && daic.first + daic.count < zone->end_offset)
            {
                return *zone;
            }
        }
    }

    void ZoneVertexPool::waitBuffer()
    {
        if (m_sync)
        {
            while (1)
            {
                GLenum waitReturn = glClientWaitSync(m_sync, GL_SYNC_FLUSH_COMMANDS_BIT, 1);
                if (waitReturn == GL_ALREADY_SIGNALED || waitReturn == GL_CONDITION_SATISFIED) {
                    return;
                }
            }
        }
    }

    void ZoneVertexPool::lockBuffer()
    {
        if (m_sync) {
            glDeleteSync(m_sync);
        }

        m_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }

    ZoneVertexPool::~ZoneVertexPool()
    {
        delete m_mesh_persistent_buffer;
    }

    void ZoneVertexPool::initBuckets()
    {
        for (VertexPool::Zone* zone : zones)
        {
            for (size_t bucket_idx = 0; bucket_idx < zone->buckets_amount; bucket_idx++)
            {
                MeshBucket bucket;
                bucket._id = bucket_idx;
                bucket._is_free = true;
                bucket._start_offset = zone->start_offset + (bucket_idx * zone->max_vertices_per_bucket);
                m_chunk_buckets[zone->level].push_back(bucket);

            }
        }
    }

    void ZoneVertexPool::initZones(Vertex* buffer)
    {
        // TODO: loop
        size_t vertices_in_zero_zone = Zero.max_vertices_per_bucket * Zero.buckets_amount;
        Zero.start_offset = 0;
        Zero.end_offset = vertices_in_zero_zone;

        size_t vertices_in_first_zone = One.max_vertices_per_bucket * One.buckets_amount;
        One.start_offset = Zero.end_offset;
        One.end_offset = One.start_offset + vertices_in_first_zone;

        size_t vertices_in_second_zone = Two.max_vertices_per_bucket * Two.buckets_amount;
        Two.start_offset = One.end_offset;
        Two.end_offset = Two.start_offset + vertices_in_second_zone;

        size_t vertices_in_third_zone = Three.max_vertices_per_bucket * Three.buckets_amount;
        Three.start_offset = Two.end_offset;
        Three.end_offset = Three.start_offset + vertices_in_third_zone;

        size_t vertices_in_fourth_zone = Four.max_vertices_per_bucket * Four.buckets_amount;
        Four.start_offset = Three.end_offset;
        Four.end_offset = Four.start_offset + vertices_in_fourth_zone;

        Five.start_offset = Four.end_offset;
        Five.end_offset = m_persistent_buffer_vertices_amount;

        LOG_F(INFO, "Zero start offset: %zu, end offset: %zu", Zero.start_offset, Zero.end_offset);
        LOG_F(INFO, "One start offset: %zu, end offset: %zu", One.start_offset, One.end_offset);
        LOG_F(INFO, "Two start offset: %zu, end offset: %zu", Two.start_offset, Two.end_offset);
        LOG_F(INFO, "Three start offset: %zu, end offset: %zu", Three.start_offset, Three.end_offset);
        LOG_F(INFO, "Four start offset: %zu, end offset: %zu", Four.start_offset, Four.end_offset);
        LOG_F(INFO, "Five start offset: %zu, end offset: %zu", Five.start_offset, Five.end_offset);
    }

    size_t ZoneVertexPool::calculateBucketAmountInZones()
    {
        // TODO: loop
        size_t buckets_added = 0;
        Zero.buckets_amount = std::pow(LevelOfDetail::One.draw_distance, 2) * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS;
        buckets_added += Zero.buckets_amount;
        One.buckets_amount = std::pow(LevelOfDetail::Two.draw_distance, 2) * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - buckets_added;
        buckets_added += One.buckets_amount;
        Two.buckets_amount = std::pow(LevelOfDetail::Three.draw_distance, 2) * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - buckets_added;
        buckets_added += Two.buckets_amount;
        Three.buckets_amount = std::pow(LevelOfDetail::Four.draw_distance, 2) * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - buckets_added;
        buckets_added += Three.buckets_amount;
        Four.buckets_amount = std::pow(LevelOfDetail::Five.draw_distance, 2) * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - buckets_added;
        buckets_added += Four.buckets_amount;
        Five.buckets_amount = TOTAL_BUCKETS_AMOUNT - buckets_added;
        buckets_added += Five.buckets_amount;

        // For a short while Zone buffer might overflow, add extra buckets to prevent that
        // Example: Zone is full, there are pending tasks in the queue
        // and "allocate" tasks are preceeding "free" tasks
        for (Zone* zone : zones)
        {
            zone->buckets_amount += EXTRA_BUFFER_SPACE;
            buckets_added += EXTRA_BUFFER_SPACE;
        }
        LOG_F(INFO, "Total buckets amount: %zu", buckets_added);
        return buckets_added;
    }


    size_t ZoneVertexPool::calculateTotalVertexAmount()
    {
        size_t vertex_amount = 0;

        for (Zone* zone : zones)
        {
            vertex_amount += zone->max_vertices_per_bucket * zone->buckets_amount;
            LOG_F(INFO, "zone %d vertexamount: %d, max_vertices_per_bucket: %d, buckets_amount: %d", zone->level, zone->max_vertices_per_bucket * zone->buckets_amount, zone->max_vertices_per_bucket, zone->buckets_amount);

        }
        return vertex_amount;
    }

    void ZoneVertexPool::formatVBO()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribIFormat(0, 1, GL_UNSIGNED_INT, 0);
        glVertexBindingDivisor(0, 0);
        glVertexAttribBinding(0, 0);
        glBindVertexBuffer(0, m_vbo, offsetof(Vertex, packed_vertex), sizeof(Vertex));
    }

    void ZoneVertexPool::draw()
    {
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_daicbo);

        glMultiDrawArraysIndirect(
            GL_TRIANGLES,
            (GLvoid*)0,                                   // start with first draw command
            m_chunk_metadata.active_daics.size(),         // DAIC amount
            0                                             // tightly packed
        );
    }

    void ZoneVertexPool::createMeshBuffer() {
        GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
        m_persistent_buffer_vertices_amount = calculateTotalVertexAmount();
        size_t buffer_size = m_persistent_buffer_vertices_amount * sizeof(Vertex);
        LOG_F(INFO, "createMeshBuffer buffer_size: %zu, daic size: %zu, OpenGL Error: %zu", buffer_size, m_chunk_metadata.active_daics.size(), glGetError());
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferStorage(GL_ARRAY_BUFFER, buffer_size, 0, flags);
        m_mesh_persistent_buffer = (Vertex*)glMapBufferRange(GL_ARRAY_BUFFER, 0, buffer_size, flags);

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_daicbo);
        glBufferData(GL_DRAW_INDIRECT_BUFFER, m_chunk_metadata.active_daics.size() * sizeof(DAIC), NULL, GL_DYNAMIC_DRAW);
    }

    void ZoneVertexPool::updateMeshBuffer(std::vector<Vertex>& mesh, int buffer_offset)
    {
        OPTICK_EVENT("updateMeshBuffer");
        waitBuffer();
        std::copy(mesh.begin(), mesh.end(), m_mesh_persistent_buffer + buffer_offset);
        lockBuffer();

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_daicbo);
        glBufferData(GL_DRAW_INDIRECT_BUFFER, m_chunk_metadata.active_daics.size() * sizeof(DAIC), m_chunk_metadata.active_daics.data(), GL_DYNAMIC_DRAW);
    }

    void ZoneVertexPool::createChunkInfoBuffer()
    {
        OPTICK_EVENT("createChunkInfoBuffer");
        m_chunk_info_ssbo = 0;
        glGenBuffers(1, &m_chunk_info_ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_chunk_info_ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(m_chunk_metadata.active_chunk_info), &m_chunk_metadata.active_chunk_info, GL_STATIC_COPY);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_chunk_info_ssbo);
    }

    void ZoneVertexPool::createChunkLodBuffer()
    {
        OPTICK_EVENT("createChunkLodBuffer");
        m_chunks_lod_ssbo = 1;
        glGenBuffers(1, &m_chunks_lod_ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_chunks_lod_ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(m_chunk_metadata.active_chunks_lod), &m_chunk_metadata.active_chunks_lod, GL_STATIC_COPY);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_chunks_lod_ssbo);
    }

    void ZoneVertexPool::createChunkBlockInfoBuffer()
    {

    }

}


