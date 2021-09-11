#ifndef __WILLE_CONFIG_H__
#define __WILLE_CONFIG_H__

#include <memory>
#include <string>
#include <sstream>
#include "log.h"
#include <yaml-cpp/yaml.h>
#include <boost/lexical_cast.hpp>

namespace wille {

class ConfigVarBase {
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    ConfigVarBase(const std::string& name, const std::string& description = "")
        :m_name(name)
        ,m_description(description) {
    }
    virtual ~ConfigVarBase() = default;
    
    const std::string& getName() const { return m_name; }
    const std::string& getDescription() const { return m_description; }

    virtual std::string toString() = 0;
    virtual bool fromString(const std::string& val) = 0;
protected:
    std::string m_name;
    std::string m_description;
};

template<class T>
class ConfigVar : public ConfigVarBase {
public:
    typedef std::shared_ptr<ConfigVar> ptr;
    ConfigVar(const std::string& name
              ,const T& default_value
              ,const std::string& description = "") 
        :ConfigVarBase(name, description)
        ,m_val(default_value) {

    }

    std::string toString() override {
        try {
            return boost::lexical_cast<std::string>(m_val);
        } catch (std::exception& e) {
            WILLE_LOG_ERROR(WILLE_LOG_ROOT()) << "ConfigVar::toString exception"
                << e.what() << " convert: " << typeid(m_val).name() << "to string";
        }
        return "";
    }

    bool fromString(const std::string& val) override {
        try {
            m_val = boost::lexical_cast<T>(val);
            return true;
        } catch (std::exception& e) {
            WILLE_LOG_ERROR(WILLE_LOG_ROOT()) << "ConfigVar::fromString exception"
                << e.what() << " convert: " << typeid(m_val).name() << "to string";
        }
        return false;
    }

    const T getValue() const { return m_val; }
    void setValue(const T& v) { m_val = v; }

private:
    T m_val;
};

class Config {
public:
    typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name,
            const T& default_value, const std::string& description = "") {
        auto tmp = Lookup<T>(name);
        if(tmp) {
            WILLE_LOG_INFO(WILLE_LOG_ROOT()) << "Lookup name" << name << " exists";
            return tmp;
        }

        if (name.find_first_not_of("abcdefghikjlmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._0123456789")
                != std::string::npos)  {
            WILLE_LOG_ERROR(WILLE_LOG_ROOT()) << "Lookup name invalid " << name;
            throw std::invalid_argument(name);
        }

        typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
        s_data[name] = v;
        return v;
    }

    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name) {
        auto it = s_data.find(name);
        if(it == s_data.end()) {
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
    }

    static void LoadFromYaml(const YAML::Node& root);

    static ConfigVarBase::ptr LookupBase(const std::string& name);
private:
    static ConfigVarMap s_data;
};

}

#endif
