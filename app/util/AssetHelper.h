#ifndef _COMMON_ASSETHELPER_H_
#define _COMMON_ASSETHELPER_H_

#include <string>
#include <vector>

struct AAssetManager;

namespace util {
// read asset file

class AssetHelper {
public:
    static AssetHelper *Get();
    void Init(struct AAssetManager *mgr);
    bool AssetReadFile(const std::string &name, std::vector<uint8_t> &buf);
    ~AssetHelper();

protected:
    AssetHelper();

private:
    struct AAssetManager *m_aassetMgr;

};

} // namespace common
#endif // _COMMON_ASSETHELPER_H_
