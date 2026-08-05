#!/bin/bash

set -e

projects=""
for file in ${ALL_CHANGED_FILES}; do
    if [[ \
        "$file" == *"board_configs/"* || "$file" == *"capi/"* || "$file" == *"cmake/"* || "$file" == *"drivers/"* \
        || "$file" == *"iio/"* || "$file" == *"include/"* || "$file" == *"jesd204/"* || "$file" == *"libraries/"* \
        || "$file" == *"network/"* || "$file" == *"tests/"* || "$file" == *"tools/"* || "$file" == *"util/"* \
        || "$file" == *"workflows/"* || "$file" == "CMakeLists.txt" || "file" == "CMakePresets.json" || "$file" == "Kconfig" ]]; \
        then
              
        echo "Dependecy paths were modified. Building whole projects."
        echo "projects=all" >> $GITHUB_OUTPUT
              
        break
    else
        project=$(echo $file | cut -d "/" -f2)
        projects="$projects $project" 
        echo "projects=$projects" >> $GITHUB_OUTPUT
    fi
done
