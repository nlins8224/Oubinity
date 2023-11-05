#include "ZoneVertexPool.h"

namespace ZonePool {
    ZoneVertexPool::ZoneVertexPool() :
        m_mesh_persistent_buffer{ nullptr },
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

    void ZoneVertexPool::allocate(Chunk& chunk)
    {
        unsigned int added_faces = chunk.getAddedFacesAmount();
        if (added_faces == 0)
        {
            glm::ivec3 chunk_pos = chunk.getPos();
            LOG_F(INFO, "Empty chunk chunk at pos (%d, %d, %d), no faces added", chunk_pos.x, chunk_pos.y, chunk_pos.z);
            return;
        }
        unsigned int added_vertices = 6 * added_faces;
        size_t lod_level = chunk.getLevelOfDetail().level;
        Zone zone = chooseZone(lod_level);
        MeshBucket* first_free_bucket = getFirstFreeBucket(zone.level);
        m_stats.chunks_in_buckets[zone.level]++;


        if (first_free_bucket == nullptr)
        {
            glm::ivec3 chunk_pos = chunk.getPos();
            LOG_F(WARNING, "ALL BUCKETS AT ZONE: %d ARE FULL, NO SPACE LEFT. Tried to add %d vertices, chunk pos (%d, %d, %d)", zone.level, added_vertices, chunk_pos.x, chunk_pos.y, chunk_pos.z);
            return;
        }
        DAIC daic
        {
            added_vertices, // vertices in face * added_faces
            1,
            first_free_bucket->_start_offset, // offset in m_mesh_persistent_buffer
            0
        };

        LevelOfDetail::LevelOfDetail lod = chunk.getLevelOfDetail();
        glm::ivec3 chunk_pos = chunk.getPos();
        std::vector<Vertex> mesh{ chunk.getMesh().getMeshDataCopy() };
        size_t id = first_free_bucket->_id;
        LOG_F(INFO, "Allocating bucket at level: %d, with id: %d, vertices: %d, at chunk pos: (%d, %d, %d)", zone.level, id, added_vertices, chunk_pos.x, chunk_pos.y, chunk_pos.z);

        m_chunk_metadata.active_daics.push_back(daic);
        first_free_bucket->_is_free = false;

        size_t daic_id = m_chunk_metadata.active_daics.size() - 1;
        m_chunk_metadata.active_chunk_info.chunk_pos[daic_id] = { chunk.getWorldPos(), id };
        m_chunk_metadata.active_chunks_lod.chunks_lod[daic_id] = static_cast<GLuint>(lod.block_size);
        m_chunk_pos_to_bucket_id[chunk_pos] = {zone.level, id};
        m_bucket_id_to_daic_id[{zone.level, id}] = daic_id;

        updateMeshBuffer(mesh, first_free_bucket->_start_offset);

        m_stats.max_vertices_occurred[zone.level] = std::max(m_stats.max_vertices_occurred[zone.level], (size_t)added_vertices);
        m_stats.min_vertices_occurred[zone.level] = std::min(m_stats.min_vertices_occurred[zone.level], (size_t)added_vertices);

        //for (int i = 0; i < zones.size(); i++)
        //{
        //    LOG_F(INFO, "Zone %d, chunks to be in zone %d, max verts: %d, min verts: %d", i, m_stats.chunks_in_buckets[i], m_stats.max_vertices_occurred[i], m_stats.min_vertices_occurred[i]);
        //}
    }

    void ZoneVertexPool::free(glm::ivec3 chunk_pos)
    {
        if (m_chunk_pos_to_bucket_id.find(chunk_pos) == m_chunk_pos_to_bucket_id.end()) {
            LOG_F(WARNING, "Chunk at (%d, %d, %d) not found", chunk_pos.x, chunk_pos.y, chunk_pos.z);
            return;
        }
        fastErase(chunk_pos);
    }

