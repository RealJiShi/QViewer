#include "AssetHelper.h"
#ifdef __ANDROID__
#include <android/asset_manager.h>
#endif
#include "LogUtil.h"

namespace util {

AssetHelper *AssetHelper::Get() {
    static AssetHelper helper;
    return &helper;
}

void AssetHelper::Init(AAssetManager *mgr) {
    m_aassetMgr = mgr;
}

bool AssetHelper::AssetReadFile(const std::string &name, std::vector<uint8_t> &buf)
{
    if (name.empty() || !m_aassetMgr) {
        return false;
    }
#ifdef __ANDROID__
    AAsset *asset_dsc = AAssetManager_open(m_aassetMgr, name.c_str(), AASSET_MODE_BUFFER);
    if (!asset_dsc) {
        return false;
    }
    std::size_t file_len = AAsset_getLength(asset_dsc);

    buf.resize(file_len);
    int64_t read_size = AAsset_read(asset_dsc, buf.data(), buf.size());

    AAsset_close(asset_dsc);
    return (read_size == buf.size());
#endif
}

AssetHelper::AssetHelper() :
    m_aassetMgr(nullptr) {
}

AssetHelper::~AssetHelper() {}

}
