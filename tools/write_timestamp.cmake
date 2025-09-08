string(TIMESTAMP timestamp "%Y%m%d%H%M%S")
file(WRITE "${CMAKE_CURRENT_LIST_DIR}/../main_timestamp.go" "package main\n// ${timestamp}\n")
