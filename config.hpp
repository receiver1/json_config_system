#pragma once

#include "nlohmann_json/json.hpp"
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace config {

/*
* Pure virtual class needed for polymorphism
*/
class ConfigVariableBase_t {
protected:
  std::string key_{};

public:
  ConfigVariableBase_t() = default;
  ConfigVariableBase_t(const std::string key) : key_{key} {}

  virtual void load(const nlohmann::json &object) = 0;
  virtual void save(nlohmann::json &object) const = 0;
};

class Config {
  std::filesystem::path path_{std::filesystem::current_path()};
  std::vector<ConfigVariableBase_t *> variables_;

public:
  /*
  * Adds a variables in storage
  * \param[in] variable Base variable class
  */
  void add_variable(ConfigVariableBase_t *const variable) {
    variables_.push_back(variable);
  }

  /*!
  * Saves stored variables in json
  * \return Store containing json
  */
  std::string get() const {
    nlohmann::json object{};
    for (auto &variable : variables_)
      variable->save(object);

    return object.dump();
  }

  /*!
  * Loads stored variables from json
  * \param[in] json Store containing json
  */
  void set(std::string json) {
    const auto object{nlohmann::json::parse(json)};
    if (!object.is_object())
      return;

    for (auto &variable : variables_)
      variable->load(object);
  }

  /*
  * Loads configuration from file
  * \param[in] file_name File name
  */
  void load(std::string file_name) {
    std::filesystem::create_directory(path_);

    std::ifstream ifs{path_ / file_name, std::ios::in | std::ios::binary};
    ifs.unsetf(std::ios::skipws);
    if (ifs.is_open()) {
      std::string str{};
      std::copy(std::istreambuf_iterator<char>(ifs),
                std::istreambuf_iterator<char>(), std::back_inserter(str));

      this->set(str);
    }
  }

  /*
  * Saves configuration in file
  * \param[in] file_name File name, without path
  */
  void save(std::string file_name) const {
    std::filesystem::create_directory(path_);

    std::ofstream ofs{path_ / file_name, std::ios::out | std::ios::binary};
    ofs.unsetf(std::ios::skipws);
    if (ofs.is_open())
      ofs << this->get();
  }
};

static Config instance{};

/*
* Wrapper for variables that handles their loading and saving
* Automatically adds a variable to storage in Config
*/
template <typename T> class ConfigVariable_t : public ConfigVariableBase_t {
  T value_;

public:
  constexpr ConfigVariable_t() = default;
  ConfigVariable_t(const std::string key, const T value)
      : ConfigVariableBase_t{key}, value_{value} {
    instance.add_variable(this);
  }

  void load(const nlohmann::json &object) override {
    if (object.find(key_) == object.end())
      return;

    value_ = object[key_].get<T>();
  }
  void save(nlohmann::json &object) const override { object[key_] = value_; }

  operator T() const { return value_; }
  T *operator&() { return &value_; }
  void set(const T value) { value_ = value; }
  T &get() { return value_; }
  const T &get() const { return value_; }
};

} // namespace config