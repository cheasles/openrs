#--------------------------------------------------------------------
# Add subdirectories
#--------------------------------------------------------------------

add_subdirectory(cryptopp)
add_subdirectory(frozen)
add_subdirectory(json)

find_path (
    QTL_INCLUDE_DIR
    NAMES "qtl_common.hpp" "qtl_mysql.hpp" "qtl_sqlite.hpp"
    PATHS qtl
    PATH_SUFFIXES include
    DOC "QTL header include directory"
)
