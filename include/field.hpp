//
// Created by Lesleis Nagy on 15/06/2024.
//

#ifndef MFC_INCLUDE_FIELD_HPP_
#define MFC_INCLUDE_FIELD_HPP_

#include <utility>

#include <utility>

#include "aliases.hpp"

/**
 * Holds a field - which is a collection of vectors associated with vertices.
 */
class Field {

 public:

  /**
   * Create a new Field object with `n` vectors and a blank annotation.
   * @param n the number of vectors.
   */
  explicit Field(size_t n) :
      _vectors{n} {}

  /**
   * Create a new Field object with `n` vectors and a annotation.
   * @param annotation a annotation for the vector field.
   * @param n the number of vectors.
   */
  Field(std::string annotation, size_t n) :
      _annotation{std::move(annotation)},
      _vectors{n} {}

  /**
   * Create a new Field object from a list of vectors, with a blank annotation.
   * @param vectors the vectors of the field.
   */
  explicit Field(fv_list vectors) :
      _vectors{std::move(vectors)} {}

  /**
   * Create a new Field object from a list list of vectors with a annotation.
   * @param annotation annotation for the vector field.
   * @param vectors the vectors of the field.
   */
  Field(std::string annotation, fv_list vectors) :
      _annotation{std::move(annotation)},
      _vectors{std::move(vectors)} {}

  /**
   * Retrieve a const list of vectors.
   * @return the vectors comprising the field.
   */
  [[nodiscard]] const fv_list &
  vectors() const { return _vectors; }

  /**
   * Retrieve a list of vectors.
   * @return the vectors comprising the field.
   */
  fv_list &
  vectors() { return _vectors; }

  /**
   * Retrieve the annotation of the field.
   * @return the annotation of the field.
   */
  [[nodiscard]] const std::string &
  annotation() const { return _annotation; }

 private:

  // The field's annotation.
  std::string _annotation;

  // The field's vectors.
  fv_list _vectors;

};

/**
 * Holds a collection of fields.
 */
class FieldList {

 public:

  /**
   * A default constructor.
   */
  FieldList() = default;

  /**
   * Retrieve the fields associated with this field list.
   * @return the fields in this field list.
   */
  [[nodiscard]] const std::vector<Field> &
  fields() const { return _fields; }

  /**
   * Retrieve the fields associated with this field list.
   * @return the fields in this field list.
   */
  std::vector<Field> &
  fields() { return _fields; }

  /**
   * Add a field to this field list.
   * @param field the field t add.
   */
  void
  add_field(Field field) { _fields.push_back(std::move(field)); }

  /**
   * Retrieve the count of fields in this field list.
   * @return the number of fields in this list.
   */
  [[nodiscard]] size_t
  n_fields() const { return _fields.size(); }

 private:

  std::vector<Field> _fields;

};

#endif //MFC_INCLUDE_FIELD_HPP_
