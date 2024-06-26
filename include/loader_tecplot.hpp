//
// Created by L. Nagy on 27/06/2023.
//

#ifndef MFC_INCLUDE_LOADER_TECPLOT_HPP_
#define MFC_INCLUDE_LOADER_TECPLOT_HPP_

#include <exception>
#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <optional>
#include <regex>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "utilities.hpp"
#include "fraction.hpp"
#include "model.hpp"
#include "field.hpp"

/**
 * Object that will be thrown on tecplot file '*.tec" loading
 * exception.
 */
class TecplotFileLoaderException : std::exception {

 public:

  /**
   * Constructor, will create a new exception object.
   * @param message the exception message.
   */
  explicit
  TecplotFileLoaderException(std::string message) :
      _message(std::move(message)) {}

  [[nodiscard]] const char *
  what() const noexcept override {

    return _message.c_str();

  }

 private:

  std::string _message;

};

/**
 * Temporary tecplot data class.
 */
class TecplotData {

 public:

  TecplotData() = default;

  [[nodiscard]] size_t n_verts() const { return _n_verts.value(); }

  [[nodiscard]] size_t n_elems() const { return _n_elems.value(); }

  [[nodiscard]] size_t n_zones() const { return _n_zones.value(); }

  [[nodiscard]] std::chrono::minutes processing_time() const { return _processing_time; };

  [[nodiscard]] const std::vector<double> &x() const { return _x; }

  [[nodiscard]] const std::vector<double> &y() const { return _y; }

  [[nodiscard]] const std::vector<double> &z() const { return _z; }

  [[nodiscard]] const std::vector<size_t> &tetra_submesh_idxs() const { return _tetra_submesh_idxs; }

  [[nodiscard]] const std::vector<size_t> &tetra_idxs() const { return _tetra_idxs; }

  [[nodiscard]] const std::vector<std::vector<double>> &mx() const { return _mx; }

  [[nodiscard]] const std::vector<std::vector<double>> &my() const { return _my; }

  [[nodiscard]] const std::vector<std::vector<double>> &mz() const { return _mz; }

  [[nodiscard]] std::vector<std::array<double, 3>>
  get_verts() const {

    std::vector<std::array<double, 3>> verts(n_verts());

    for (size_t i = 0; i < n_verts(); ++i) {
      verts[i] = {_x[i], _y[i], _z[i]};
    }

    return verts;

  }

  [[nodiscard]] std::vector<std::array<size_t, 4>>
  get_elements() const {

    std::vector<std::array<size_t, 4>> elements(n_elems());

    for (size_t i = 0; i < n_elems(); ++i) {
      elements[i] = {
          _tetra_idxs[4*i + 0],
          _tetra_idxs[4*i + 1],
          _tetra_idxs[4*i + 2],
          _tetra_idxs[4*i + 3]
      };
    }

    return elements;

  }

  [[nodiscard]] std::vector<size_t>
  get_submesh_idxs() const {

    std::vector<size_t> submesh_idxs(n_elems());

    for (size_t i = 0; i < n_elems(); ++i) {
      submesh_idxs[i] = _tetra_submesh_idxs[i];
    }

    return submesh_idxs;

  }

  [[nodiscard]] FieldList
  get_fields() const {

    FieldList field_list;

    for (size_t zone_idx = 0; zone_idx < n_zones(); ++zone_idx) {

      fv_list field(n_verts());
      for (size_t i = 0; i < n_verts(); ++i) {
        field[i] = {_mx[zone_idx][i], _my[zone_idx][i], _mz[zone_idx][i]};
      }

      field_list.add_field(Field{field});

    }

    return field_list;

  }


  // Exceptions.

  class XCountException : public std::exception {
   public:
    [[nodiscard]] const char * what() const  throw() final {
      return "Incorrect number of vertex x-components.";
    }
  };

  class YCountException : public std::exception {
   public:
    [[nodiscard]] const char * what() const throw() final {
      return "Incorrect number of vertex y-components.";
    }
  };

