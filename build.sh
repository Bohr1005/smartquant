case $1 in
    debug)
        echo "====== Build Debug Version ======"
        if [ ! -d "Debug/" ]; then
            mkdir Debug
        fi
        cd Debug
        cmake -DCMAKE_BUILD_TYPE=Debug ..
        make
        ;;
    release)
        echo "====== Build Release Version ======"
        if [ ! -d "Release/" ]; then
            mkdir Release
        fi
        cd Release
        cmake -DCMAKE_BUILD_TYPE=Release ..
        make
        ;;
    clean)
        rm -rf Debug
        rm -rf Release
        rm -rf ${project_path}/3rd/cppzmq/build/
        rm -rf ${project_path}/3rd/libzmq-master/build/
        ;;
    *)
        echo "====== Build Release Version ======"
        if [ ! -d "${project_path}/Release/" ]; then
            mkdir Release
        fi
        cd Release
        cmake -DCMAKE_BUILD_TYPE=Release ..
        make
esac