    void ZoneVertexPool::fastErase(glm::ivec3 chunk_pos) {
        std::pair<size_t, size_t> bucket_id = m_chunk_pos_to_bucket_id[chunk_pos];
        size_t daic_id = m_bucket_id_to_daic_id[bucket_id];
        size_t last_daic_id = m_chunk_metadata.active_daics.size() - 1;
        std::pair<size_t, size_t> bucket_id_of_last_daic = getBucketIdFromDAIC(m_chunk_metadata.active_daics[last_daic_id]);

        LOG_F(INFO, "daic_id: %zu, zone_id: %zu, bucket_id: %zu, last_daic_id: %zu, last_zone_id: %zu, last_bucket_id: %zu, chunk_pos: (%d, %d, %d)",
            daic_id, bucket_id.first, bucket_id.second, last_daic_id, bucket_id_of_last_daic.first, bucket_id_of_last_daic.second, chunk_pos.x, chunk_pos.y, chunk_pos.z);

        if (m_chunk_metadata.active_daics.empty() || last_daic_id < daic_id) {
            LOG_F(ERROR, "An attempt to delete element %d was made, but DAIC size is %d", daic_id, last_daic_id);
            return;
        }

        m_chunk_metadata.active_chunk_info.chunk_pos[daic_id] = m_chunk_metadata.active_chunk_info.chunk_pos[last_daic_id];
        m_chunk_metadata.active_chunks_lod.chunks_lod[daic_id] = m_chunk_metadata.active_chunks_lod.chunks_lod[last_daic_id];
        std::swap(m_chunk_metadata.active_daics[daic_id], m_chunk_metadata.active_daics[last_daic_id]);

        m_bucket_id_to_daic_id[bucket_id_of_last_daic] = daic_id;
        //m_bucket_id_to_daic_id.erase(bucket_id);
        m_chunk_buckets[bucket_id.first][bucket_id.second]._is_free = true;
        m_chunk_pos_to_bucket_id.erase(chunk_pos);
        m_chunk_metadata.active_daics.pop_back();
    }

    MeshBucket* ZoneVertexPool::getFirstFreeBucket(int zone_id)
    {
        const size_t buckets_amount = zones[zone_id]->buckets_amount;
        LOG_F(INFO, "zone_id: %d, buckets_amount: %d", zone_id, buckets_amount);

        for (MeshBucket& bucket : m_chunk_buckets[zone_id])
        {
            if (bucket._is_free)
                return &bucket;
        }
        return nullptr;
    }

    Zone ZoneVertexPool::chooseZone(unsigned int lod_level)
    {
        LOG_F(INFO, "lod_level: %d", lod_level);
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
                LOG_F(INFO, "zone->start_offset: %d <= start_offset: %d < zone->end_offset: %d ", zone->start_offset, daic.first, zone->end_offset);
                LOG_F(INFO, "returning target zone with level: %d", zone->level);
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
        LOG_F(INFO, __FUNCTION__);
        for (ZonePool::Zone* zone : zones)
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

        LOG_F(INFO, "vector of vectors size: %d", m_chunk_buckets.size());
        for (auto& bucket_vector : m_chunk_buckets) {
            LOG_F(INFO, "bucket_vector size: %d", bucket_vector.size());
        }
    }

    void ZoneVertexPool::formatVBO()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glVertexAttribIFormat(0, 1, GL_UNSIGNED_INT, 0);
        glVertexAttribIFormat(1, 1, GL_UNSIGNED_INT, 0);

        glVertexBindingDivisor(0, 0);
        glVertexBindingDivisor(1, 0);

        glVertexAttribBinding(0, 0);
        glVertexAttribBinding(1, 1);

        glBindVertexBuffer(0, m_vbo, offsetof(Vertex, _xyzs), sizeof(Vertex));
        glBindVertexBuffer(1, m_vbo, offsetof(Vertex, _uvw), sizeof(Vertex));
    }

    void ZoneVertexPool::draw()
    {
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_daicbo);

        glMultiDrawArraysIndirect(
            GL_TRIANGLES,
            (GLvoid*)0,                                   // start with first draw command
            m_chunk_metadata.active_daics.size(),     // DAIC amount
            0                                             // tightly packed
        );
    }

    void ZoneVertexPool::createMeshBuffer() {
        LOG_F(INFO, "%d", glGetError());
        GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
        size_t buffer_size = calculateTotalVertexAmount() * sizeof(Vertex);
        LOG_F(INFO, "createMeshBuffer buffer_size: %zu, daic size: %d", buffer_size, m_chunk_metadata.active_daics.size());
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferStorage(GL_ARRAY_BUFFER, buffer_size, 0, flags);
        m_mesh_persistent_buffer = (Vertex*)glMapBufferRange(GL_ARRAY_BUFFER, 0, buffer_size, flags);

        if (m_mesh_persistent_buffer == nullptr) {
            LOG_F(INFO, "%d", glGetError());
        }

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_daicbo);
        glBufferData(GL_DRAW_INDIRECT_BUFFER, m_chunk_metadata.active_daics.size() * sizeof(DAIC), NULL, GL_DYNAMIC_DRAW);
    }

    void ZoneVertexPool::updateMeshBuffer(std::vector<Vertex>& mesh, int buffer_offset)
    {
        OPTICK_EVENT("updateMeshBuffer");
        waitBuffer();
        std::copy(mesh.begin(), mesh.end(), m_mesh_persistent_buffer + buffer_offset);
        lockBuffer();

        //int index = 0;
        //for (DAIC daic : m_chunk_metadata.active_daics) {
        //    LOG_F(INFO, "DAIC: %d, count: %d, first: %d", index, daic.count, daic.first);
        //    index++;
        //}

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

}


