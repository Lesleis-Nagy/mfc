add_executable(tec2hdf5 main.cpp)

target_include_directories(tec2hdf5
    PUBLIC ${MFC_INCLUDE_DIR}
           ${ARGS_INCLUDE_DIR}
           ${RAPIDXML_INCLUDE_DIR}
           ${HDF5_INCLUDE_DIRS})

target_link_libraries(tec2hdf5
        ${HDF5_LIBRARIES}
        ${HDF5_HL_LIBRARIES})
