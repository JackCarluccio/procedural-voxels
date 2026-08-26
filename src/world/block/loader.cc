#include "world/block/loader.h"

#include "world/block/block.h"
#include "world/block/data.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>

#include <json.h>
#include <stb_image.h>

namespace voxels::world::block  {

    namespace {

        constexpr int BLOCK_TEXTURE_SIZE = 16;
        constexpr size_t BLOCK_IMAGE_BYTES = BLOCK_TEXTURE_SIZE * BLOCK_TEXTURE_SIZE * 4;

        struct BlockFileData {
            std::string side_texture;
            std::string top_texture;
            std::string bottom_texture;
        };
    
        BlockFileData ReadBlockFile(const std::filesystem::path& file_path) {
            std::ifstream file(file_path);
            if (!file.is_open()) {
                throw std::runtime_error("Failed to open block data file: " + file_path.string());
            }
    
            BlockFileData data;
            auto document = nlohmann::json::parse(file);
    
            try {
                data.side_texture = document.at("side_texture");
                data.top_texture = document.at("top_texture");
                data.bottom_texture = document.at("bottom_texture");
            } catch (const nlohmann::json::exception& e) {
                throw std::runtime_error("Failed to parse block data file: " + file_path.string() + " - " + e.what());
            }
    
            return data;
        }
    
        void ReadAllBlockFiles(BlockFileData* file_data_array, std::unordered_map<std::string, uint8_t>& texture_to_id) {
            const std::filesystem::path blocks_directory("data/blocks");
    
            int next_texture_id = 0;
            for (int i = 1; i < COUNT; i++) {
                std::string block_name = ToString(static_cast<Block>(i));
                std::filesystem::path block_file_path = blocks_directory / (block_name + ".json");
    
                BlockFileData data = ReadBlockFile(block_file_path);
                file_data_array[i] = data;
    
                if (texture_to_id.find(data.side_texture) == texture_to_id.end()) {
                    texture_to_id[data.side_texture] = next_texture_id++;
                }
                if (texture_to_id.find(data.top_texture) == texture_to_id.end()) {
                    texture_to_id[data.top_texture] = next_texture_id++;
                }
                if (texture_to_id.find(data.bottom_texture) == texture_to_id.end()) {
                    texture_to_id[data.bottom_texture] = next_texture_id++;
                }
            }
        }
    
        void ReadBlockTextures(unsigned char* block_texture_data, const std::unordered_map<std::string, uint8_t>& texture_to_id) {
            for (const auto& [texture_name, texture_id] : texture_to_id) {
                std::filesystem::path texture_file_path = "assets/textures/" + texture_name + ".png";
    
                int width, height, channels;
                unsigned char* image_data = stbi_load(texture_file_path.string().c_str(), &width, &height, &channels, 4);
    
                if (!image_data) {
                    throw std::runtime_error("Failed to load texture: " + texture_file_path.string());
                }
    
                if (width != BLOCK_TEXTURE_SIZE || height != BLOCK_TEXTURE_SIZE) {
                    throw std::runtime_error(
                        "Texture size mismatch for " + texture_file_path.string() +
                        ". Expected: " + std::to_string(BLOCK_TEXTURE_SIZE) + "x" + std::to_string(BLOCK_TEXTURE_SIZE) +
                        ", but got: " + std::to_string(width) + "x" + std::to_string(height)
                    );
                }
    
                std::memcpy(block_texture_data + texture_id * BLOCK_IMAGE_BYTES, image_data, BLOCK_IMAGE_BYTES);
                stbi_image_free(image_data);
            }
        }
    
        void PopulateBlockData(const BlockFileData* file_data_array, const std::unordered_map<std::string, uint8_t>& texture_to_id) {
            for (int i = 1; i < COUNT; i++) {
                const BlockFileData& data = file_data_array[i];
    
                block::data[i] = {
                    .side_texture_index = texture_to_id.at(data.side_texture),
                    .top_texture_index = texture_to_id.at(data.top_texture),
                    .bottom_texture_index = texture_to_id.at(data.bottom_texture),
                };
            }
        }
    }

    unsigned char* LoadBlocks() {
        BlockFileData file_data_array[block::COUNT];
        std::unordered_map<std::string, uint8_t> texture_to_id;
        unsigned char* block_texture_data = new unsigned char[256 * BLOCK_IMAGE_BYTES];
        std::memset(block_texture_data, 0, 256 * BLOCK_IMAGE_BYTES);

        ReadAllBlockFiles(file_data_array, texture_to_id);
        ReadBlockTextures(block_texture_data, texture_to_id);
        PopulateBlockData(file_data_array, texture_to_id);

        return block_texture_data;
    }

}
