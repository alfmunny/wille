#include "wille/config.h"
#include "wille/log.h"
#include <string>
#include <yaml-cpp/yaml.h>

wille::ConfigVar<int>::ptr g_int_value_config =
    wille::Config::Lookup("system.port", (int)8080, "system port");

wille::ConfigVar<float>::ptr g_int_valuex_config =
    wille::Config::Lookup("system.port", (float)8080, "system port");

wille::ConfigVar<float>::ptr g_float_value_config =
    wille::Config::Lookup("system.value", (float)10.2f, "system value");

wille::ConfigVar<std::vector<int>>::ptr g_int_vec_value_config =
    wille::Config::Lookup("system.int_vec", std::vector<int>{1, 2},
                          "system int vec");

wille::ConfigVar<std::list<int>>::ptr g_int_list_value_config =
    wille::Config::Lookup("system.int_list", std::list<int>{1, 2},
                          "system int list");

wille::ConfigVar<std::set<int>>::ptr g_int_set_value_config =
    wille::Config::Lookup("system.int_set", std::set<int>{1, 2},
                          "system int set");

wille::ConfigVar<std::unordered_set<int>>::ptr g_int_uset_value_config =
    wille::Config::Lookup("system.int_uset", std::unordered_set<int>{1, 2},
                          "system int uset");

wille::ConfigVar<std::map<std::string, int>>::ptr g_int_map_value_config =
    wille::Config::Lookup("system.int_map",
                          std::map<std::string, int>{{"k", 2}},
                          "system int map");

wille::ConfigVar<std::unordered_map<std::string, int>>::ptr
    g_int_umap_value_config =
        wille::Config::Lookup("system.int_umap",
                              std::unordered_map<std::string, int>{{"k", 2}},
                              "system int umap");

void test_config() {
    YAML::Node root =
        YAML::LoadFile("/Users/alfmunny/Projects/wille/bin/conf/test.yml");
    WILLE_LOG_INFO(WILLE_LOG_ROOT()) << root;

    WILLE_LOG_INFO(WILLE_LOG_ROOT())
        << "before: " << g_int_value_config->getValue();
    WILLE_LOG_INFO(WILLE_LOG_ROOT())
        << "before: " << g_float_value_config->toString();

#define XX(g_var, name, prefix)                                                \
    for (auto& i : g_var->getValue()) {                                        \
        WILLE_LOG_INFO(WILLE_LOG_ROOT()) << #prefix " " #name ": " << i;       \
    }                                                                          \
    WILLE_LOG_INFO(WILLE_LOG_ROOT())                                           \
        << #prefix " " #name " yaml: " << g_var->toString();

#define XX_M(g_var, name, prefix)                                              \
    for (auto& i : g_var->getValue()) {                                        \
        WILLE_LOG_INFO(WILLE_LOG_ROOT())                                       \
            << #prefix " " #name ": {" << i.first << " - " << i.second << "}"; \
    }                                                                          \
    WILLE_LOG_INFO(WILLE_LOG_ROOT())                                           \
        << #prefix " " #name " yaml: " << g_var->toString();

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

class Person {
public:
    std::string m_name;
    int m_age = 0;
    bool m_sex = 0;

    std::string toString() const {
        std::stringstream ss;
        ss << "[Person name = " << m_name;
        ss << " age=" << m_age;
        ss << " sex=" << m_sex;
        ss << "]";
        return ss.str();
    }

    bool operator==(const Person& oth) const {
        return m_name == oth.m_name && m_age == oth.m_age && m_sex == oth.m_sex;
    }
};

namespace wille {
template <> class LexicalCast<std::string, Person> {
public:
    Person operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        Person p;
        p.m_name = node["name"].as<std::string>();
        p.m_age = node["age"].as<int>();
        p.m_sex = node["sex"].as<bool>();
        return p;
    }
};

template <> class LexicalCast<Person, std::string> {
public:
    std::string operator()(const Person& p) {
        YAML::Node node;
        std::stringstream ss;
        node["name"] = p.m_name;
        node["age"] = p.m_age;
        node["sex"] = p.m_sex;
        ss << node;
        return ss.str();
    }
};
}


wille::ConfigVar<Person>::ptr g_person =
    wille::Config::Lookup("class.person", Person(), "person");

wille::ConfigVar<std::map<std::string, Person>>::ptr g_person_map =
    wille::Config::Lookup("class.map", std::map<std::string, Person>(),
                          "person map");

wille::ConfigVar<std::map<std::string, std::vector<Person>>>::ptr
    g_person_vec_map =
        wille::Config::Lookup("class.vec_map",
                              std::map<std::string, std::vector<Person>>(),
                              "person map");

void test_class() {
    WILLE_LOG_INFO(WILLE_LOG_ROOT())
        << "before: " << g_person->getValue().toString() << " - "
        << g_person->toString();

    auto m = g_person_map->getValue();

    for (auto& i : m) {
        WILLE_LOG_INFO(WILLE_LOG_ROOT())
            << "before: " << i.first << " - " << i.second.toString();
    }

    g_person->addListener([](const Person& old_value, const Person& new_value) {
                              WILLE_LOG_INFO(WILLE_LOG_ROOT())
                                  << "old_value = " << old_value.toString()
                                  << " new_value = " << new_value.toString();
                          });

    YAML::Node root =
        YAML::LoadFile("/Users/alfmunny/Projects/wille/config/test.yml");
    wille::Config::LoadFromYaml(root);
    WILLE_LOG_INFO(WILLE_LOG_ROOT())
        << "after: " << g_person->getValue().toString() << " - "
        << g_person->toString();

    m = g_person_map->getValue();
    for (auto& i : m) {
        WILLE_LOG_INFO(WILLE_LOG_ROOT())
            << "after: " << i.first << " - " << i.second.toString();
    }

    WILLE_LOG_INFO(WILLE_LOG_ROOT())
        << "after: " << g_person_vec_map->toString();
}

void test_log() {
    static wille::Logger::ptr system_logger = WILLE_LOG_NAME("system");
    std::cout << wille::LoggerMgr::GetInstance()->toYamlString() << std::endl;
    WILLE_LOG_INFO(system_logger) << "hello world";
    YAML::Node root =
        YAML::LoadFile("/Users/alfmunny/Projects/wille/config/log.yml");
    wille::Config::LoadFromYaml(root);
    std::cout << "===============" << std::endl;
    std::cout << wille::LoggerMgr::GetInstance()->toYamlString() << std::endl;
    system_logger->setFormatter("%d - %m%n");
    WILLE_LOG_INFO(system_logger) << "hello world";
    WILLE_LOG_INFO(WILLE_LOG_ROOT()) << "Hello world from root";
}

int main(int argc, char** argv) {
    // test_config();
    // test_class();
    test_log();

    wille::Config::Visit([](wille::ConfigVarBase::ptr var) {
            WILLE_LOG_INFO(WILLE_LOG_ROOT()) << "name=" << var->getName() 
            << " description=" << var->getDescription() 
            << " typename=" << var->getTypeName()
            << " value=" << var->toString(); 

    });
    return 0;
}
