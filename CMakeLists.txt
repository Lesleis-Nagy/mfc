cmake_minimum_required(VERSION 3.25)
project(mfc)

set(CMAKE_CXX_STANDARD 20)

set(MFC_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(MFC_SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(MFC_THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/third-party")

# Set up third-party tools that we'd like to just include.

set(ARGS_INCLUDE_DIR "${MFC_THIRD_PARTY_DIR}/args-v6.3.0/include")
set(RAPIDXML_INCLUDE_DIR "${MFC_THIRD_PARTY_DIR}/rapidxml-v1.13")

# Find packages.

find_package(HDF5 COMPONENTS CXX HL REQUIRED)

# Descend in to the src subdirectory.
add_subdirectory(${MFC_SRC_DIR})
