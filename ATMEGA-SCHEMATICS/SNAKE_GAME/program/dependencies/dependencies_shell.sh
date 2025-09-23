check_dependencies(){
    verbose="${1:-nv}"
    DEP="$PWD/dependencies/dependencies.txt"

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
            sudo apt install $line
        fi
    done <"$DEP"
}

check_dependencies "$1"