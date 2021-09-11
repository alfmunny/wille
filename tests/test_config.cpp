#include "wille/config.h"
#include "wille/log.h"
#include <yaml-cpp/yaml.h>

wille::ConfigVar<int>::ptr g_int_value_config = 
    wille::Config::Lookup("system.port", (int)8080, "system port");

wille::ConfigVar<float>::ptr g_float_value_config = 
    wille::Config::Lookup("system.value", (float)10.2f, "system value");

void test_yaml() {
    YAML::Node root = YAML::LoadFile("/Users/alfmunny/Projects/wille/bin/conf/log.yml");
    WILLE_LOG_INFO(WILLE_LOG_ROOT()) << root;

    WILLE_LOG_INFO(WILLE_LOG_ROOT()) << "before: " << g_int_value_config->getValue();
    WILLE_LOG_INFO(WILLE_LOG_ROOT()) << "before: " << g_float_value_config->toString();

    wille::Config::LoadFromYaml(root);

    WILLE_LOG_INFO(WILLE_LOG_ROOT()) << "after: " << g_int_value_config->getValue();
    WILLE_LOG_INFO(WILLE_LOG_ROOT()) << "after: " << g_float_value_config->toString();
}

int main(int argc, char** argv) {
    test_yaml();
    return 0;
}
