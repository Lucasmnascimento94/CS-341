################################################################################################################                                                   
#  Check dependencies on Linux Environment  
check_dependencies(){ 
    DEP="$PWD/dependencies/dependencies.txt" # txt File which holds necessary packages for the program to run
    verbose="${1:-nv}" # Verbose Mode <Default !Verbose>

    while read -r line || [ -n "$line" ];
    do
        if IFS= command -v "$line" >/dev/null;
        then
            if [[ "$verbose" == "v" ]] || [[ "$verbose" == "-v" ]] || [[ "$verbose" == "V" ]] || [[ "$verbose" == "-V" ]];
            then
                echo "Dependencie Already Installed -> $line"
            fi

        else
            echo "$line Not installed..."
            echo "Installing dependencie... $line"
            brew install $line
        fi
    done <"$DEP"
}

check_dependencies "$1"