  class ZCountException : public std::exception {
   public:
    [[nodiscard]] const char * what() const throw() final {
      return "Incorrect number of vertex z-components.";
    }
  };

  class TetraIdxCountException : public std::exception {
   public:
    [[nodiscard]] const char * what() const throw() final {
      return "Incorrect number of tetrahedral indices.";
    }
  };

  class TetraSubmeshIdxCountException : public std::exception {
   public:
    [[nodiscard]] const char * what() const throw() final {
      return "Incorrect number of tetrahedral submesh indices";
    }
  };

  class MxZoneCountException : public std::exception {
   public:
    [[nodiscard]] const char * what() const throw() final {
      return "Incorrect number of magnetization x-component zones.";
    }
  };

  class MyZoneCountException : public std::exception {
   public:
    [[nodiscard]] const char * what() const throw() final {
      return "Incorrect number of magnetization y-component zones.";
    }
  };

  class MzZoneCountException : public std::exception {
   public:
    [[nodiscard]] const char * what() const throw() final {
      return "Incorrect number of magnetization z-component zones.";
    }
  };

  class MxComponentCountException : public std::exception {
   public:
    [[nodiscard]] const char * what() const throw() final {
      return "Incorrect number of magnetization x-components.";
    }
  };

  class MyComponentCountException : public std::exception {
   public:
    [[nodiscard]] const char * what() const throw() final {
      return "Incorrect number of magnetization y-components.";
    }
  };

  class MzComponentCountException : public std::exception {
   public:
    [[nodiscard]] const char * what() const throw() final {
      return "Incorrect number of magnetization z-components.";
    }
  };

  std::optional<size_t> _n_verts;
  std::optional<size_t> _n_elems;
  std::optional<size_t> _n_zones;

  std::optional<size_t> _current_field_idx;

  std::vector<double> _x;
  std::vector<double> _y;
  std::vector<double> _z;

  std::vector<size_t> _tetra_submesh_idxs;
  std::vector<size_t> _tetra_idxs;

  std::vector<std::vector<double>> _mx;
  std::vector<std::vector<double>> _my;
  std::vector<std::vector<double>> _mz;

  std::vector<std::string> _zone_titles;

  std::chrono::minutes _processing_time{};

  friend class TecplotReader;

  [[nodiscard]] bool x_is_full() const {
    return _x.size() >= _n_verts.value();
  }

  [[nodiscard]] bool y_is_full() const {
    return _y.size() >= _n_verts.value();
  }

  [[nodiscard]] bool z_is_full() const {
    return _z.size() >= _n_verts.value();
  }

  [[nodiscard]] bool current_mx_is_full() const {
    return _mx[_current_field_idx.value()].size() >= _n_verts.value();
  };

  [[nodiscard]] bool current_my_is_full() const {
    return _my[_current_field_idx.value()].size() >= _n_verts.value();
  }

  [[nodiscard]] bool current_mz_is_full() const {
    return _mz[_current_field_idx.value()].size() >= _n_verts.value();
  }

  [[nodiscard]] bool tetra_submesh_idx_is_full() const {
    return _tetra_submesh_idxs.size() >= _n_elems.value();
  }

  [[nodiscard]] bool tetra_idx_is_full() const {
    return _tetra_idxs.size() >= _n_elems.value() * 4;
  }

  void finish_object() {

    _n_zones = _zone_titles.size();

    validate_object();

  }

  void validate_object() {

    // The number of vertices must be consistent.
    if (_n_verts.value() != _x.size()) throw XCountException();
    if (_n_verts.value() != _y.size()) throw YCountException();
    if (_n_verts.value() != _z.size()) throw ZCountException();

    // The number of elements must be consistent.
    if (4*_n_elems.value() != _tetra_idxs.size()) throw TetraIdxCountException();
    if (_n_elems.value() != _tetra_submesh_idxs.size()) throw TetraSubmeshIdxCountException();

    // Check that the number of zones is consistent.
    if (_n_zones.value() != _mx.size()) throw MxZoneCountException();
    if (_n_zones.value() != _my.size()) throw MyZoneCountException();
    if (_n_zones.value() != _mz.size()) throw MzZoneCountException();

    for (size_t i = 0; i < _n_zones; ++i) {
      if (_mx[i].size() != _n_verts.value()) throw MxComponentCountException();
      if (_my[i].size() != _n_verts.value()) throw MyComponentCountException();
      if (_mz[i].size() != _n_verts.value()) throw MzComponentCountException();
    }

  }

};

