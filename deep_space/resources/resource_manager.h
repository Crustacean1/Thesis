#ifndef RESOURCE_MANAGER
#define RESOURCE_MANAGER

#include "../components/framebuffer.h"
#include "../vertices.h"

#include <functional>
#include <map>
#include <memory>
#include <string>

class Shader;

std::pair<std::vector<std::string>, std::function<std::vector<unsigned char>(const std::string &)>>
create_loader(const std::string &filepath);

template <typename T> class PartialResourceManager {
    std::map<std::string, T> resources;

  public:
    T &get(const std::string &res_id) {
        if (auto res = resources.find(res_id); res != resources.end()) {
            return res->second;
        } else {
            auto [files, loader] = create_loader(res_id);
            resources.emplace(
                res_id, std::move(files.empty() ? T::default_resource() : T::load(files, loader)));
            return resources.find(res_id)->second;
        }
    }

    T &add(const std::string &res_id, T &&resource) {
        return resources.emplace(res_id, std::forward<T>(resource)).first->second;
    }
};

std::tuple<const char *, const char *, const char *> read_shader(const char *);

template <typename... Resources>
class GenericResourceManager : public PartialResourceManager<Resources>... {
  public:
    template <typename T> T &get(const std::string &res_id) {
        return PartialResourceManager<T>::get(res_id);
    }

    using PartialResourceManager<Resources>::add...;
};

#endif /*RESOURCE_MANAGER*/
