#include "wille/config.h"
#include "wille/log.h"
#include <yaml-cpp/yaml.h>

wille::ConfigVar<int>::ptr g_int_value_config = 
    wille::Config::Lookup("system.port", (int)8080, "system port");

wille::ConfigVar<float>::ptr g_float_value_config = 
    wille::Config::Lookup("system.value", (float)10.2f, "system value");

wille::ConfigVar<std::vector<int> >::ptr g_int_vec_value_config = 
    wille::Config::Lookup("system.int_vec", std::vector<int>{1,2}, "system int vec");

wille::ConfigVar<std::list<int> >::ptr g_int_list_value_config = 
    wille::Config::Lookup("system.int_list", std::list<int>{1,2}, "system int list");

wille::ConfigVar<std::set<int> >::ptr g_int_set_value_config = 
    wille::Config::Lookup("system.int_set", std::set<int>{1,2}, "system int set");

wille::ConfigVar<std::unordered_set<int> >::ptr g_int_uset_value_config = 
    wille::Config::Lookup("system.int_uset", std::unordered_set<int>{1,2}, "system int uset");

wille::ConfigVar<std::map<std::string, int> >::ptr g_int_map_value_config = 
    wille::Config::Lookup("system.int_map", std::map<std::string, int>{{"k", 2}}, "system int map");

wille::ConfigVar<std::unordered_map<std::string, int> >::ptr g_int_umap_value_config = 
    wille::Config::Lookup("system.int_umap", std::unordered_map<std::string, int>{{"k", 2}}, "system int umap");

void test_yaml() {
    YAML::Node root = YAML::LoadFile("/Users/alfmunny/Projects/wille/bin/conf/log.yml");
    WILLE_LOG_INFO(WILLE_LOG_ROOT()) << root;

    WILLE_LOG_INFO(WILLE_LOG_ROOT()) << "before: " << g_int_value_config->getValue();
    WILLE_LOG_INFO(WILLE_LOG_ROOT()) << "before: " << g_float_value_config->toString();

#define XX(g_var, name, prefix) \
    for(auto& i : g_var->getValue()) { \
       WILLE_LOG_INFO(WILLE_LOG_ROOT()) << #prefix " " #name ": " << i;  \
    } \
    WILLE_LOG_INFO(WILLE_LOG_ROOT()) << #prefix " " #name " yaml: " << g_var->toString();  \

#define XX_M(g_var, name, prefix) \
    for(auto& i : g_var->getValue()) { \
       WILLE_LOG_INFO(WILLE_LOG_ROOT()) << #prefix " " #name ": {" << i.first << " - " << i.second << "}"; \
    } \
    WILLE_LOG_INFO(WILLE_LOG_ROOT()) << #prefix " " #name " yaml: " << g_var->toString();  \

    XX(g_int_vec_value_config, int_vec, before);
    XX(g_int_list_value_config, int_list, before);
    XX(g_int_set_value_config, int_set, before);
    XX(g_int_uset_value_config, int_uset, before);
    XX_M(g_int_map_value_config, int_map, before)
    XX_M(g_int_umap_value_config, int_umap, before)

    wille::Config::LoadFromYaml(root);

    XX(g_int_vec_value_config, int_vec, after);
    XX(g_int_list_value_config, int_list, after);
    XX(g_int_set_value_config, int_set, after);
    XX(g_int_uset_value_config, int_uvec, after);
    XX_M(g_int_map_value_config, int_map, after)
    XX_M(g_int_umap_value_config, int_umap, after)

}

int main(int argc, char** argv) {
    test_yaml();
    return 0;
}