/**
 * Class to load a tecplot file.
 */
class TecplotFileLoader {

 public:

  /**
   * Default constructor.
   */
  TecplotFileLoader() = default;

  /**
 * Function that will read a file and produce a Model object.
 * @param file_name the name of the file.
 * @return a new model object, this object will only contain Mesh information.
 */
  static Model
  read(const std::string &file_name) {

    TecplotData curves;

    std::string line;
    std::fstream fin(file_name);

    size_t zone_counter = 0;

    std::regex _regex_zone = new_regex_zone();
    std::regex _regex_float_line = new_regex_float_line();
    std::regex _regex_int_line = new_regex_int_line();

    std::smatch match_zone;
    std::smatch match_zone_title;
    std::smatch match_int_line;
    std::smatch match_float_line;

    auto start = std::chrono::high_resolution_clock::now();

    while (std::getline(fin, line)) {

      //////////////////////////////////////////////////////////////////////////
      // Handle matching a 'ZONE' line.                                       //
      //////////////////////////////////////////////////////////////////////////

      if (std::regex_match(line, match_zone, _regex_zone)) {

        std::string str_n_verts = match_zone[2].str();
        std::string str_n_elems = match_zone[3].str();

        zone_counter++;

        std::cout << "Processing zone: " << zone_counter << " " << std::endl;

        if (zone_counter == 1) {

          // this is the first zone.

          curves._n_verts = std::stoi(str_n_verts);
          curves._n_elems = std::stoi(str_n_elems);

          curves._tetra_idxs.reserve(4 * curves._n_elems.value());
          curves._tetra_submesh_idxs.reserve(curves._n_elems.value());

          curves._x.reserve(curves._n_verts.value());
          curves._y.reserve(curves._n_verts.value());
          curves._z.reserve(curves._n_verts.value());

          curves._current_field_idx = 0;
          curves._mx.emplace_back();
          curves._my.emplace_back();
          curves._mz.emplace_back();

          curves._mx[curves._current_field_idx.value()].reserve(curves._n_verts.value());
          curves._my[curves._current_field_idx.value()].reserve(curves._n_verts.value());
          curves._mz[curves._current_field_idx.value()].reserve(curves._n_verts.value());

        } else {

          // This is not the first zone.

          if ((!curves._n_verts.has_value())
              && (!curves._n_elems.has_value())) {
            throw std::runtime_error(
                "Parsing subsequent zone, but no. of vertices/elements is not set.");
          } else {
            if (curves._n_verts.value() != std::stoi(str_n_verts)) {
              throw std::runtime_error("Unexpected number of vertices in zone.");
            }
            if (curves._n_elems.value() != std::stoi(str_n_elems)) {
              throw std::runtime_error("Unexpected number of elements in zone.");
            }
          }

          curves._current_field_idx.value()++;
          curves._mx.emplace_back();
          curves._my.emplace_back();
          curves._mz.emplace_back();

          curves._mx[curves._current_field_idx.value()].reserve(curves._n_verts.value());
          curves._my[curves._current_field_idx.value()].reserve(curves._n_verts.value());
          curves._mz[curves._current_field_idx.value()].reserve(curves._n_verts.value());

        }

        // Process the ZONE title that contains Br & Bb field values.

        std::string str_zone_title = match_zone[1].str();

        curves._zone_titles.push_back(str_zone_title);

        continue;

      }

      //////////////////////////////////////////////////////////////////////////////////////////////////////
      // Handle matching a line of integer values.                                                        //
      //////////////////////////////////////////////////////////////////////////////////////////////////////

      if (std::regex_match(line, match_int_line, _regex_int_line)) {

        std::vector<std::string> str_values = regex_split(line);

        if (zone_counter == 1) {

          for (const auto &str_value : str_values) {
            if (!str_value.empty()) {
              if (!curves.tetra_submesh_idx_is_full()) {
                curves._tetra_submesh_idxs.push_back(std::stoull(str_value));
              } else if (!curves.tetra_idx_is_full()) {
                curves._tetra_idxs.push_back(std::stoull(str_value) - 1);
              } else {
                throw std::runtime_error("Too many integers for zone.");
              }
            }
          }

        } else {

          // This is not the first zone.
          throw std::runtime_error(
              "Integers should only be found in the first zone.");

        }

        continue;

      }

      //////////////////////////////////////////////////////////////////////////////////////////////////////
      // Handle matching a line of floating point values.                                                 //
      //////////////////////////////////////////////////////////////////////////////////////////////////////

      if (std::regex_match(line, match_float_line, _regex_float_line)) {

//      std::cout << "FLOAT LINE: " << line << "\n";

        std::vector<std::string> str_values = regex_split(line);

        if (zone_counter == 1) {

          // This is the first zone.

          for (const auto &str_value : str_values) {
            if (!str_value.empty()) {
              if (!curves.x_is_full()) {
                curves._x.push_back(std::stod(str_value));
              } else if (!curves.y_is_full()) {
                curves._y.push_back(std::stod(str_value));
              } else if (!curves.z_is_full()) {
                curves._z.push_back(std::stod(str_value));
              } else if (!curves.current_mx_is_full()) {
                curves._mx[curves._current_field_idx.value()].push_back(
                    std::stod(str_value));
              } else if (!curves.current_my_is_full()) {
                curves._my[curves._current_field_idx.value()].push_back(
                    std::stod(str_value));
              } else if (!curves.current_mz_is_full()) {
                curves._mz[curves._current_field_idx.value()].push_back(
                    std::stod(str_value));
              } else {
                throw std::runtime_error("Too many doubles for zone.");
              }
            }
          }

        } else {

          // This is not the first zone.

          for (const auto &str_value : str_values) {
            if (!str_value.empty()) {
              if (!curves.current_mx_is_full()) {
                curves._mx[curves._current_field_idx.value()].push_back(
                    std::stod(str_value));
              } else if (!curves.current_my_is_full()) {
                curves._my[curves._current_field_idx.value()].push_back(
                    std::stod(str_value));
              } else if (!curves.current_mz_is_full()) {
                curves._mz[curves._current_field_idx.value()].push_back(
                    std::stod(str_value));
              } else {
                throw std::runtime_error("Too many doubles for zone.");
              }
            }
          }

        }

        continue;

      }

    }

    auto stop = std::chrono::high_resolution_clock::now();

    curves._processing_time =
        std::chrono::duration_cast<std::chrono::minutes>(stop - start);

    curves.finish_object();

    return {
      curves.get_verts(),
      curves.get_elements(),
      curves.get_submesh_idxs(),
      curves.get_fields()
    };

  }

 private:

  /**
   * Return a regular expression that will match a zone line.
   */
  static std::regex
  new_regex_zone() {
    return std::regex{
      R"r(^\s*ZONE\s*T\s*=\s*"([A-Za-z0-9=\-.,;\s]+)?"\s*,?\s*N\s*=\s*([0-9]+)\s*,?\s*E\s*=\s*([0-9]+)\s*$)r"
    };
  }

  static std::regex
  new_regex_int_line() {
    return std::regex{
      R"r(^\s*([0-9]+)(\s+[0-9]+)*\s*$)r"
    };
  }

  static std::regex
  new_regex_float_line() {
    return std::regex{
      R"r(^\s*([-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?)(\s+([-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?))*\s*$)r"
    };
  }

};

#endif // MFC_INCLUDE_LOADER_TECPLOT_HPP_
