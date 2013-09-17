project(odf-library CXX)

file(GLOB odf_SOURCES "*.cpp")

#
# Configure make hoooks.
#
set(ODF_SHARED_LIBRARY ${ODF_LIBRARY_TARGET})
set(ODF_STATIC_LIBRARY odf-static)

#
# Shared library.
#
add_library(${ODF_SHARED_LIBRARY} SHARED ${odf_SOURCES})
target_link_libraries(${ODF_SHARED_LIBRARY} ${ODF_LINK_LIBRARIES})

set_target_properties(
    ${ODF_SHARED_LIBRARY}
        PROPERTIES
            VERSION
                ${LIBRARY_VERSION}
            SOVERSION
                ${LIBRARY_SOVERSION}
            OUTPUT_NAME
                odf
            DEFINE_SYMBOL
                ODF_EXPORTS
)

install(
    TARGETS ${ODF_SHARED_LIBRARY}
    RUNTIME DESTINATION ${BIN_INSTALL_DIR} COMPONENT odf-shared
    LIBRARY DESTINATION ${LIB_INSTALL_DIR} COMPONENT odf-shared
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT odf-shared
)

#
# Static library.
#

add_library(${ODF_STATIC_LIBRARY} STATIC EXCLUDE_FROM_ALL ${odf_SOURCES})
target_link_libraries(${ODF_STATIC_LIBRARY} ${ODF_LINK_LIBRARIES})

set_target_properties(
    ${ODF_STATIC_LIBRARY}
        PROPERTIES
            VERSION
                ${LIBRARY_VERSION}
            SOVERSION
                ${LIBRARY_SOVERSION}
            OUTPUT_NAME
                odf
)
