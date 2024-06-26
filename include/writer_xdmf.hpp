//
// Created by Lesleis Nagy on 25/06/2024.
//

#ifndef MFC_INCLUDE_WRITER_XDMF_HPP_
#define MFC_INCLUDE_WRITER_XDMF_HPP_

#include <exception>
#include <fstream>
#include <string>
#include <sstream>

#include <rapidxml_ext.hpp>

#include "aliases.hpp"
#include "model.hpp"

class XDMFFileWriterException : std::exception {

 public:

  /**
   * Constructor, will create a new exception object.
   * @param message the exception message.
   */
  explicit
  XDMFFileWriterException(std::string
  message) :
  _message (std::move(message)) {}

  [[nodiscard]] const char *
  what() const noexcept override {

    return _message.c_str();

  }

 private:

  std::string _message;

};

class XDMFFileWriter {

 public:

  /**
   * Default constructor.
   */
  XDMFFileWriter() = default;

  /**
   * Function that will write a file.
   */
  static void
  write(const std::string &file_name,
        const std::string &hdf5_file_name,
        const Model &model) {

    using namespace rapidxml;

    std::string nodes_per_element = "4";
    std::string precision = "8";
    std::string format = "HDF";
    std::string dtype_float = "Float";
    std::string dtype_uint = "UInt";

    std::stringstream ss_no_of_verts;
    std::stringstream ss_no_of_elems;

    std::stringstream ss_no_of_verts_x3;
    std::stringstream ss_no_of_elems_x1;
    std::stringstream ss_no_of_elems_x4;

    std::stringstream ss_mesh_elements;
    std::stringstream ss_mesh_vertices;
    std::stringstream ss_mesh_submesh;

    std::stringstream ss_magnetizations;

    ss_no_of_verts << model.mesh().vcl().size();
    ss_no_of_elems << model.mesh().til().size();
    ss_no_of_verts_x3 << model.mesh().vcl().size() << " 3";
    ss_no_of_elems_x1 << model.mesh().til().size() << " 1";
    ss_no_of_elems_x4 << model.mesh().til().size() << " 4";

    ss_mesh_elements << hdf5_file_name << ":/mesh/elements";
    ss_mesh_vertices << hdf5_file_name << ":/mesh/vertices";
    ss_mesh_submesh << hdf5_file_name << ":/mesh/submesh";

    std::string dim_no_of_verts = ss_no_of_verts.str();
    std::string dim_no_of_elems = ss_no_of_elems.str();

    std::string dim_no_of_verts_x3 = ss_no_of_verts_x3.str();
    std::string dim_no_of_elems_x1 = ss_no_of_elems_x1.str();
    std::string dim_no_of_elems_x4 = ss_no_of_elems_x4.str();

    std::string mesh_elements = ss_mesh_elements.str();
    std::string mesh_vertices = ss_mesh_vertices.str();
    std::string mesh_submesh = ss_mesh_submesh.str();

    std::string magnetizations = ss_magnetizations.str();

    xml_document<> doc;

    // Create a declaration node.
    xml_node <> *decl = doc.allocate_node(rapidxml::node_declaration);
    decl->append_attribute(doc.allocate_attribute("version", "1.0"));
    doc.append_node(decl);

    // Create /Xdmf node.
    xml_node <> *xdmf = doc.allocate_node(rapidxml::node_element, "Xdmf");
    xdmf->append_attribute(doc.allocate_attribute("Version", "3.0"));
    doc.append_node(xdmf);

    // Create /Xdmf/Domain node.
    xml_node <> *domain = doc.allocate_node(rapidxml::node_element, "Domain");
    xdmf->append_node(domain);

    // Create Xdmf/Domain/Grid node
    xml_node <> *mesh_grid = doc.allocate_node(rapidxml::node_element, "Grid");
    mesh_grid->append_attribute(doc.allocate_attribute("GridType", "Collection"));
    mesh_grid->append_attribute(doc.allocate_attribute("CollectionType", "Temporal"));
    domain->append_node(mesh_grid);

    size_t time_index = 0;
    std::vector<std::string> time_indices(model.field_list().fields().size());
    std::vector<std::string> vector_fields_paths(model.field_list().fields().size());
    for (const auto &field : model.field_list().fields()) {

      // Create Xdmf/Domain/Grid/Grid node
      xml_node <> *field_grid = doc.allocate_node(rapidxml::node_element, "Grid");
      field_grid->append_attribute(doc.allocate_attribute("Name", "m"));
      field_grid->append_attribute(doc.allocate_attribute("GridType", "Uniform"));
      mesh_grid->append_node(field_grid);
      //field_grids.push_back(field_grid);

      // Create Xdmf/Domain/Grid/Grid/Topology node
      xml_node <> *topology = doc.allocate_node(rapidxml::node_element, "Topology");
      topology->append_attribute(doc.allocate_attribute("TopologyType", "Tetrahedron"));
      topology->append_attribute(doc.allocate_attribute("NumberOfElements", dim_no_of_elems.c_str()));
      topology->append_attribute(doc.allocate_attribute("NodesPerElement", "4"));
      field_grid->append_node(topology);
      //topologies.push_back(topology);

      // Create Xdmf/Domain/Grid/Grid/Topology/DataItem node
      xml_node <> *topo_data_item = doc.allocate_node(node_element, "DataItem", mesh_elements.c_str());
      topo_data_item->append_attribute(doc.allocate_attribute("Format", "HDF"));
      topo_data_item->append_attribute(doc.allocate_attribute("DataType", "Int"));
      topo_data_item->append_attribute(doc.allocate_attribute("Precision", "8"));
      topo_data_item->append_attribute(doc.allocate_attribute("Dimensions", dim_no_of_elems_x4.c_str()));
      topology->append_node(topo_data_item);
      //topo_data_items.push_back(topo_data_item);

      // Create /Xdmf/Domain/Grid/Grid/Geometry
      xml_node <> *geometry = doc.allocate_node(node_element, "Geometry");
      geometry->append_attribute(doc.allocate_attribute("GeometryType", "XYZ"));
      field_grid->append_node(geometry);
      //geometries.push_back(geometry);

      // Create Xdmf/Domain/Grid/Grid/Geometry/DataItem node
      xml_node <> *geom_data_item = doc.allocate_node(node_element, "DataItem", mesh_vertices.c_str());
      geom_data_item->append_attribute(doc.allocate_attribute("Format", "HDF"));
      geom_data_item->append_attribute(doc.allocate_attribute("DataType", "Float"));
      geom_data_item->append_attribute(doc.allocate_attribute("Precision", "8"));
      geom_data_item->append_attribute(doc.allocate_attribute("Dimensions", dim_no_of_verts_x3.c_str()));
      geometry->append_node(geom_data_item);

      // Create /Xdmf/Domain/Grid/Grid/Attribute (Name="sid")
      xml_node <> *attribute_sid = doc.allocate_node(node_element, "Attribute");
      attribute_sid->append_attribute(doc.allocate_attribute("Name", "sid"));
      attribute_sid->append_attribute(doc.allocate_attribute("AttributeType", "Scalar"));
      attribute_sid->append_attribute(doc.allocate_attribute("Center", "Cell"));
      field_grid->append_node(attribute_sid);

      // Create /Xdmf/Domain/Grid/Grid/Attribute/DataItem
      xml_node<> *attr_sid_data_item = doc.allocate_node(node_element, "DataItem", mesh_submesh.c_str());
      attr_sid_data_item->append_attribute(doc.allocate_attribute("Format", "HDF"));
      attr_sid_data_item->append_attribute(doc.allocate_attribute("DataType", "Int"));
      attr_sid_data_item->append_attribute(doc.allocate_attribute("Precision", "8"));
      attr_sid_data_item->append_attribute(doc.allocate_attribute("Dimensions", dim_no_of_elems.c_str()));
      attribute_sid->append_node(attr_sid_data_item);

      // Create /Xdmf/Domain/Grid/Grid/Time
      time_indices[time_index] = std::to_string(time_index);
      xml_node <> *time = doc.allocate_node(node_element, "Time");
      time->append_attribute(doc.allocate_attribute("Value", time_indices[time_index].c_str()));
      field_grid->append_node(time);

      // Create /Xdmf/Domain/Grid/Grid/Attribute
      xml_node <> *attribute_field = doc.allocate_node(rapidxml::node_element, "Attribute");
      attribute_field->append_attribute(doc.allocate_attribute("Name", "m"));
      attribute_field->append_attribute(doc.allocate_attribute("AttributeType", "Vector"));
      attribute_field->append_attribute(doc.allocate_attribute("Center", "Node"));
      field_grid->append_node(attribute_field);

      // Create /Xdmf/Domain/Grid/Grid/Attribute/DataItem
      std::stringstream ss_field;
      ss_field << hdf5_file_name << ":/fields/field" << time_index << "/vectors";
      vector_fields_paths[time_index] = ss_field.str();
      xml_node <> *attr_field_data_item = doc.allocate_node(node_element, "DataItem", vector_fields_paths[time_index].c_str());
      attr_field_data_item->append_attribute(doc.allocate_attribute("Format", "HDF"));
      attr_field_data_item->append_attribute(doc.allocate_attribute("DataType", "Float"));
      attr_field_data_item->append_attribute(doc.allocate_attribute("Precision", "8"));
      attr_field_data_item->append_attribute(doc.allocate_attribute("Dimensions", dim_no_of_verts_x3.c_str()));
      attribute_field->append_node(attr_field_data_item);

      time_index = time_index + 1;

    }

    // Print the XML document to a string
    std::string xmlString;
    rapidxml::print(std::back_inserter(xmlString), doc, rapidxml::print_no_indenting);

    // Save the XML document to a file
    std::ofstream file(file_name);
    file << doc;
    file.close();

    // Clean up the memory
    doc.clear();

  }

};

#endif //MFC_INCLUDE_WRITER_XDMF_HPP_